#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"


void parse_Task_Execute(Task t,pid_t pid, char *argv[]){
    t.type = EXECUTE;

    t.pid = pid;

    char *aux = NULL;
    strcpy(aux,argv[4]);
    if (aux[0] == '"' && aux[strlen(aux)-1] == '"') {
        aux[strlen(aux)-1] = '\0';
        aux++;
    }
    strcpy(t.command,aux);

    if(strcmp(argv[3],"-u")==0) t.arg = ONE;
    else if(strcmp(argv[3],"-p")==0) t.arg= MULTIPLE;

    t.exp_time = atoi(argv[2]);

    t.real_time = -1;

    t.status = WAITING;
}

void parse_Task_Status(Task t, pid_t pid){
    t.type = STATUS;
    t.pid = pid;
}

void argsToList(Task t, char *list[]){

    char *copy = strdup(t.command);
    char *token;
    int i=0;

    while ((token = strsep(&copy, " ")) != NULL && i < MAX) {
        list[i] = strdup(token);
        i++;
    }
    list[i]=NULL;
}


int execute_task_ONE(Task t, char *list_args[]){
    //agora temos de meter o dup2 dentro dos casos  if t.arg == 0 de modo a 
    // para ele depois meter o output direito?
    pid_t pid = fork();

    switch(pid){
        case -1:
            perror("Erro ao criar processo filho");
            return -1;
    }
    if(pid == -1){

    }
    else if (pid == 0) {
        char outfilename[40];
        char errorsfilename[40];
        sprintf(outfilename, "%d_output.txt", t.pid);
        sprintf(outfilename, "%d_errors.txt", t.pid);

        int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fdout == -1) {
            perror("Didn't open output file");
            return -1;
        }
        int fderr = open(errorsfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fderr == -1) {
            perror("Didn't open errors file");
            return -1;
        }

        dup2(fdout, 1);
        //dup2(fdout, 0);
        close(fdout);

        if (t.arg == 0) {
            execlp(t.name, t.name, NULL);
        }
    }

    
}

/*else{

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
 */


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

void add_Task(Task_List* list, Task task){ //mete a task à cabeça da lista
    Task_List new = malloc(sizeof (struct Task_List));

    new->task = task;
    new->next = (*list);
    (*list) = new;
}

