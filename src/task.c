#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
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

void addsched_task(Task_List** list, Task* task, char* sched_policy){
    if(strcmp(sched_policy, "fcfs") == 0){
        add_Task_fcfs(list, task);
    } else if (strcmp(sched_policy, "sjf") == 0){
        add_Task_sjf(list, task);
    }
}


void add_Task_fcfs(Task_List** list, Task* task){ //mete a task no fim da list - first come, first served

    Task_List* new = malloc(sizeof (struct Task_List));
    if(new == NULL){
        perror("Erro ao alocar memória para a lista fcfs");
        exit(EXIT_FAILURE);
    }
    new->task = task;
    new->next = NULL;

    if(*list == NULL){
        *list = new;

    }
    else{
        Task_List* current = *list;
        while(current->next != NULL)
            current = current->next;

        current->next = new;
    }

}


void add_Task_sjf(Task_List** list, Task* task){ //mete a task por ordem crescente de tempo - shortest job first

    Task_List* new = malloc(sizeof (struct Task_List));
    if(new == NULL){
        perror("Erro ao alocar memória para a lista sjf");
        exit(EXIT_FAILURE);
    }
    new->task = task;
    new->next=NULL;

    if(*list == NULL || (*list)->task->exp_time >= task->exp_time){
        new->next = *list;
        *list = new;
    }
    else{
        Task_List *current = *list;
        while(current->next != NULL && current->next->task->exp_time < task->exp_time){
            current = current->next;
        }
        new->next = current->next;
        current->next = new;
    }

}


void remove_head_Task(Task_List** list){ // quando a task é feita é removida da lista

    if(list!=NULL){
        Task_List* aux = *list;
        *list = (*list)->next;
        free(aux);
    }

}

Task* get_task(Task_List* t){
    return t->task;
}

