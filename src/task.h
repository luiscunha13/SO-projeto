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
    PIPELINE
} TaskArg;


typedef struct Task{
    TaskType type; //tipo de task: Execute ou Status
    pid_t pid; // Pid do fifo do cliente que manda o pedido
    char command[MAX]; // comando a ser executado
    int arg; // argumento: -u ou -p
    int exp_time; //tempo esperado em ms (dado pelo utilizador)
    long real_time; //tempo que realmente demorou a ser executado
    TaskStatus status; //estado da task: Waiting, Executing, Finished
    int id;

}Task;

typedef struct Task_List{
    Task task;
    struct Task_List *next;
}Task_List;

void parse_Task_Execute(Task t,pid_t pid, char *argv[]);

void parse_Task_Status(Task t, pid_t pid);

void argsToList(char *command, char *list[]);

int commandsToList(char *command, char *list[]);

void set_realtime(Task t, long time);

void set_id(Task t, int id);

void status_executing(Task t);

void status_finished(Task t);

void add_Task_fcfs(Task_List** list, Task task);

void add_task_sjf(Task_List** list, Task task);

void add_task_head(Task_List** list, Task task);

void remove_head_Task(Task_List** list);

Task get_task(Task_List* t);

#endif
