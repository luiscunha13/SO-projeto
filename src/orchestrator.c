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
    //cria fifo servidor-cliente
    /*
    if(mkfifo(SERVER_CLIENT, 0666) == -1){
        perror("mkfifo server-client");
        return -1;
    }*/
    //abre o fifo client-server para leitura
    int client_server_read = open(CLIENT_SERVER, O_RDONLY);
    if (client_server_read == -1) {
        perror("client-server_read open");
        return -1;
    }
    //abre o fifo client-server para escrita
    int client_server_write = open(CLIENT_SERVER, O_WRONLY);
    if (client_server_write == -1) {
        perror("client-server_write open");
        return -1;
    }
    //abre fifo server-client para escrita
    /*
    int server_client = open(SERVER_CLIENT, O_WRONLY);
    if (server_client == -1) {
        perror("server-client open");
        return -1;
    }*/

    Task_List *list = new_List();
    pid_t pid;

    while(1){
        char *instruction = malloc(MAX);
        read(client_server,instruction, MAX);

        if(strncmp(instruction, "execute", 7) == 0){
            char aux[MAX];


            //função que transforme a instruction em uma string para por na lista tasks
            //função que saque o tempo previsto da instruction para a expected_time


            char notification[30];
            sprintf(notification,"task id - %d\n ",task_id+1);
            //envia o id da tarefa para o cliente
            write(server_client,notification, strlen(notification));



        }
        else if(strncmp(instruction, "status", 6)==0){

        }

    }





}