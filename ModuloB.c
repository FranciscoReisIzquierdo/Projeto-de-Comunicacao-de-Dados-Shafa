#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "ModuloB.h"
#include "ModuloA.h"

FILE *freq;
FILE *cod;
char *nomeDoFile;

/*Função que lê números do ficheiro, isto é, o número de blocos/ tamanho do bloco em causa para pudermos saber quantos blocos o ficheiro tem e/ou o tamanho
do bloco que vamos analisar.*/
int give_Number(){
    int number= -1;
    unsigned char arroba= '@', ponto_virgula= ';', buffer[1]= "t";
    fread(buffer, 1, 1, freq);
    //if(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0) number= 0;
    while(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0){
        //if(memcmp(buffer, &arroba, 1)== 0 || memcmp(buffer, &ponto_virgula, 1)== 0) break;
        number= adiciona_digito(buffer[0], number);
        fread(buffer, 1, 1, freq);
    }
    return number;
}


/*Função que devolve a extensão do nome ficheiro.*/
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


/*Função que recebe um inteiro e o adiciona ao buffer.*/
int adiciona_digito(unsigned char buffer, int number){
    if(number== -1) number= 0;
    number*= 10;
    number+= (int)buffer- 48;
    return number;
}

/*Função que calcula as frequências de cada caractér do ficheiro .freq.*/
int *give_Frequencias(){
    int *frequencias= malloc(256*sizeof(int));
    int anterior= -1;
    for(int i= 0; i< 256; i++){
        int ocorr= give_Number();
        if(ocorr== -1) frequencias[i]= anterior;
        else{
            frequencias[i]= ocorr;
            anterior= ocorr;
        }
    }
    return frequencias;
}

/*Função que troca valores de dois índices.*/
void swap(int *array, int i, int indice){
    int temp= array[i];
    array[i]= array[indice];
    array[indice]= temp;
}

/*Função que gera o array de pares ordenado decrescentemente.*/
int *gera_Pares(int *frequencias){
    int posicao= 0;
    int *pares= malloc(sizeof(int)*512);
    for(int i= 0; i< 512; i++){
        pares[i]= posicao;
        pares[++i]= frequencias[posicao];
        posicao++;
    }
    return pares;
}


/*Função que ordena decrescentemente o array de frequências.*/
int *reverse_Sort(int *frequencias){
    for(int i= 1; i< 512; i+= 2){
            int maior= frequencias[i];
            int indice= i;
        for(int j= i; j< 512; j+= 2){
            if(maior< frequencias[j]){
                maior= frequencias[j];
                indice= j;
            }
        }
        swap(frequencias, i, indice);
        swap(frequencias, i- 1, indice- 1);
    }
    return frequencias;
}


/*Função que insere o bit ao array de structs Caracter.*/
void insere_Bit(Caracter *array, unsigned char bit, int posicao){
    Caracter inicial= array[posicao];
    if(!inicial){
        inicial= malloc(sizeof(struct nodo));
        inicial-> value= bit;
        inicial-> prox= NULL;
        array[posicao]= inicial;
    }
    else{
        Caracter aux= inicial;
        for(; aux-> prox; aux= aux-> prox);
        Caracter new_nodo= malloc(sizeof(struct nodo));
        new_nodo-> value= bit;
        new_nodo-> prox= NULL;
        aux-> prox= new_nodo;
    }
}


/*Função que adiciona os bits correspondentes de cada caracter ao array de structs Caracter.*/
void add_bit_to_code(Caracter *array, unsigned char bit, int *pares, int inicio, int fim){
    for(; inicio<= fim; inicio+= 2){
        insere_Bit(array, bit, pares[inicio- 1]);
    }
}


/*Função que retorna a frequência total entre um intervalo de caracteres.*/
int give_Tamanho(int *pares, int inicio, int meio){
    int count= 0;
    for(; inicio<= meio; inicio+= 2) count+= pares[inicio];
    return count;
}

/*Função que divide a meio o tamanho (frequência total entre um intervalo de caracteres)*/
int divide(int inicio, int fim, int *pares){
    int count= 0;
    for(int i= inicio; i<= fim; i+= 2) count+= pares[i];
    //if((count/2)*2< count) return (count/2)+ 1;
    //else return count/2;
    return count;
}

/*
int calcular_melhor_divisao(int *pares, int i, int j){
    int div=i, g1=0;
    int total, mindif, dif;
    total= mindif=dif=divide(pares,i,j);
    do{
            g1=g1+pares[div];
            dif=abs(2*g1-total);
            if (dif< mindif){
                div= div+2;
                mindif= dif;
                }
            else dif= mindif+1;
            }
    while (dif!=mindif);
    return div- 2;
    }*/


/*Função que encontra o meio correto da frequência total entre um intervalo de caracteres*/
int findMeio(int *pares, int inicio, int fim){
    int count= pares[inicio], indice= inicio+ 2, tam_bloco= divide(inicio, fim, pares);
    while(abs(tam_bloco- count* 2)/ 2 > abs(tam_bloco- (count+ pares[indice])* 2)/ 2){
        count+= pares[indice];
        indice+= 2;
    }

    return indice -2;
}


