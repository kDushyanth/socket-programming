//gcc client.c -o client -lpthread
//good bye:- to terminate
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#define PORT 1234 
int terminate=0;

//thread to send messages
void *sender(void *sock_fd_v){
	int sock_fd = *((int *)sock_fd_v);
	char buffer_s[128] = {0}; // sending message will be stored in this char array
	while(1){
		
		memset(buffer_s ,0 , 128);
		
		scanf("%[^\n]%*c", buffer_s);
		send(sock_fd, buffer_s, 128, 0);
		if(terminate==0)printf("Sent: %s\n", buffer_s);
		if (strcmp(buffer_s, "good bye" ) == 0 || terminate){
		if(terminate==0)
		  {
		  	close(sock_fd);
      		  	terminate=1;
      		  	
      		  }
		
		  break;
	        }
	}
	
}
//thread to receive messages
void *receiver(void *sock_fd_v){
	int sock_fd = *((int *)sock_fd_v);
	char buffer_r[128] = {0}; // received message will be stored in this char array
	while(1){
		memset(buffer_r ,0 , 128);
		recv(sock_fd,(void *) buffer_r,128, 0);
	   	if(terminate==0)printf("received: %s\n", buffer_r);
	   	if (strcmp(buffer_r, "good bye") == 0 ||terminate){
	   	if(terminate==0)
		  {
		  	
		  	close(sock_fd);
      		  	terminate=1;
      		  	
      		  }
		
		  break;
	       } 
	}
	
}

int main(int argc, char const *argv[]) 
{ 
	int sock_fd; 
	struct sockaddr_in add_port; 
	char *init_msg = "Hi from client"; 
	int len = strlen(init_msg);
	
	char init_buffer_r[128] = {0}; // initial message from server will be stored in this char array
	
	//IPv4 socket that accepts TCP connection
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd==-1){
		perror("failed to create socket");
		exit(1);
	}
	
	add_port.sin_family = AF_INET; 
	add_port.sin_port = htons(PORT); 
	
	if(inet_pton(AF_INET, "127.0.0.1", &add_port.sin_addr)==-1) 
	{ 
		perror("Invalid address"); 
		exit(1); 
	} 
	
	//connecting to local host on port 1234
	if (connect(sock_fd, (struct sockaddr *)&add_port, sizeof(add_port)) < 0) 
	{ 
		perror("failed to connect"); 
		exit(1);
	} 
	//inital message
	send(sock_fd, init_msg, len, 0);
	recv(sock_fd,(void *)init_buffer_r,128, 0);
	printf("%s\n",init_buffer_r ); 
	
	//interaction
	pthread_t tid[2];
	
	pthread_create(&tid[0],NULL,sender,(void *)&sock_fd);
	pthread_create(&tid[1],NULL,receiver,(void *)&sock_fd);
	
	
	/*wait for the thread receiver to complete and after receiver completes cancel the sender that stays at scanf*/
	pthread_join(tid[1],NULL);
	pthread_cancel(tid[0]);
	printf("client terminated\n");
	return 0; 
} 

