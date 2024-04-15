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

    char comand[MAX];

    if (argc < 2){
        printf("não tem argumentos suficientes\n");
        //não sei se é para fazer mais cenas
        return 0;
    }

    char fifoc_name[30];
    sprintf(fifoc_name,SERVER_CLIENT"%d",getpid());

    if(mkfifo(fifoc_name, 0666) == -1){
        perror("mkfifo server-client");
        return -1;
    }

    Task t = new

    if(strcmp(argv[1],"execute") == 0){
        Task t = new_Task(getpid(),argv[4],atoi(argv[2]),argv[3],0);
    }
    else if(strcmp(argv[1],"status") == 0){
        //strcpy(comand,"status");
    }
    //else printf("comando inválido");

    int cs_fifo = open("client_server", O_WRONLY);
    if(cs_fifo<0) perror("erro abrir fifo client_server");

    write(cs_fifo,t,sizeof(struct Task));
    close(cs_fifo);

    size_t b_read;
    int server_client = open("server_client",O_RDONLY);
    while((b_read = read(server_client,comand,MAX))>0){
        write(...,comand,b_read); //perceber onde se deve escrever a resposta
    }
    close(server_client);

    return 0;

}