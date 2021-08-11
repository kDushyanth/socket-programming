//gcc server.c -o server -lpthread
//good bye:- to terminate
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#define BACKLOG 3
#define PORT 1234

int terminate = 0;
//thread to send messages
void *sender(void *new_sock_fd_v){
	int new_sock_fd = *((int *)new_sock_fd_v);
	char buffer_s[128] = {0}; // sending message will be stored in this char array
	while(1){
		
		memset(buffer_s ,0 , 128);
		scanf("%[^\n]%*c", buffer_s);
		send(new_sock_fd,buffer_s, 128, 0);
		if(terminate==0)printf("Sent: %s\n", buffer_s);
		if (strcmp(buffer_s, "good bye") == 0 ||terminate){
		  if(terminate==0)
		  {
		  	close(new_sock_fd);
      		  	terminate=1;
      		  }
		  
		  break;
	        } 
	}
	
}
//thread to receive messages
void *receiver(void *new_sock_fd_v){
	int new_sock_fd = *((int *)new_sock_fd_v);
	char buffer_r[128] = {0}; // received message will be stored in this char array
	while(1){
		memset(buffer_r ,0 , 128);
		recv(new_sock_fd,(void *) buffer_r,128, 0);
		if(terminate==0)printf("received: %s\n", buffer_r);
		if (strcmp(buffer_r, "good bye") == 0|| terminate){
		  if(terminate==0)
		  {
		  	
		  	close(new_sock_fd);
      		  	terminate=1;
      		  }
		 
		  break;
	        } 
	}
	
}
int main(int argc,char *argv[]){
	int sock_fd;
	char *init_msg = "Hi from server"; 
	int len = strlen(init_msg);
	
	
	char init_buffer_r[128] = {0}; // initial message from client will be stored in this char array
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
	 //accept connection 
	 int new_sock_fd = accept(sock_fd,(struct sockaddr *)&add_port,(socklen_t *) &addrlen);
	 if(new_sock_fd==-1){
	 	perror("failed to accept request");
	 	exit(1);
	 }
	 	//initial message
	 	recv(new_sock_fd,(void *) init_buffer_r,128, 0);
	 	send(new_sock_fd, init_msg, len, 0);
		printf("%s\n",init_buffer_r );
		
		//interaction
	pthread_t tid[2];
	
	pthread_create(&tid[0],NULL,sender,(void *)&new_sock_fd);
	pthread_create(&tid[1],NULL,receiver,(void *)&new_sock_fd);
	
	
	
	/*wait for the thread receiver to complete and after receiver completes cancel the sender that stays at scanf*/
	pthread_join(tid[1],NULL);
	pthread_cancel(tid[0]);
	printf("server terminated\n");
	close(sock_fd);
 return 0;
}
