#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ModuloA.h"
#include "ModuloD.h"

FILE *ficheiro;


/*Função que irá chamar o módulorespetivo na execução do programa.*/
void verificaModulo(char *listaMod[], char *nome_ficheiro){
    int flagA= 0;
    for(int i= 0; i< 4; i++){
        if(strcmp(listaMod[i],"f")== 0){
            flagA= mainModuloA(nome_ficheiro); // Siga módulo A
        }
        else if(strcmp(listaMod[i],"t")== 0){
            mainModuloB(nome_ficheiro, flagA); // Siga módulo B
        }
        else if(strcmp(listaMod[i],"c")== 0) mainModuloC(nome_ficheiro, flagA); // Siga módulo C
        else if(strcmp(listaMod[i], "d")== 0) mainModuloD(nome_ficheiro); // Siga módulo D
        else if(strcmp(listaMod[i],"@")== 0) break;
        else{
          printf("Modulo %s nao existe\n", listaMod[i]);
          return;
        }
    }
}


int main(int argc, char *argv[]) {

    char *modulo[4]= {"@","@","@","@"}, *comando, *nomeFicheiro=(char *)malloc(sizeof (char)* (strlen(argv[1])+1));
    strcpy(nomeFicheiro, argv[1]);

    //Nome do ficheiro de entrada
    alteraNomeDoFile(nomeFicheiro);
    //Ordem que o utilizador põe os argumento, nomeadamente o módulo que quer usar e o tipo de ficheiro de saída

    for(int i= 2, j= 0; i< argc -1; i= i+ 2){
        if(strcmp(argv[i], "-m")== 0){
            modulo[j]= argv[i+ 1];
            j++;
        }
        if(strcmp(argv[i], "-b")== 0){
            if(strcmp(argv[i+ 1], "K")== 0) alteraTamanho( 640*1024);
            else if(strcmp(argv[i+ 1], "m")== 0) alteraTamanho( 8388608);
            else if(strcmp(argv[i+ 1], "M")== 0) alteraTamanho( 67108864);
        }
        if(strcmp(argv[i], "-c")== 0){
            if(strcmp(argv[i+ 1], "r")== 0) alteraForcarCompressao(1);
        }
        if(strcmp(argv[i], "-d")== 0){
            if(strcmp(argv[i+ 1], "s")== 0) alteraDescompressao(1);
        }
    }
    // abre o arquivo para leitura
    ficheiro= fopen(nomeFicheiro, "rb");

    // verifica se o arquivo foi aberto com sucesso
    if (ficheiro!= NULL){
        //fclose(ficheiro);
        verificaModulo(modulo, nomeFicheiro);
    }
    else {
        printf("Arquivo inexistente");
    }
    return 0;
}
