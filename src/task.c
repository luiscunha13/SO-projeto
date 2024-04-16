#include <fcntl.h>
#include<stdlib.h>
#include<string.h>
#include"task.h"


void set_Task_Execute(Task t,pid_t pid, char* name, int time, char* arg, int status){

    t.pid = pid;
    strcpy(t.name,name);
    if(strcmp(arg,"-u")==0) t.arg = 0;
    else if(strcmp(arg,"-p")==0) t.arg=1;
    t.exp_time = time;
    t.real_time = 0;
    t.status = status;
    t.type = EXECUTE;

}

void set_Task_Status(Task t){
    t.type = STATUS;
}

void set_realtime(Task t, int time){
    t.real_time = time;
}

void update_status(Task t, int status){
    t.status = status;
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

