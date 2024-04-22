#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include"task.h"

#define MAX 300

#define SERVER_CLIENT "server_client_"

int main(int argc, char * argv[]){

    char command[MAX];
    Task t;


    if(argc == 5 && strcmp(argv[1],"execute")==0) parse_Task_Execute(t, getpid(),argv);
    else if (argc == 2 && strcmp(argv[1],"status")==0) parse_Task_Status(t, getpid());
    else{
        printf("argumentos inválidos");
        return -1;
    }


    //criar fifo server-cliente como pid
    char fifoc_name[30];
    sprintf(fifoc_name,SERVER_CLIENT"%d",getpid());

    if(mkfifo(fifoc_name, 0666) == -1){
        perror("mkfifo server-client");
        return -1;
    }

    // Aqui ele vai enviar a task para o servidor
    int cs_fifo = open("client_server", O_WRONLY);
    if(cs_fifo == -1){
        perror("Dind't open client-server fifo");
        return -1;
    }

    write(cs_fifo,&t,sizeof(struct Task));
    close(cs_fifo);

    // ---------------------------------------------------------------
    //até aqui deve estar tudo bem (só falta completar a cena do status)
    // ---------------------------------------------------------------

    //apos mandar, ele abre como modo de leitura para receber a resposta do sv
    size_t b_read;
    int server_client = open("server_client",O_RDONLY);
    if(server_client == -1){
        perror("Dind't open server-client fifo");
        return -1;
    }


    while((b_read = read(server_client,command,MAX))>0){
        write(1,command,b_read); //perceber onde se deve escrever a resposta, meti 1 para não dar erro
    }
    close(server_client);

    return 0;

}