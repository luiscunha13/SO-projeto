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
    //agora temos de meter o dup2 dentro dos casos  if t.arg == 0 de modo a 
    // para ele depois meter o output direito?
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
            
            char* token;
            char* args[30];
            int i = 0;
            char *command, *tofree;
            int status, ret;

            command = tofree = strdup(t.name);

            
            while ((token   = strsep(&command, "|"))!= NULL && i < 30){
                args[i++] = token;
            }
            args[i] = NULL;

            pid_t pipe_pid = fork();
            switch(pipe_pid){
                case -1:
                    perror("Erro ao criar o processo pid para o pipeline");
                    ret = -1;
                    break;
                
                case 0: 
                    execvp(args[0],args);
                    perror("Erro ao executar o comando no pipeline");
                    _exit(EXIT_FAILURE);
                    break;

                default : 
                wait(&status);
                if(WIFEXITED(status)){
                    ret = WEXITSTATUS(status);
                }
                else{
                    perror("filho não terminou\n");
                    ret = -1;
                }
                free(tofree);

            }
            _exit(ret);
    
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


void send_status(int client_fifo, int status){
    size_t b_written = write(client_fifo, &status,sizeof(status));
    if(b_written == -1){
        perror("Erro ao escrever status no FIFO do cliente");
        exit(EXIT_FAILURE);
    }
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

