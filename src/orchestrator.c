#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 300

#define CLIENT_SERVER "client_server"
#define SERVER_CLIENT "server_client"

int main(int argc, char *argv[]){
    //cria fifo cliente-servidor
    if(mkfifo(CLIENT_SERVER, 0666) == -1){
        perror("mkfifo client-server");
        return -1;
    }
    //cria fifo servidor-cliente
    if(mkfifo(SERVER_CLIENT, 0666) == -1){
        perror("mkfifo server-client");
        return -1;
    }
    //abre o fifo client-server para leitura
    int client_server = open(CLIENT_SERVER, O_RDONLY);
    if (client_server == -1) {
        perror("client-server open");
        return -1;
    }

    //abre fifo server-client para escrita
    int server_client = open(SERVER_CLIENT, O_WRONLY);
    if (server_client == -1) {
        perror("server-client open");
        return -1;
    }

    pid_t pid;

    while(1){
        char *instruction = malloc(MAX);
        read(client_server,instruction, MAX);

        if(strncmp(instruction, "execute", 7) == 0){

        }
        else if(strncmp(instruction, "status", 6)==0){
            
        }

    }





}