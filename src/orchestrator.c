#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"
#include<sys/wait.h>

#define MAX 300

#define CLIENT_SERVER "client_server"
#define SERVER_CLIENT "server_client_"

int task_id=1;
//para o -u
int execute_task_ONE(Task t, char *list_args[]){ //fica a faltar a parte de meter o tempo a contar
    char outfilename[40];
    char errorsfilename[40];

    pid_t pid = fork();

    int status;
    switch(pid){
        case -1:
            perror("Erro ao criar processo filho");
            return -1;
            break;
        case 0:
            sprintf(outfilename, "%d_output.txt", t.pid);
            sprintf(errorsfilename, "%d_errors.txt", t.pid);

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
            dup2(fderr,2);
            close(fdout);
            close(fderr);

            execvp(list_args[0], list_args);

            _exit(0);
        default :
            wait(&status);
            if(WIFEXITED(status))
                printf("tarefa %d terminou \n", t.id);
            else
                printf("tarefa %d não terminou normalmente", t.id);
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

int execute_task_PIPELINE(char *tasks[], int num_tasks){

    int pipes[num_tasks -1][2];
    for(int i = 0; i< num_tasks; i++){
        if(i == 0){
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                        perror("Erro ao criar pid filho");
                        break;

                case 0 : 
                        close(pipes[i][0]);
                        dup2(pipes[i][1],1);
                        exec_command(tasks[i]);
                default:
                        close(pipes[i][1]);
            }
        }
        else{
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                        perror("Erro ao criar pid filho");
                        break;
                case 0 :
                        close(pipes[i][0]);
                        dup2(pipes[i-1][0], 0);
                        close(pipes[i-1][0]);

                        dup2(pipes[i][1],1);
                        close(pipes[i][1]);
                        exec_command(tasks[i]);
                default:
                        close(pipes[i-1][0]);
                        close(pipes[i][1]);                        
            }
        }
        if(i == num_tasks-1){
            switch(fork()){
                case 0 :
                        dup2(pipes[i-1][0],0);
                        close(pipes[i-1][0]);
                        exec_command(tasks[i]);
                default :
                close(pipes[i-1][0]);
            }
        }
    }
    for(int i = 0; i<num_tasks;i++){
			wait(NULL);
		}
    return 0;
}

int status(Task_List list){

}

int main(int argc, char *argv[]){

    //cria fifo cliente-servidor
    if(mkfifo(CLIENT_SERVER, 0666) == -1){
        perror("SERVER: mkfifo client-server");
        return -1;
    }


    //abre o fifo client-server para escrita
    int client_server_write = open(CLIENT_SERVER, O_WRONLY);
    if (client_server_write == -1) {
        perror("SERVER: Dind't open client-server fifo to write");
        return -1;
    }


    //abre o fifo client-server para leitura
    int client_server_read = open(CLIENT_SERVER, O_RDONLY);
    if (client_server_read == -1) {
        perror("SERVER: Dind't open client-server fifo to read");
        return -1;
    }

    Task t;
    Task_List list_tasks;
    char *list[MAX];
    char *copy = strdup(t.command);

    while(read(client_server_read,&t, sizeof(struct Task))>0){
        if(t.type == EXECUTE){
            int aux = task_id;
            set_id(t,task_id);
            task_id++;
            add_Task(&list_tasks,t);
            //Envio do id da tarefa para o servidor
            char fifoc_name[30];
            sprintf(fifoc_name,SERVER_CLIENT"%d",t.pid);

            int fdc = open(fifoc_name,O_WRONLY);
            if(fdc==-1){
                perror("SERVER: Dind't open server-client fifo to write");
                return -1;
            }
            write(fdc,&aux,sizeof (int));
            close(fdc);

            if(t.arg==ONE){ // -u
                argsToList(t.command,list);       //tem que se mudar o status da task
                execute_task_ONE(t, list);
            }
            else if(t.arg==PIPELINE){ // -p
                int n = commandsToList(t,list);
                execute_task_PIPELINE(list,n);
            }
        }
        else if(t.type == STATUS){

        }
    }


    return 0;


}