#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include"task.h"
#include<sys/wait.h>

#define MAX 300

#define WAITING 1
#define EXECUTING 2
#define FINISHED 0

#define CLIENT_SERVER "client_server"

int task_id=0;
//para o -u
int execute_task_ONE(Task t, char *list_args[]){
    //agora temos de meter o dup2 dentro dos casos  if t.arg == 0 de modo a 
    // para ele depois meter o output direito?
    pid_t pid = fork();
    int status;
    switch(pid){
        case -1:
            perror("Erro ao criar processo filho");
            return -1;
        case 0 :

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
        dup2(fderr,2);
        close(fdout);
        close(fderr);

        //não entendi esta parte
            execvp(list_args[0], list_args);
        default :
            wait(&status);
            if(WIFEXITED(status))
                printf("processo filho terminou normalmente com %d\n", WIFEXITED(status));
        else
                printf("processo filho não terminou normalmente.");
    }
    return 0;
}

    

//para o -p
int exec_onetask(Task arg){
    char *exec_tasks[10];
    char *string;
    int exec_ret = 0;
	int i=0;

	char* command = strdup(arg);

	string=strtok(command," ");
	
	while(string!=NULL){
		exec_tasks[i]=string;
		string=strtok(NULL," ");
		i++;
	}

	exec_tasks[i]=NULL;
	
	exec_ret=execvp(exec_tasks[0],exec_tasks);
	
	return exec_ret;
}

void execute_pipeline(Task tasks[], int num_tasks){
    

    int pipes[num_tasks -1][2];
    for(int i = 0; i< num_tasks; i++){
        if(i == 0){
            pipe(pipes[i]);
            switch(fork()){
                case -1:
                        perror("Erro ao criar pid filho");
                        break;

                case 0 : 
                        close(pipes[i][0]); //fecha o de leitura
                        dup2(pipes[i][1],1); //redereciona para o de escrita
                        exec_task(tasks[i]);
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
                        exec_task(tasks[i]);
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
                        exec_onetask(tasks[i]);
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
int main(int argc, char *argv[]){

    //cria fifo cliente-servidor
    if(mkfifo(CLIENT_SERVER, 0666) == -1){
        perror("mkfifo client-server");
        return -1;
    }


    //abre o fifo client-server para escrita
    int client_server_write = open(CLIENT_SERVER, O_WRONLY);
    if (client_server_write == -1) {
        perror("client-server_write open");
        return -1;
    }


    //abre o fifo client-server para leitura
    int client_server_read = open(CLIENT_SERVER, O_RDONLY);
    if (client_server_read == -1) {
        perror("client-server_read open");
        return -1;
    }

    Task t;
    Task_List list;
    char *list_args[MAX];
    char *copy = strdup(t.command);

    while(read(client_server_read,&t, sizeof(struct Task))>0){
        if(t.type == EXECUTE){
            if(t.arg==ONE){ // -u
                argsToList(t,list_args);
                add_Task(&list,t);
                execute_task_ONE(t, list_args);
            }
            else if(t.arg==MULTIPLE){ // -p
                //execute_pipeline(tasks, num_tasks);
                //não estão declaradas, necessário o tal parse
            }
            execute_task(t);
        }
        else if(t.type == STATUS){
            send_status(client_server_write, t.status);
        }
    }





    return 0;


}