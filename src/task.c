#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"task.h"


void parse_Task_Execute(Task t,pid_t pid, char *argv[]){
    t.type = EXECUTE;

    t.pid = pid;

    char *aux = NULL;
    strcpy(aux,argv[4]);
    if (aux[0] == '"' && aux[strlen(aux)-1] == '"') {
        aux[strlen(aux)-1] = '\0';
        aux++;
    }
    strcpy(t.command,aux);

    if(strcmp(argv[3],"-u")==0) t.arg = ONE;
    else if(strcmp(argv[3],"-p")==0) t.arg= PIPELINE;

    t.exp_time = atoi(argv[2]);

    t.real_time = -1;

    t.status = WAITING;

    t.id = -1;
}

void parse_Task_Status(Task t, pid_t pid){
    t.type = STATUS;
    t.pid = pid;
}

void argsToList(char *command, char *list[]){

    char *copy = strdup(command);
    char *token;
    int i=0;

    while ((token = strsep(&copy, " ")) != NULL && i < MAX) {
        list[i] = strdup(token);
        i++;
    }
    list[i]=NULL;
}

int commandsToList(Task t, char *list[]){

    char *copy = strdup(t.command);
    char *token;
    int i=0;

    while ((token = strsep(&copy, "|")) != NULL && i < MAX) {
        list[i] = strdup(token);
        i++;
    }
    list[i]=NULL;

    return i-1;
}


void send_status(int client_fifo, int status){
    size_t b_written = write(client_fifo, &status,sizeof(status));
    if(b_written == -1){
        perror("Erro ao escrever status no FIFO do cliente");
        exit(EXIT_FAILURE);
    }
}


void set_realtime(Task t, long time){
    t.real_time = time;
}

void set_id(Task t, int id){
    t.id = id;
}

void status_executing(Task t){
    t.status = EXECUTING;
}

void status_finished(Task t){
    t.status = FINISHED;
}

void add_Task(Task_List* list, Task task){ //mete a task à cabeça da lista
    Task_List new = malloc(sizeof (struct Task_List));

    new->task = task;
    new->next = (*list);
    (*list) = new;
}

Task get_task(Task_List t){
    return t->task;
}

