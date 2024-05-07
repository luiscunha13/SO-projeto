#ifndef SO_PROJETO_TASK_H
#define SO_PROJETO_TASK_H

#define MAX 300

typedef enum TaskType {
    EXECUTE,
    STATUS,
    DONE,
    CLOSE
} TaskType;

typedef enum TaskStatus{
    SCHEDULED,
    EXECUTING
} TaskStatus;

typedef enum TaskArg{
    ONE,
    PIPELINE
} TaskArg;


typedef struct Task{
    TaskType type;
    pid_t pid;
    char command[MAX];
    TaskArg arg;
    int exp_time;
    TaskStatus status;
    int id;

}Task;

typedef struct Task_List{
    Task* task;
    struct Task_List *next;
}Task_List;


void argsToList(char *command, char *list[]);

int commandsToList(char *command, char *list[]);

void addsched_task(Task_List** list, Task* task, char* sched_policy);

void add_Task_fcfs(Task_List** list, Task* task);

void add_Task_sjf(Task_List** list, Task* task);

void remove_head_Task(Task_List** list);

Task* get_task(Task_List* t);

Task_List* copyTaskList(Task_List* original);

#endif
