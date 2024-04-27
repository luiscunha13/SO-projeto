#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"
#include<sys/wait.h>
#include<sys/time.h>

#define MAX 300

#define CLIENT_SERVER "client_server"
#define SERVER_CLIENT "server_client_"

int task_id=1;
//para o -u
int execute_task_ONE(Task t, char *list_args[], struct timeval before, char *folder, char *outputs_file){
    char outfilename[100];
    char errorsfilename[100];
    struct timeval after;
    pid_t pid = fork();

    int status;
    switch(pid){
        case -1:
            perror("Erro ao criar processo filho");
            return -1;
        case 0:
            sprintf(outfilename, "%s/%d_output.txt",folder, t.id);
            sprintf(errorsfilename, "%s/%d_errors.txt",folder, t.id);

            int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fdout == -1) {
                perror("EXECUTE_ONE: Didn't open output file");
                return -1;
            }
            int fderr = open(errorsfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fderr == -1) {
                perror("EXECUTE_ONE: Didn't open errors file");
                return -1;
            }

            int fd = open(outputs_file,O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                perror("EXECUTE_ONE: Didn't open outputs file");
                return -1;
            }

            dup2(fdout, 1);
            dup2(fderr,2);
            close(fdout);
            close(fderr);

            execvp(list_args[0], list_args);
            gettimeofday(&after,NULL);

            close(fdout);

            long ms = (after.tv_sec - before.tv_sec) * 1000 + (after.tv_usec - before.tv_usec) / 1000;

            t.real_time=ms;
            t.status=FINISHED;


            char lines[50];
            sprintf(lines,"Task %d \n Time spent: %ld ms\n", t.id,t.real_time);

            write(fd,lines,strlen(lines));
            /*
            ssize_t bytes_written =
            if (bytes_written == -1) {
                perror("EXECUTE_ONE: Error writing to outputs file");
                close(fd);
                close(fderr);
                return -1;
            }*/

            close(fderr);
            _exit(0);
        default :
            wait(&status);
            if(WIFEXITED(status))
                printf("Task %d finished \n", t.id);
            else
                printf("Task %d did not finish correctly", t.id);
            break;
    }

    return 0;
}

int exec_command(char* command){

    char *exec_args[MAX];

    argsToList(command,exec_args);

    int exec_ret=0;

    exec_ret=execvp(exec_args[0],exec_args);

    return exec_ret;
}

int execute_task_PIPELINE(Task t, char *tasks[], int num_tasks, struct timeval before, char *folder, char *outputs_file){
    char outfilename[100];
    char errorsfilename[100];
    struct timeval after;

    sprintf(outfilename, "%s/%d_output.txt",folder, t.id);
    sprintf(errorsfilename, "%s/%d_errors.txt",folder, t.id);

    int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fdout == -1) {
        perror("EXECUTE_PIPELINE: Didn't open output file");
        return -1;
    }

    int fderr = open(errorsfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fderr == -1) {
        perror("EXECUTE_PIPELINE: Didn't open errors file");
        return -1;
    }

    int pipes[num_tasks -1][2];
    for(int i = 0; i< num_tasks; i++){
        if(i == 0){
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                    perror("EXECUTE_PIPELINE: Erro ao criar processo filho");
                    break;

                case 0 : 
                    close(pipes[i][0]);
                    dup2(pipes[i][1],1);

                    dup2(fderr,2);
                    close(fderr);

                    exec_command(tasks[i]);
                default:
                    close(pipes[i][1]);
                    close(fderr);
            }
        }
        else{
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                    perror("EXECUTE_PIPELINE: Erro ao criar processo filho");
                    break;
                case 0 :
                    close(pipes[i][0]);
                    dup2(pipes[i-1][0], 0);
                    close(pipes[i-1][0]);

                    dup2(pipes[i][1],1);
                    close(pipes[i][1]);

                    dup2(fderr,2);
                    close(fderr);

                    exec_command(tasks[i]);
                default:
                    close(pipes[i-1][0]);
                    close(pipes[i][1]);
                    close(fderr);
            }
        }
        if(i == num_tasks-1){
            switch(fork()){
                case 0 :
                    dup2(pipes[i-1][0],0);
                    close(pipes[i-1][0]);

                    dup2(fdout,1);
                    close(fdout);

                    dup2(fderr,2);
                    close(fderr);

                    exec_command(tasks[i]);
                default :
                    close(pipes[i-1][0]);
                    close(fdout);
                    close(fderr);
            }
        }
    }
    for(int i = 0; i<num_tasks;i++){
        wait(NULL);
    }

    gettimeofday(&after,NULL);

    close(fdout);

    long ms = (after.tv_sec - before.tv_sec) * 1000 + (after.tv_usec - before.tv_usec) / 1000;

    t.real_time=ms;
    t.status=FINISHED;

    int fd = open(outputs_file,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("EXECUTE_PIPELINE: Didn't open outputs file");
        return -1;
    }

    char lines[50];
    sprintf(lines,"Task %d \n Time spent: %ld ms\n", t.id,t.real_time);

    ssize_t bytes_written = write(fd,lines,strlen(lines));
    if (bytes_written == -1) {
        perror("EXECUTE_PIPELINE: Error writing to outputs file");
        close(fd);
        close(fderr);
        return -1;
    }

    close(fderr);

    return 0;
}

