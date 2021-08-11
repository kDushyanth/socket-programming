//gcc client.c -o client -lpthread
//good bye:- to terminate
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include <sys/stat.h>

#define PORT 1234 
#define SIZE 1024

struct stat st;

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
	
	//authentication
	char username[128]={0};
	char password[128]={0};
	char msg[128]={0};
	int loggedIn=0;
	char curr_user[128]={0};
	
	printf("\n----------------authentication----------------------\n\n");
	while(loggedIn==0){
	
	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	printf("Enter password:");
	
	scanf("%[^\n]%*c",password);
	
	send(sock_fd,username,128,0);
	send(sock_fd,password,128,0);
	
	memset(msg,0,128);
	recv(sock_fd,(void *)msg,128,0);
	printf("Message:%s \n",msg);
	if(strcmp(msg,"Logged In")==0){
	printf("\n-------------------operations-----------------------\n\n");
	printf("modes: a-ascii and b-binary;send/recv\n");
	printf("input format:mode a send sample.txt\n");
	printf("input format:mode b recv sample.txt\n\n");
		strncpy(curr_user,username,128);
		loggedIn=1;break;
	}
	
	}
	printf("curr_user %s\n",curr_user);
	
	
	char input[64];char op[5];char fname[64];char *done="$D0Ne#";
	int mode=-1;//a-ascii & b-binary
	while(1){
	
	   memset(input,0,32);memset(op,0,5);memset(fname,0,28);
	   scanf("%[^\n]%*c",input);
	   strncpy(op,input+7,4);strncpy(fname,input+12,51);
	   
	   
	   if(strcmp(op,"stop")==0){
	        send(sock_fd,input,32,0);
	     	printf("Connection closed \n");
	     	break;
	   }
	   mode = input[5]-'a';
	   printf("mode-%d;input-%s;operation-%s;filename-%s\n",mode,input,op,fname);
	   if(mode!=0 && mode!=1){
	   printf("enter valid mode\n");
	   continue;
	   }
	  char filepath[100]="./";
	  strcat(filepath,curr_user);
	  strcat(filepath,"/");
	  strcat(filepath,fname);
	   if(strcmp(op,"send")==0){
	   if(mode==0){
	   	  char data[SIZE] ={0};
	         
                 FILE *fd = fopen(filepath,"r");
                 
                 if(fd!=NULL){
                            stat(filepath, &st);
                            
			     int y= st.st_size;
                            
                            printf("Sending... file of size:%dbytes\n",y);
                            int z=y/1024;
                            int counter=0;
		            send(sock_fd,input,64,0);
		            printf("0%%\n");
		            while(fread(data,1,SIZE,fd)>0){
		            counter++;
		            if(counter==z/2)printf("50%% \n");
		            if(counter==z/4)printf("25%% \n");
		            if(counter==3*z/4)printf("75%% \n");
		            if(counter==z)printf("100%% \n");
			     if(send(sock_fd,data,sizeof(data),0)==-1){
			  	perror("Error in sending File");
			  	exit(1);
			     }
			      memset(data,0,SIZE);
			    }
			    fclose(fd);
		    	    send(sock_fd,done,sizeof(done),0);
		     	    printf("File Transfered\n");
                 }else{
                    printf("enter valid file\n");
                 }
	   }else{
	   if(mode){
                 unsigned char data[SIZE] ={0};
	         
                 FILE *fd = fopen(filepath,"rb");
                 
                 if(fd!=NULL){
                            stat(filepath, &st);
                            
			     int y= st.st_size;
                            
                            printf("Sending... file of size:%dbytes\n",y);
                            int z=y/1024;
                            int counter=0;
		            send(sock_fd,input,64,0);
		            printf("0%%\n");
		            while(fread(data,1,SIZE,fd)>0){
		            counter++;
		            if(counter==z/2)printf("50%% \n");
		            if(counter==z/4)printf("25%% \n");
		            if(counter==3*z/4)printf("75%% \n");
		            if(counter==z)printf("100%% \n");
			     if(send(sock_fd,data,sizeof(data),0)==-1){
			  	perror("Error in sending File");
			  	exit(1);
			     }
			      memset(data,0,SIZE);
			    }
			    fclose(fd);
		    	    send(sock_fd,done,sizeof(done),0);
		     	    printf("File Transfered\n");
                 }else{
                    printf("enter valid file\n");
                 }
	   }
	   
	   }
	         	  
	   }else {
	   if(strcmp(op,"recv")==0){
	   if(mode==0){
	   	char buffer[SIZE]={0};int n;
	    	send(sock_fd,input,64,0);
	    	recv(sock_fd,buffer,SIZE,0);
	    	if(buffer[1]=='$' && buffer[2]=='D' && buffer[3]=='0' && buffer[4]=='N' && buffer[5]=='e' && buffer[6]=='#' && buffer[0]=='!'){
			      printf("Transfer Failed, No such file\n");
			      continue;
		}
	    	 char stry[64]={0};
	    	 recv(sock_fd,stry,64,0);
	    	 long int y;
	    	 sscanf(stry, "%ld", &y);
	    	 printf("Receiving... file of size:%ldbytes\n",y);
                long int z=y/1024;
                long int counter=0;
		 printf("0%%\n");
            	 FILE *fd = fopen(filepath, "w");
            	 while (1) {
		    	    	    n = recv(sock_fd, buffer, SIZE, 0);
				    counter++;
				    if(counter==z/2)printf("50%% \n");
				    if(counter==z/4)printf("25%% \n");
				    if(counter==3*z/4)printf("75%% \n");
				    if(counter==z)printf("100%% \n");
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
	   	unsigned char buffer[SIZE]={0};int n;
	    	send(sock_fd,input,64,0);
	    	recv(sock_fd,buffer,SIZE,0);
	    	if(buffer[1]=='$' && buffer[2]=='D' && buffer[3]=='0' && buffer[4]=='N' && buffer[5]=='e' && buffer[6]=='#' && buffer[0]=='!'){
			      printf("Transfer Failed, No such file\n");
			      continue;
		}
		char stry[64]={0};
	    	 recv(sock_fd,stry,64,0);
	    	 long int y;
	    	 sscanf(stry, "%ld", &y);
	    	 printf("Receiving... file of size:%ldbytes\n",y);
                long int z=y/1024;
                long int counter=0;
		 printf("0%%\n");
            	 FILE *fd = fopen(filepath, "wb");
            	 while (1) {
		    	    	    n = recv(sock_fd, buffer, SIZE, 0);
			    	    counter++;
				    if(counter==z/2)printf("50%% \n");
				    if(counter==z/4)printf("25%% \n");
				    if(counter==3*z/4)printf("75%% \n");
				    if(counter==z)printf("100%% \n");
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
	     
	       printf("Enter valid command\n");
	     
	   }
	   }
	
	}
	
	
	
	close(sock_fd);
	
	return 0; 
} 

