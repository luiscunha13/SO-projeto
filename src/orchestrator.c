#include<stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

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
    //abre o fifo para leitura
    int server_fifo_fd = open(CLIENT_SERVER, O_RDONLY);
    if (server_fifo_fd == -1) {
        perror("server_fifo open");
        return -1;
    }

    pid_t pid;

    while(1){
        char *instruction = malloc(MAX);
        if(server_fifo_fd)
    }





}