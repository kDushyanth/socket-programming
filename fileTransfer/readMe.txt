Directory structure: (initial)
	|
	|___fserver
	|
	|___client.c
	|
	|___server.c
	
Directory Structure (updated by process of program 'server.c' when user is created):
If 2 users- user123,user456 are created then directory structure is as follows:
	|
	|___fserver
	|      |______user123
	|      |
	|      |______user456
	|
	|___client.c
	|
	|___server.c
	|
	|___user123
	|
	|___user456

File transfer happens between './fserver/user123' and './user123' for user123 and similary
            	       between './fserver/user456' and './user456' for user456.

server side supported command:
create user:this command asks for username,password and creates new user. After this a new folder dedicated to new user will be created in fserver folder(server side) and a new user folder gets created (client side).

client side supported commands: (Client program initially asks for username and password that we created in above step to Log in)
1.client stop: terminates client and closes connection.

For example: If we login as user123 then
2.mode a send a.txt: sends the file 'a.txt' in client folder (./user123)to the server (i.e., ./fserver/user123 folder)
3.mode a recv a.txt: transfers the file 'a.txt' in server (./fserver/user123) to the client (./user123)
similarly,
mode b send a.txt
mode b recv a.txt

modes of transfer:
a:ascii mode
b:binary mode 

server.c, client.c programs can check if the file specified in send, recv commands exists.

Not only '.txt' files but also other types of files can be transferred between client and server.
