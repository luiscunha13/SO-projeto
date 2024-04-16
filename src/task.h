#ifndef SO_PROJETO_TASK_H
#define SO_PROJETO_TASK_H

    #define MAX 300
    #define WAITING 0
    #define EXECUTING 1
    #define FINISHED 2

    #define EXECUTE 1
    #define STATUS 2

    typedef struct Task{
        pid_t pid;
        char name[MAX];
        int arg;
        int exp_time;
        int real_time;
        int status;
        int type;

    }Task;

    typedef struct Task_List{
        Task task;
        struct Task_List *next;
    }Task_List;


    void set_Task_Execute(Task t,pid_t pid, char* name, int time, char* type, int status);

    void set_Task_Status(Task t);

    void set_realtime(Task t, int time);

    void update_status(Task t, int status);

    Task_List* new_List();

    void add_Task(Task_List* list, Task task);

#endif
