#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
const int SOCKET_COUNT=512;
int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    char *message, readData[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , SOCKET_COUNT);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    int count = 0;
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted: %d\n", count++);
        read(socket_desc, readData , 2000);
        //Reply to the client
        message = "Hello Client , I have received your connection. bye\n";
        write(new_socket , message , strlen(message));
        usleep(200);
    }


    return 0;
}
