#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include"task.h"

#define MAX 300

#define CLIENT_SERVER "client_server"
#define SERVER_CLIENT "server_client_"

int main(int argc, char * argv[]){

    Task t;


    if(argc == 5 && strcmp(argv[1],"execute")==0) parse_Task_Execute(t, getpid(),argv);
    else if (argc == 2 && strcmp(argv[1],"status")==0) parse_Task_Status(t, getpid());
    else{
        printf("CLIENT: argumentos inválidos");
        return -1;
    }


    //criar fifo server-cliente como pid
    char fifoc_name[30];
    sprintf(fifoc_name,SERVER_CLIENT"%d",getpid());

    if(mkfifo(fifoc_name, 0666) == -1){
        perror("CLIENT: mkfifo server-client");
        return -1;
    }

    // Aqui ele vai enviar a task para o servidor
    int cs_fifo = open(CLIENT_SERVER, O_WRONLY);
    if(cs_fifo == -1){
        perror("CLIENT: Dind't open client-server fifo to write");
        return -1;
    }

    write(cs_fifo,&t,sizeof(struct Task));
    close(cs_fifo);


    int server_client = open(fifoc_name,O_RDONLY);
    if(server_client == -1){
        perror("CLIENT: Dind't open server-client fifo to read");
        return -1;
    }

    int id;
    read(server_client,&id,sizeof (int));

    printf("Id da tarefa: %d\n",id);

    close(server_client);

    unlink(fifoc_name);

    return 0;

}