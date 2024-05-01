#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"
#include<sys/wait.h>
#include<sys/time.h>


#define CLIENT_SERVER "client_server"

int task_id=1;
//para o -u
long execute_task_ONE(int id, char *command, struct timeval before, char *folder, char *outputs_file){

    char *list_args[MAX];
    argsToList(command,list_args);
    char outfilename[100];
    struct timeval after;
    pid_t pid = fork();

    int status;
    switch(pid){
        case -1:
            perror("Erro ao criar processo filho");
            return -1;
        case 0:
            sprintf(outfilename, "%s/%d_output.txt",folder, id);

            int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fdout == -1) {
                perror("EXECUTE_ONE: Didn't open output file");
                return -1;
            }

            dup2(fdout, 1);
            dup2(fdout,2);
            close(fdout);

            execvp(list_args[0], list_args);



            _exit(0);
        default :
            wait(&status);
            if(WIFEXITED(status))
                printf("Task %d finished \n", id);
            else
                printf("Task %d did not finish correctly", id);
            break;
    }

    gettimeofday(&after,NULL);

    long ms = (after.tv_sec - before.tv_sec) * 1000 + (after.tv_usec - before.tv_usec) / 1000;

    char lines[50];
    sprintf(lines,"%d %s %ld ms\n", id,command,ms);

    int fd = open(outputs_file,O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("EXECUTE_ONE: Didn't open outputs file");
        return -1;
    }

    ssize_t bytes_written = write(fd,&lines,strlen(lines));
    if (bytes_written == -1) {
        perror("EXECUTE_ONE: Error writing to outputs file");
        close(fd);
        return -1;
    }

    close(fd);

    return ms;
}

void exec_command(char* command){
    char *exec_args[MAX];
    argsToList(command, exec_args);
    execvp(exec_args[0], exec_args);
}
//para o -p
long execute_task_PIPELINE(int id, char *command, struct timeval before, char *folder, char *outputs_file){
    char *tasks[MAX];
    int num_tasks = commandsToList(command,tasks);

    char outfilename[100];
    struct timeval after;

    sprintf(outfilename, "%s/%d_output.txt",folder, id);

    int fdout = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fdout == -1) {
        perror("EXECUTE_PIPELINE: Didn't open output file");
        return -1;
    }

    int pipes[num_tasks][2];
    for(int i = 0; i< num_tasks; i++){
        if(i == 0){
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                    perror("EXECUTE_PIPELINE: Problem creating child process");
                    break;

                case 0 : 
                    close(pipes[i][0]);
                    dup2(pipes[i][1],1);

                    dup2(fdout,2);

                    exec_command(tasks[i]);
                    break;
                default:
                    close(pipes[i][1]);
                    break;
            }
        }
        else if(i == num_tasks-1){
            switch(fork()){
                case 0 :
                    dup2(pipes[i-1][0],0);
                    close(pipes[i-1][0]);

                    dup2(fdout,1);
                    dup2(fdout,2);
                    close(fdout);

                    exec_command(tasks[i]);
                    break;
                default :
                    close(pipes[i-1][0]);
                    close(fdout);
                    break;
            }
        }
        else{
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                    perror("EXECUTE_PIPELINE: Problem creating child process");
                    break;
                case 0 :
                    close(pipes[i][0]);
                    dup2(pipes[i-1][0], 0);
                    close(pipes[i-1][0]);

                    dup2(pipes[i][1],1);
                    close(pipes[i][1]);

                    dup2(fdout,2);

                    exec_command(tasks[i]);
                    break;
                default:
                    close(pipes[i-1][0]);
                    close(pipes[i][1]);
                    break;
            }
        }

    }
    


    int status;
    int aux=0;
    for(int i = 0; i<num_tasks;i++){
        wait(&status);
        if(WIFEXITED(status))
            continue;
        else{
            aux=1;
            printf("Task %d did not finish correctly", id);
            break;
        }
    }
    if(aux==0) printf("Task %d finished \n", id);

    gettimeofday(&after,NULL);

    long ms = (after.tv_sec - before.tv_sec) * 1000 + (after.tv_usec - before.tv_usec) / 1000;


    int fd = open(outputs_file,O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("EXECUTE_PIPELINE: Didn't open outputs file");
        return -1;
    }

    char lines[50];
    sprintf(lines,"%d %s %ld\n", id,command,ms);

    ssize_t bytes_written = write(fd,lines,strlen(lines));
    if (bytes_written == -1) {
        perror("EXECUTE_PIPELINE: Error writing to outputs file");
        close(fd);
        return -1;
    }

    close(fdout);
   

    return ms;
}

