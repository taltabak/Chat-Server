
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//---------------------------Forward Declarations-------------------------//
// specifies a boolean variable.
typedef enum { FALSE = 0, TRUE } bool;

//---------------------------Funcnions-------------------------//
//The next function gets the fd of the welcome socket and the number of the max clients and implementing the chat_server.
void implement_chat(int welcome_sd ,int max_clients);
//the next function gets a port and initiate a connection through this port, it returns the number of the socket.
int init_connection(int port);

//--------------------------End Of Declarations-------------------------//


//------------------Main---------------------//

int main (int argc , char* argv[])
{
    //check validation of arguments:
    if(argc != 3)
    {
        printf("Usage: ./chatserver <port> <max_clients>\n");
        return 0;
    }
    for(int i = 1; i < 3 ; i++)
        if(atoi(argv[i]) <= 0)
        {
            printf("Usage: ./chatserver <port> <max_clients>\n");
            return 0;
        }
    //end validation.
    int port = atoi(argv[1]);
    int max_clients = atoi(argv[2]);
    int welcome_sd = init_connection(port);
    implement_chat(welcome_sd,max_clients);
    return 0;
}

int init_connection(int port)
{
    int welcome_sd;		// socket descriptor
    if((welcome_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
	    perror("socket");
	    exit(1);
    }
    struct sockaddr_in srv;	/* used by bind() */
     /* create the socket */
    srv.sin_family = AF_INET; /* use the Internet addr family */
    srv.sin_port = htons(port); /* bind socket ‘fd’ to the port */
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(welcome_sd, (struct sockaddr*) &srv, sizeof(srv)) < 0) 
    {
        perror("bind"); 
        exit(1);
    }
    if(listen(welcome_sd, 5) < 0) 
    {
	    perror("listen");
	    exit(1);
    }
    return welcome_sd;
}

void implement_chat(int welcome_sd ,int max_clients)
{
    int client_cnt = 0; //ensure the clients number will not be more than max.
    bool write_to_clients = FALSE;  //specifies wether to write to all clients
    fd_set client_fds , write_fds , read_fds; // client_fds = all fd's in chat, write = all fd's ready to write , read_fds = all fd's ready to read 
    char buffer[1025]; //used in read/write functions.
    //clear fd_sets  
    FD_ZERO(&client_fds);
    FD_ZERO(&write_fds);
    FD_SET(welcome_sd,&client_fds); //insert welcome socket to to client fd_set
    while(1)
    {
        read_fds = client_fds; //check for all read fds if there is anything to read.
        int rc = select(getdtablesize(),&read_fds,&write_fds,NULL,NULL); 
        if(rc < 0)  //case select failed.
        {
            perror("select");
	        exit(1);  
        }
        if(FD_ISSET(welcome_sd,&read_fds)) //case there is a new client 
        {
            if(client_cnt < max_clients) //check if the clients number is no more than max_clients
            {
                int newsd = accept(welcome_sd,NULL,NULL);   //accept new client.
                if (newsd >= 0) //case accept succeed
                {
                    FD_SET(newsd,&client_fds); //add new client fd to the fd_set
                    client_cnt++; //increament client counter.
                }
            }
        }
        if(write_to_clients) //case there is a message to broadcast all clients
        {
            for(int fd = welcome_sd+1 ; fd < getdtablesize() ; fd++) //the loop runs in the range of all client's fd's
                if(FD_ISSET(fd,&write_fds)) //checks wether the client's fd is in the write fd_set 
                {
                    printf("fd %d is ready to write\n", fd);
                    write(fd,buffer,1024); //write the message.
                }
            FD_ZERO(&write_fds); //empty write fd_set.
            write_to_clients = FALSE; //message wrriten to all clients.
        }
        for(int fd = welcome_sd+1 ; fd < getdtablesize() ; fd++)    //the loop runs in the range of all client's fd's
            if(FD_ISSET(fd,&read_fds)) //case there is a message to read from a client
            {
                printf("fd %d is ready to read\n", fd);
                memset(buffer,0,1025);  //clear buffer.
                rc = read(fd,buffer,1024);  //read the message.
                if(rc <= 0) //case return value from read is equal to zero or less than client closed or read failed
                {
                    close(fd); //close socket
                    FD_CLR(fd,&client_fds); //clear the client fd from the fd_set of the clients
                    client_cnt--; //decreament the client counter.
                }
                else    
                {
                write_fds = client_fds; //set all clients to write fd_set
                FD_CLR(welcome_sd,&write_fds);  //clear welcome socket from the write fd_set
                write_to_clients = TRUE;   
                }
            }
    }
}
