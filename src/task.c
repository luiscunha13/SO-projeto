#include <fcntl.h>
#include<stdlib.h>
#include<string.h>
#include"task.h"

Task* new_Task(){
    Task* t = NULL;
    return t;
}

Task set_Task(pid_t pid, char* name, int time, char* type, int status){
    Task newtask;
    newtask.pid = pid;
    strcpy(newtask.name,name);
    if(strcmp(type,"-u")==0) newtask.type = 0;
    else if(strcmp(type,"-p")==0) newtask.type=1;
    newtask.exp_time = time;
    newtask.real_time = 0;
    newtask.status = status;

    return newtask;
}

void set_realtime(Task t, int time){
    t.real_time = time;
}

void update_status(Task t, int status){
    t.status = status;
}

Task parse_Task(char* instruction){
    char *copy = strdup(instruction);
    char *token;
    int i=0;
    int time;

    while ((token = strsep(&instruction, " ")) != NULL) {
        switch(i++){
            case 0:
                break;
            case 1:
                time = atoi(token);
                break;
            case 2:
                //ver depois se se tem de fazer aqui alguma coisa
                break;
            case 3:
        }
    }
}

Task_List* new_List(){
    Task_List* list = NULL;
    return list;
}

void add_Task(Task_List* list, Task task){
    Task_List* new = malloc(sizeof (struct Task_List));

    new->task = task;
    //acabar
    //perceber se se deve inserir no início ou no fim
}

