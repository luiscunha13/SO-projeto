#include<stdio.h>
#include<string.h>

#define MAX 300

int main(int argc, char * argv[]){

    if (argc < 2){
        printf("não tem argumentos suficientes\n");
        //não sei se é para fazer mais cenas
        return 0;
    }

    if(strcmp(argv[1],"execute") == 0){

    }
    else if(strcmp(argv[1],"status") == 0){

    }
    else printf("comando inválido");

}