#include <fcntl.h>
#include<stdlib.h>
#include<string.h>

typedef struct Task{
    pid_t pid;
    char name[50];
    int exp_time;
    int real_time;
    int status;
}Task;

typedef struct Task_List{
    Task task;
    struct Task_List *next;
}Task_List;

Task new_Task(pid_t pid, char* name, int time, int status){
    Task newtask;
    newtask.pid = pid;
    strcpy(newtask.name,name);
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

