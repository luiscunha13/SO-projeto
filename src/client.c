#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 300

int main(int argc, char * argv[]){

    char comand[MAX];

    if (argc < 2){
        printf("não tem argumentos suficientes\n");
        //não sei se é para fazer mais cenas
        return 0;
    }

    if(strcmp(argv[1],"execute") == 0){
        sprintf(comand,"%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
    }
    else if(strcmp(argv[1],"status") == 0){
        strcpy(comand,"status");
    }
    //else printf("comando inválido");

    int cs_fifo = open("client_server", O_WRONLY);
    if(cs_fifo<0) perror("erro abrir fifo client_server");

    write(cs_fifo,comand,strlen(comand));
    close(cs_fifo);

    size_t b_read;
    int server_client = open("server_client",O_RDONLY);
    while((b_read = read(server_client,comand,MAX))>0){
        write(...,comand,b_read); //perceber onde se deve escrever a resposta
    }
    close(server_client);

    return 0;

}