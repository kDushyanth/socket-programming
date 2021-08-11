#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BACKLOG 10
#define PORT 1234
#define SIZE 1024
#define NUM 5

struct stat st = {0};

struct some{
	 	int info_sock_fd;
	 	struct sockaddr_in info_add_port;
	 	socklen_t info_len;
};
struct user{
char username[128];
char password[128];
};


struct user users[NUM];
int curr_index =-1;

void *create_user(){
   
   char msg[16];
   char user[128];
   char pass1[128];
   char pass2[128];
   getchar();
   while(1){
   char server[128]="./fserver/";
   char client[128]="./";
   memset(msg,0,16);
   memset(user,0,128);
   memset(pass1,0,128);
   memset(pass2,0,128);
   
   scanf("%[^\n]%*c",msg);
   if(strcmp(msg,"create user")==0){
   	if(curr_index==NUM-1){
   	  printf("cannot create user, max users reached\n");
   	  break;
   	}
   	printf("enter username:");
   	scanf("%s",user);
   	printf("enter password:");
   	scanf("%s",pass1);
   	printf("confirm password:");
   	scanf("%s",pass2);
   	if(strcmp(pass1,pass2)==0){
              	  curr_index++;
   	   	  strncpy(users[curr_index].username,user,128);
   	   	  strncpy(users[curr_index].password,pass1,128);
   	   	  printf("user created\n"); 
   	   	  if (stat(server, &st) == -1) {
   			 mkdir(server, 0700);
		  }
   	   	  strcat(server,user);
   	   	  strcat(client,user);
   	   	  
   	   	  if (stat(server, &st) == -1) {
   			 mkdir(server, 0700);
		  }
		  if(stat(client,&st)==-1){
       		 mkdir(client,0700);
		  }
   	
   	}else{
   		printf("check password\n");
   	}
   }else{
     
     printf("invalid input\n");
     
     continue;
   }
    getchar();
   
   }

}
void *connect_fun(void *connection){
		pthread_t tid[2];
		struct some *connect_info = ((struct some *)connection);
		int sock_fd = accept(connect_info->info_sock_fd,(struct sockaddr *)&(connect_info->info_add_port),(socklen_t *) &(connect_info->info_len));
		
		if(sock_fd==-1){
		 	perror("failed to accept request");
		 	exit(1);
		}
		
		char init_buffer_s[32]="Hi from server\n";
		char init_buffer_r[128]={0};
		
		
		recv(sock_fd,(void *)init_buffer_r,128, 0);
		printf("Received:%s\n",init_buffer_r);
		send(sock_fd, init_buffer_s, 32, 0);
		
		//authenticate
		char username[128]={0};
		char password[128]={0};
		char err_msg[128]="Invalid username or password";
		char success[128]="Logged In";
		
		int loggedIn=0;
		
		char curr_user[128];
		while(!loggedIn){
		
		memset(username,0,128);
		memset(password,0,128);
		recv(sock_fd,username,128,0);
		recv(sock_fd,password,128,0);
		
		for(int i=0;i<2;i++){
			if(strcmp(username,users[i].username)==0 && strcmp(password,users[i].password)==0){
				send(sock_fd,success,128,0);
				loggedIn=1;
				strncpy(curr_user,username,128);
				break;
			}
		}
		if(loggedIn==0){
				send(sock_fd,err_msg,128,0);
		}
		
		
		}
		printf("curr_user %s\n",curr_user);
		
		char input[64];char op[5];char fname[64];char *done="$D0Ne#";char *ndone="!$D0Ne#";
		int mode=-1;//a-ascii & b-binary
		while(1){
		   
		   memset(input,0,32);memset(op,0,5);memset(fname,0,28);
		   recv(sock_fd,input,64,0);
		   
		   strncpy(op,input+7,4);strncpy(fname,input+12,51);
		   mode = input[5]-'a';
		   
		   FILE *fd;
	           char filepath[100] = "./fserver/";
		   strcat(filepath,curr_user);
		   strcat(filepath,"/");
		   strcat(filepath,fname);
		   printf("file path: %s\n",filepath);
		   
		   if(strcmp(op,"send")==0){
		   if(mode==0){
		   	  int n;
			  
			  char buffer[SIZE];
			  
			  fd = fopen(filepath, "w");
			  
			  while (1) {
			    
				    n = recv(sock_fd, buffer, SIZE, 0);
				    
				      if(buffer[0]=='$' && buffer[1]=='D' && buffer[2]=='0' && buffer[3]=='N' && buffer[4]=='e' && buffer[5]=='#'){
					      printf("File Received\n");
					      fclose(fd); 
					      break;
				      }else{
					    fwrite(buffer,1,sizeof(buffer),fd);
					    memset(buffer,0,SIZE);
			    	      }
  			  } 
		   }else{
		   	if(mode){
		   	  int n;
			  
			  unsigned char buffer[SIZE];
			  
			  fd = fopen(filepath, "wb");
			  
			  while (1) {
			    
				    n = recv(sock_fd, buffer, SIZE, 0);
				    
				      if(buffer[0]=='$' && buffer[1]=='D' && buffer[2]=='0' && buffer[3]=='N' && buffer[4]=='e' && buffer[5]=='#'){
					      printf("File Received\n");
					      fclose(fd); 
					      break;
				      }else{
					    fwrite(buffer,1,sizeof(buffer),fd);
					    memset(buffer,0,SIZE);
			    	      }
  			  } 
		   	
		   	}
		   }
		          			
		   }else{
		     
		     if(strcmp(op,"recv")==0){
		     if(mode==0){
		      		char data[SIZE] ={0};
				 fd = fopen(filepath,"r");
				 				 
				 if(fd!=NULL){
				 stat(filepath, &st);
                            
			     	 long int y= st.st_size;
			     	 char stry[64]={0};
			     	 sprintf(stry, "%ld", y);
			     	 //printf("%s\n",stry);
				 send(sock_fd,done,sizeof(data),0);
				 send(sock_fd,stry,64,0);
				 while(fread(data,1,SIZE,fd)>0){
				 if(send(sock_fd,data,sizeof(data),0)==-1){
				  	perror("Error in sending File");
				  	exit(1);
			         }
				      memset(data,0,SIZE);
				 }
				    fclose(fd);
				    printf("File Transfered\n");
				    send(sock_fd,done,sizeof(done),0);
				     
				 }else{
				    
				    send(sock_fd,ndone,sizeof(ndone),0);
				    printf("No such file\n");
				 }	
		     }else{
		     		 char data[SIZE] ={0};
				 fd = fopen(filepath,"rb");
				 				 
				 if(fd!=NULL){
				 stat(filepath, &st);
                            
			     	 long int y= st.st_size;
			     	 char stry[64]={0};
			     	 sprintf(stry, "%ld", y);
			     	 //printf("%s\n",stry);
				 send(sock_fd,done,sizeof(data),0);
				 send(sock_fd,stry,64,0);
				 
				 while(fread(data,1,SIZE,fd)>0){
				 if(send(sock_fd,data,sizeof(data),0)==-1){
				  	perror("Error in sending File");
				  	exit(1);
			         }
				      memset(data,0,SIZE);
				 }
				    fclose(fd);
				    printf("File Transfered\n");
				    send(sock_fd,done,sizeof(done),0);
				     
				 }else{
				    
				    send(sock_fd,ndone,sizeof(ndone),0);
				    printf("No such file\n");
				 }
		     }
		     
				     
		     }else{
		     	if(strcmp(input,"client stop")==0){
		     		printf("Connection closed\n");
		     		break;
		     	}
		     }
		   
		   }
		
		}
		
		close(sock_fd);
		//pthread_create(&tid,NULL,calc,(void *)&new_sock_fd);
		//pthread_join(tid,NULL);
}


int main(int argc,char *argv[]){
	int sock_fd;
	pthread_t tid_u;
	
	
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
	 
	 
	
	 struct some connect_info;
	 connect_info.info_sock_fd=sock_fd;
	 connect_info.info_add_port.sin_family = AF_INET;
	 connect_info.info_add_port.sin_addr.s_addr = INADDR_ANY;
	 connect_info.info_add_port.sin_port = htons(PORT);
	 connect_info.info_len = addrlen;
	 
	 
	 int num_connects;
	 printf("enter max connections:");
	 scanf("%d",&num_connects);
	 
	 pthread_create(&tid_u,NULL,create_user,NULL);
	 pthread_t conn_tid[num_connects];
	 for(int i=0;i<num_connects;i++){
	 	pthread_create(&conn_tid[i],NULL,connect_fun,(void *) &connect_info);
	 }
	 for(int i=0;i<num_connects;i++){
	 	pthread_join(conn_tid[i],NULL);
	 }
	 pthread_cancel(tid_u);
	 
	 close(sock_fd);
	 
 return 0;
}
