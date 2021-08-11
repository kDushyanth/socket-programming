//gcc server.c -o server -lpthread
//good bye:- to terminate
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#define BACKLOG 10
#define PORT 1234

//parse the message received and return result
char *parse(char *exp,int len){
	
	
	//1-add, 2-sub, 3-mul, 4-div
	char *result = (char *)malloc(sizeof(char)*128);
	
	char a[128],b[128];
	int f=0;int index;
	int minus=0;
	if(exp[0]=='-')minus=1;
	for(int i=0+minus;i<128 && exp[i]!='\0';i++){
		if(exp[i]=='+' || exp[i]=='/' || exp[i]=='*' || exp[i]=='-'){
			f=1;index = i;break;
		}
	}
	if(f==0)return "";
	int i;
	
	for(i=0;i<index;i++){
		a[i] = exp[i];
	}
	a[i]='\0';	
	int j;
	
	for(j=index+1;exp[j]!='\0';j++){
		b[j-index-1] = exp[j];
	}
	b[j-index-1]='\0';
	//printf("%s -- %s",a,b);

	float op1 = strtof(a,NULL);
	float op2 = strtof(b,NULL);
	if(exp[index]=='*'){
		sprintf(result, "%g", op1*op2);
	}
	if(exp[index]=='-'){
		sprintf(result, "%g", op1-op2);
	}
	if(exp[index]=='+'){
		sprintf(result, "%g", op1+op2);
	}
	if(exp[index]=='/'){
		if(op2 ==0)return "";
		sprintf(result, "%g", op1/op2);
	}
	return result;
	
}
void *calc(void *new_sock_fd_v){
	int new_sock_fd = *((int *)new_sock_fd_v);
	char buffer_r[128] = {0}; // received message will be stored in this char array
	char buffer_s[128] = {0}; // sending message will be stored in this char array
	char init_buffer_r[128] = {0}; // initial message from client will be stored in this char array
	char *init_msg = "Hi from server"; 
	int len = strlen(init_msg);
	char *err = "enter valid expression";
	int errlen = strlen(err);
	recv(new_sock_fd,(void *) init_buffer_r,128, 0);
	send(new_sock_fd,init_msg, len, 0);
	printf("%s\n",init_buffer_r );
	
	while(1){
	memset(buffer_r ,0 , 128);
	int num = recv(new_sock_fd,(void *) buffer_r,128, 0);
	if (strcmp(buffer_r, "end")==0 ){
		send(new_sock_fd,"connection closed", 128, 0);
	 		break;
	} 
	printf("Calculating: %s\n", buffer_r);
	
	char* result = parse(buffer_r,num);
	if(strcmp(result, "")==0 ){
		memset(buffer_s ,0 , 128);
		send(new_sock_fd,(void *) err, errlen, 0);
		printf("Sent: %s\n", err);
	}else{
	memset(buffer_s ,0 , 128);
	send(new_sock_fd,result, 128, 0);
	printf("Sent: %s\n", result);
	}
	}
	close(new_sock_fd);
}


struct some{
	 	int info_sock_fd;
	 	struct sockaddr_in info_add_port;
	 	socklen_t info_len;
};
void *connect_fun(void *connection){
		pthread_t tid;
		struct some *connect_info = ((struct some *)connection);
		int new_sock_fd = accept(connect_info->info_sock_fd,(struct sockaddr *)&(connect_info->info_add_port),(socklen_t *) &(connect_info->info_len));
		if(new_sock_fd==-1){
		 	perror("failed to accept request");
		 	exit(1);
		}
		
		pthread_create(&tid,NULL,calc,(void *)&new_sock_fd);
		pthread_join(tid,NULL);
}


int main(int argc,char *argv[]){
	int sock_fd;
	

	//create IPv4 socket that accepts TCP connection
	 sock_fd = socket(AF_INET,SOCK_STREAM,0);
	 if(sock_fd==-1){
	 	perror("socket failed to create");
	 	exit(1);
	 }
	 int opt=1;
	 // to avoid port already in use error
	 if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	    { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	    } 
	    
	 struct sockaddr_in add_port;
	 int addrlen = sizeof(add_port);
	 
	 add_port.sin_family = AF_INET;
	 add_port.sin_addr.s_addr = INADDR_ANY;// set address to local host
	 add_port.sin_port = htons(PORT);//set port
	 
	 //bind to above specified address and port
	 int b = bind(sock_fd,(struct sockaddr *)&add_port, sizeof(add_port));
	 if(b==-1){
	 	perror("failed to bind socket to specific port and address");
	 	exit(1);
	 }
	 //listen for connections; specify max length of connections queue
	 int l = listen(sock_fd,BACKLOG);
	 if(l==-1){
	 	perror("failed to listen");
	 
	 	exit(1);
	 }
	 
	 
	 //-------------------------
	 struct some connect_info;
	 connect_info.info_sock_fd=sock_fd;
	 connect_info.info_add_port.sin_family = AF_INET;
	 connect_info.info_add_port.sin_addr.s_addr = INADDR_ANY;
	 connect_info.info_add_port.sin_port = htons(PORT);
	 connect_info.info_len = addrlen;
	 printf("enter max connections:");
	 int num_connects;scanf("%d",&num_connects);
	 pthread_t conn_tid[num_connects];
	 for(int i=0;i<num_connects;i++){
	 	pthread_create(&conn_tid[i],NULL,connect_fun,(void *) &connect_info);
	 }
	 for(int i=0;i<num_connects;i++){
	 	pthread_join(conn_tid[i],NULL);
	 }
	 printf("server terminated\n");
	 close(sock_fd);
	 //------------------------------
	 /*
	 pthread_t tid;
	 int new_sock_fd;
		pid_t pid;
		if((pid=fork())==-1){
			perror("fork failed");
		}
	
		new_sock_fd = accept(sock_fd,(struct sockaddr *)&add_port,(socklen_t *) &addrlen);
		if(new_sock_fd==-1){
		 	perror("failed to accept request");
		 	exit(1);
		}
		
		pthread_create(&tid,NULL,calc,(void *)&new_sock_fd);
		//wait for the thread to complete
		pthread_join(tid,NULL);
		if(pid!=0){
			wait(pid);
			printf("server terminated\n");
			close(sock_fd);
		}
		
	*/	
 return 0;
}