/*Função recursiva que percorre o array pares e codifica cada caracter numa sequência binária (bits).*/
void recursive_SF(Caracter *array, int *pares, int inicio, int fim){
    //printf("Inicio: %d\n", inicio);
    if(inicio!= fim){
        int meio= findMeio(pares, inicio, fim);
        //int meio= calcular_melhor_divisao(pares, inicio, fim);
        //printf("Meio: %d\n", meio);
        add_bit_to_code(array, '0', pares, inicio, meio);

        add_bit_to_code(array, '1', pares, meio+ 2, fim);

        recursive_SF(array, pares, inicio, meio);
        recursive_SF(array, pares, meio+2, fim);
    }
}


/*Função que imprime um prompt do que foi feito ao longo do módulo.*/
void promptB(char *file_cod, int *lista_tam_blocos, clock_t begin, clock_t end, int n_blocks){
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("Grupo 10, MIEI/CD, %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Modulo: B ( (calculo dos codigos dos simbolos) \n");
    printf("Numero de blocos: %d\n", n_blocks);

    printf("Tamanho dos blocos analisados no ficheiro de simbolos: ");
    for(int i= 0; i< n_blocks; i++){
        if(n_blocks == i+ 1) printf("%d bytes\n", lista_tam_blocos[i]);
        else printf("%d/", lista_tam_blocos[i]);
    }
    printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
    printf("Ficheiro gerado: %s\n", file_cod);
    printf("\n");
}


/*Função que retira a extensão do nome ficheiro.*/
void strip_ext(char *fname){
    char *end = fname + strlen(fname);
    while (end > fname && *end != '.') {
        --end;
    }
    if (end > fname) {
        *end = '\0';
    }
}


/* Função que é responsável por toda a execução do módulo B.*/
void mainModuloB(char *file, int flagA){

    clock_t begin = clock();
    if(flagA== 1) file= file_original_freq;
    else if(flagA== 2) file= file_freq_nome;
    else if(strcmp(get_filename_ext(file), "freq")!= 0){
      printf("O Modulo B tem de receber um ficheiro do tipo .freq\n");
      return;
    }

    char *file_cod= malloc(sizeof (char) *(strlen(file)+ 1));
    freq= fopen(file, "rb");
    strcpy(file_cod, file);
    strip_ext(file_cod);
    strcat(file_cod,".cod");


    unsigned char lixo[3];

    fread(lixo, 1, 3, freq);

    cod= fopen(file_cod, "wb");

    fwrite(lixo, 1, 3, cod);

    int n_blocks= give_Number();
    int *lista_tam_blocos= malloc(sizeof (int)* n_blocks);


    int tam = (int)((trunc(log10(n_blocks))+1)*sizeof(unsigned char));
    unsigned char *number= digits_in_ASCII(n_blocks, tam), *arroba= "@", *ponto_virgula= ";";
    fwrite(number, 1, tam, cod);
    fwrite(arroba, 1, 1, cod);


    int tam_bloco= 0;
    printf("Numero de blocos: %d\n", n_blocks);
    printf("A gerar o ficheiro %s (esta execucao pode demorar algum tempo)\n", file_cod);
    for(int bloco= 1, j= 0; bloco<= n_blocks; j++, bloco++){
        printf("A gerar a codificacao (binaria) dos caracteres do bloco %d...\n", bloco);
        tam_bloco= give_Number();
        tam = (int)((trunc(log10(tam_bloco))+1)*sizeof(unsigned char));
        number= digits_in_ASCII(tam_bloco, tam);

        lista_tam_blocos[j]= tam_bloco;
        fwrite(number, 1, tam, cod);
        fwrite(arroba, 1, 1, cod);

        int *frequencias= give_Frequencias();
        //for(int i= 0; i< 256; i++) printf("%d ", frequencias[i]);
        int *pares= gera_Pares(frequencias);
        free(frequencias);

        pares= reverse_Sort(pares); //Posições pares temos as posições na tabela ASCII, posições ímpares temos as suas frequências
        //for(int i= 0; i< 512; i+= 2) printf("%d-%d ", pares[i], pares[i+1]);
        Caracter *array= malloc(sizeof(Caracter)*256);
        for(int i= 0; i< 256; i++) array[i]= NULL;
        int fim= 1;

        for(; fim<= 511; fim+= 2) if(pares[fim]== 0) break;
        if(fim!= 1) fim-= 2;
        //if(bloco== 1) printf("%d\n", fim);
        //for(int i= 0; i<= fim; i+= 2) printf("%d-%d ", pares[i], pares[i+1]);

        recursive_SF(array, pares, 1, fim);
        free(pares);
        for(int i= 0; i< 256; i++){
            while(array[i]){
                unsigned char buffer[1];
                buffer[0]= array[i]-> value;
                fwrite(buffer, 1, 1, cod);
                array[i]= array[i]-> prox;
            }
            if(i!= 255) fwrite(ponto_virgula, 1, 1, cod);
        }
        fwrite(arroba, 1, 1, cod);
        free(array);

    }
    fwrite("0", 1, 1, cod);
    fclose(cod);
    clock_t end = clock();
    promptB(file_cod, lista_tam_blocos, begin, end, n_blocks);

}