int status(Task_List* list_tasks, Task_List* finished_tasks, char *outputs_file){
    int fd = open(outputs_file,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("STATUS: Didn't open outputs file");
        return -1;
    }

    Task_List* current = list_tasks;
    char line[MAX+100];
    sprintf(line,"WAITING/EXECUTING TASKS\n");
    write(fd,line,sizeof(line));

    while(current != NULL){
        Task t = get_task(current);
        sprintf(line,"Task %d\nCommand: %s\n", t.id,t.command);
        current = current->next;
    }

    current = finished_tasks;
    sprintf(line,"FINISHED TASKS\n");
    write(fd,line,sizeof(line));

    while(current != NULL){
        Task t = get_task(current);
        sprintf(line,"Task %d\nCommand: %s\nTime used: %ld ms\n", t.id,t.command,t.real_time);
        current = current->next;
    }
    close(fd);

    return 0;
}

int main(int argc, char *argv[]){ //output_folder parallel_tasks sched_policy

    if(argc != 4 || (strcmp(argv[3],"fcfs")!=0 && strcmp(argv[3],"sjf")!=0)){
        printf("SERVER: Invalid arguments\n");
        return -1;
    }

    //cria fifo cliente-servidor
    if(mkfifo(CLIENT_SERVER, 0666) == -1){
        perror("SERVER: mkfifo client-server\n");
        return -1;
    }


    //abre o fifo client-server para escrita
    int client_server_read = open(CLIENT_SERVER, O_RDONLY);
    if (client_server_read == -1) {
        perror("SERVER: Dind't open client-server fifo to read\n");
        return -1;
    }


    //abre o fifo client-server para leitura
    int client_server_write = open(CLIENT_SERVER, O_WRONLY);
    if (client_server_write == -1) {
        perror("SERVER: Dind't open client-server fifo to write\n");
        return -1;
    }

    char outputsfile[100];
    sprintf(outputsfile, "%s/outputs_file.txt",argv[1]);


    Task t;
    Task_List* list_tasks = NULL;
    Task_List* finished_tasks = NULL;
    char *list[MAX];
    //char *copy = strdup(t.command);
    struct timeval before;

    while(read(client_server_read,&t, sizeof(struct Task))>0){
        if(t.type == EXECUTE){
            gettimeofday(&before,NULL);
            int aux = task_id;
            t.id=task_id;
            task_id++;
            t.status=EXECUTING;
            add_Task_fcfs(&list_tasks,t);

            //Envio do id da tarefa para o cliente
            char fifoc_name[30];
            sprintf(fifoc_name,"server_client_%d",t.pid);


            int fdc = open(fifoc_name,O_WRONLY);
            if(fdc==-1){
                perror("SERVER: Dind't open server-client fifo to write\n");
                return -1;
            }
            write(fdc,&aux,sizeof (int));
            close(fdc);

            //provavelmente fazer aqui um ciclo para executar as tarefas da list_tasks

            if(t.arg==ONE){ // -u
                argsToList(t.command,list);
                execute_task_ONE(t, list, before, argv[1],outputsfile);
            }
            else if(t.arg==PIPELINE){ // -p
                int n = commandsToList(t,list);
                execute_task_PIPELINE(t,list,n,before, argv[1],outputsfile);
            }

            remove_head_Task(&list_tasks);
            add_task_head(&finished_tasks,t);

        }
        else if(t.type == STATUS){
            status(list_tasks,finished_tasks,outputsfile);
        }
    }

    close(client_server_read);
    unlink(CLIENT_SERVER);


    return 0;


}