int status(Task_List* list_tasks, char *outputs_file, char *fifoc_name){

    int fdc = open(fifoc_name,O_WRONLY);
    if(fdc==-1){
        perror("SERVER: Dind't open server-client fifo to write\n");
        return -1;
    }

    Task_List* current = list_tasks;
    char line[MAX+50];
    sprintf(line,"Executing \n");
    write(fdc,&line,strlen(line));

    if(current == NULL){
        sprintf(line,"\nScheduled\n\n");
        write(fdc,&line,strlen(line));
    }
    else{
        while(current != NULL){
            Task* t = get_task(current);
            if(t->status == EXECUTING){
                sprintf(line,"%d %s\n", t->id,t->command);
                write(fdc,&line,strlen(line));
            }
            current = current->next;
        }
        sprintf(line,"\n");
        write(fdc,&line,strlen(line));

        current=list_tasks;
        while(current != NULL){
            Task* t = get_task(current);
            if(t->status == SCHEDULED){
                sprintf(line,"%d %s\n", t->id,t->command);
                write(fdc,&line,strlen(line));
            }
            current = current->next;
        }
        sprintf(line,"\n");
        write(fdc,&line,strlen(line));
    }

    sprintf(line,"Completed\n");
    write(fdc,&line,strlen(line));
    //para as que já acabaram tem de se ler o ficheiro dos outputs e copiar

    int fd = open(outputs_file,O_RDONLY);
    if (fd == -1) {
        perror("STATUS: Didn't open outputs file");
        return -1;
    }

    ssize_t bytes_read;

    while ((bytes_read = read(fd, line, MAX+50)) > 0) {
        line[bytes_read] = '\0'; // Ensure buffer is null-terminated
        write(fdc,&line,strlen(line));

    }

    if (bytes_read < 0) {
        perror("STATUS: Error reading from outputs file");
        return 1;
    }

    close(fd);
    close(fdc);

    return 0;
}

int main(int argc, char *argv[]){ //output_folder parallel_tasks sched_policy

    if(argc != 4 || (strcmp(argv[3],"fcfs")!=0 && strcmp(argv[3],"sjf")!=0)){
        printf("SERVER: Invalid shedule policy\n");
        return -1;
    }

    //cria fifo cliente-servidor
    if (access(CLIENT_SERVER, F_OK) == -1){
        if(mkfifo(CLIENT_SERVER, 0666) == -1){
            perror("SERVER: mkfifo client-server\n");
            return -1;
        }
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


    Task* t = malloc(sizeof(struct Task));
    Task_List* list_tasks = NULL;
    struct timeval before;

    

    while(read(client_server_read,t, sizeof(struct Task))>0){
        if(t->type == EXECUTE){
            //int numtasks = 0;
            //int maxtasks = atoi(argv[2]);
             // aqui é onde ele vai estar sempre a correr
            gettimeofday(&before,NULL);
            t->id=task_id;
            task_id++;
            char aux[20];
            sprintf(aux, "Task id: %d\n", t->id);
            t->status=EXECUTING;
            addsched_task(&list_tasks, t, argv[3]);

            //Envio do id da tarefa para o cliente
            char fifoc_name[30];
            sprintf(fifoc_name,"server_client_%d",t->pid);


            int fdc = open(fifoc_name,O_WRONLY);
            if(fdc==-1){
                perror("SERVER: Dind't open server-client fifo to write\n");
                return -1;
            }
            write(fdc,&aux,strlen(aux));
            close(fdc);
            
            //while(1){
               // if(numtasks<maxtasks){
                    //provavelmente fazer aqui um ciclo para executar as tarefas da list_tasks
                    //numtasks++;
                    if(t->arg==ONE){ // -u
                        long time = execute_task_ONE(t->id, t->command, before, argv[1],outputsfile);
                        if(time != -1){
                            t->real_time= time;
                            t->status = FINISHED;
                           // numtasks--;
                        }
                    }
                    else if(t->arg==PIPELINE){ // -p
                        long time = execute_task_PIPELINE(t->id,t->command,before, argv[1],outputsfile);
                        if(time != -1){
                            t->real_time= time;
                            t->status = FINISHED;
                           // numtasks--;
                        }
                    }
                    

                    remove_head_Task(&list_tasks);
                //}
                //else{
                //    wait(NULL);
                //}
            

            //}
        }
        else if(t->type == STATUS){

            char fifoc_name[30];
            sprintf(fifoc_name,"server_client_%d",t->pid);

            status(list_tasks,outputsfile, fifoc_name);

            printf("Status finished");

        }
    }

    free(t);
    close(client_server_read);
    unlink(CLIENT_SERVER);


    return 0;


}