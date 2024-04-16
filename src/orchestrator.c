#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"

#define MAX 300

#define WAITING 1
#define EXECUTING 2
#define FINISHED 0

#define CLIENT_SERVER "client_server"

int task_id=0;

int main(int argc, char *argv[]){
    //cria fifo cliente-servidor
    if(mkfifo(CLIENT_SERVER, 0666) == -1){
        perror("mkfifo client-server");
        return -1;
    }


    //abre o fifo client-server para escrita
    int client_server_write = open(CLIENT_SERVER, O_WRONLY);
    if (client_server_write == -1) {
        perror("client-server_write open");
        return -1;
    }

    Task_List *list = new_List();

    //abre o fifo client-server para leitura
    int client_server_read = open(CLIENT_SERVER, O_RDONLY);
    if (client_server_read == -1) {
        perror("client-server_read open");
        return -1;
    }

    Task t;

    while(read(client_server_read,&t, sizeof(struct Task))>0){
        if(t.type == EXECUTE){
            execute_task(t);
        }
        else if(t.type == STATUS){
            send_status();
        }
    }








}