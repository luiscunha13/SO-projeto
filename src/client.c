#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include"task.h"

#define MAX 300

#define CLIENT_SERVER "client_server"

int main(int argc, char * argv[]){

    Task* t = malloc(sizeof(struct Task));
    pid_t pid = getpid();

    if(argc == 5 && strcmp(argv[1],"execute")==0){
        //parse_Task_Execute(t, pid,argv);
        t->type = EXECUTE;
        t->pid = pid;
        t->exp_time = atoi(argv[2]);
        t->real_time = -1;
        t->status = SCHEDULED;
        t->id = -1;

        char *aux = malloc(strlen(argv[4]) + 1);
        if(aux == NULL){
            perror("Erro ao alocar espaço para o parse");
            exit(EXIT_FAILURE);
        }
        strcpy(aux,argv[4]);
        if (aux[0] == '"' && aux[strlen(aux)-1] == '"') {
            aux[strlen(aux)-1] = '\0';
            aux++;
        }
        strcpy(t->command,aux);

        if(strcmp(argv[3],"-u")==0) t->arg = ONE;
        else if(strcmp(argv[3],"-p")==0) t->arg= PIPELINE;
        else{
            printf("CLIENT: invalid arguments\n");
            return -1;
        }

    }
    else if (argc == 2 && strcmp(argv[1],"status")==0){
        //parse_Task_Status(t, getpid());
        t->type = STATUS;
        t->pid = pid;
    }
    else{
        printf("CLIENT: invalid arguments\n");
        return -1;
    }


    //criar fifo server-cliente como pid
    char fifoc_name[30];
    sprintf(fifoc_name,"server_client_%d",pid);

    if (access(fifoc_name, F_OK) == -1){
        if(mkfifo(fifoc_name, 0666) == -1){
            perror("CLIENT: mkfifo server-client\n");
            return -1;
        }
    }

    // Aqui ele vai enviar a task para o servidor
    int cs_fifo = open(CLIENT_SERVER, O_WRONLY);
    if(cs_fifo == -1){
        perror("CLIENT: Dind't open client-server fifo to write\n");
        return -1;
    }

    write(cs_fifo,t,sizeof(struct Task));
    close(cs_fifo);


    int server_client = open(fifoc_name,O_RDONLY);
    if(server_client == -1){
        perror("CLIENT: Dind't open server-client fifo to read\n");
        return -1;
    }

    int id;

    read(server_client,&id,sizeof (int));

    if(id==-1){
        printf("Task status\n");
    }
    else{
        printf("Task id: %d\n",id);
    }




    close(server_client);

    unlink(fifoc_name);

    return 0;

}