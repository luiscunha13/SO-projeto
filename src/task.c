#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"task.h"


void argsToList(char *command, char *list[]){
    char *copy = strdup(command);
    char *token;
    int i=0;

    while ((token = strsep(&copy, " ")) != NULL && i < MAX) {
        if(strlen(token) > 0){
        list[i] = strdup(token);
        if (list[i] == NULL) {
            return;
        }
        i++;
        }
    }
    list[i]=NULL;

    free(copy);
}

int commandsToList(char *command, char *list[]){

    char *copy = command;
    char *token;
    int i=0;

    while ((token = strsep(&copy, "|")) != NULL && i < MAX) {
        list[i] = strdup(token);
        i++;
    }
    list[i]=NULL;

    free(copy);

    return i;
    

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

void add_Task_fcfs(Task_List** list, Task task){ //mete a task no fim da list - first come, first served
    Task_List* new = malloc(sizeof (struct Task_List));
    new->task = task;
    new->next=NULL;

    if(*list == NULL){
        *list = new;

    }

    Task_List* current = *list;
    while(current->next != NULL)
        current = current->next;

    current->next = new;
}

void add_task_sjf(Task_List** list, Task task){ //mete a task por ordem crescente de tempo - shortest job first
    Task_List* new = malloc(sizeof (struct Task_List));
    new->task = task;
    new->next=NULL;

    if(*list == NULL || (*list)->task.exp_time >= task.exp_time){
        new->next = *list;
        *list = new;
    }
    else{
        Task_List *current = *list;
        while(current->next != NULL && current->next->task.exp_time < task.exp_time){
            current = current->next;
        }
        new->next = current->next;
        current->next = new;
    }
}

void add_task_head(Task_List** list, Task task){
    Task_List* new = malloc(sizeof (struct Task_List));
    new->task = task;
    new->next=*list;
    *list=new;
}

void remove_head_Task(Task_List** list){ // quando a task é feita é removida da lista
    if(list!=NULL){
        Task_List* aux = *list;
        *list = (*list)->next;
        free(aux);
    }
}

Task get_task(Task_List* t){
    return t->task;
}

