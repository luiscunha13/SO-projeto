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

void execute_task(Task t){
    pid_t pid = fork();

    if(pid == -1){
        perror("Erro ao criar processo filho");
        return -1; //nao sei muito bem esta parte dos returns
    }
    else if (pid == 0){
        char outfilename[40];
        sprintf(outfilename, "%d_output.txt", t.pid);

        int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if(fdout == -1){
            perror("Didn't open output file");
            return -1;
        }

        dup2(fdout, 1);
        dup2(fdout, 0);
        close(fdout);

        if(t.arg == 0){
            execlp(t.name, t.name, NULL);
        }
        else{
            //parte para executar o pipeline de programas
        }
    }
    else{
        t.pid = pid;
        set_Task_Execute(t, pid, t.name, t.exp_time, (t.arg == 0) ? "-u" : "-p", t.status); 
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)){
            
            set_realtime(t, WEXITSTATUS(status));
        }
        
        update_status(t, FINISHED);
    
    }
    
}


void send_status(){

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

