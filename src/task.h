#ifndef SO_PROJETO_TASK_H
#define SO_PROJETO_TASK_H

#define MAX 300

typedef enum TaskType {
    EXECUTE,
    STATUS
} TaskType;

typedef enum TaskStatus{
    WAITING,
    EXECUTING,
    FINISHED
} TaskStatus;

typedef enum TaskArg{
    ONE,
    MULTIPLE
} TaskArg;


typedef struct Task{
    TaskType type; //tipo de task: Execute ou Status
    pid_t pid; // Pid do fifo do cliente que manda o pedido
    char command[MAX]; // comando a ser executado
    int arg; // argumento: -u ou -p
    int exp_time; //tempo esperado em ms (dado pelo utilizador)
    int real_time; //tempo que realmente demorou a ser executado
    TaskStatus status; //estado da task: Waiting, Executing, Finished

}Task;

typedef struct Task_List{
    Task task;
    struct Task_List *next;
}*Task_List;

void parse_Task_Execute(Task t,pid_t pid, char *argv[]);

void parse_Task_Status(Task t, pid_t pid);

void argsToList(Task t, char *list[]);

void set_realtime(Task t, int time);

void update_status(Task t, int status);

Task_List* new_List();

void add_Task(Task_List* list, Task task);

#endif
