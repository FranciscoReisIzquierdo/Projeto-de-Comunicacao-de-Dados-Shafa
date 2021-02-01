#include "ModuloC.h"
#include "ModuloB.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>



FILE *codificacao;


/*Função que lê números do ficheiro, isto é, o número de blocos/ tamanho do bloco em causa para pudermos saber quantos blocos o ficheiro tem e/ou o tamanho
do bloco que vamos analisar.*/
int give_Number_C(){
    int number= -1;
    unsigned char arroba= '@', ponto_virgula= ';', buffer[1]= "t";
    fread(buffer, 1, 1, codificacao);
    //if(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0) number= 0;
    while(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0){
        //if(memcmp(buffer, &arroba, 1)== 0 || memcmp(buffer, &ponto_virgula, 1)== 0) break;
        number= adiciona_digito(buffer[0], number);
        fread(buffer, 1, 1, codificacao);
    }
    return number;
}


/*Função que vai ler do ficheiro cod, bloco a bloco, a sequência de bits que codifica cada caractér e a guarda na posição correta
da struct Caracter que está preparada para guardar, por índice (cada índice corresponde ao caractér na tabela ASCII, por exemplo
índice 0 corresponde ao caractér NULO) essa sequência de bits.*/
void preenche_Array_Cod_C(Caracter *array){
    unsigned char buffer[1];
    unsigned char *arroba= "@", *ponto_virgula= ";";

    for(int i= 0; i< 256; i++){
            fread(buffer, 1, 1, codificacao);
        while(memcmp(buffer, ponto_virgula, 1)!= 0 && memcmp(buffer, arroba, 1)!= 0){
            insere_Bit(array, buffer[0], i);
            fread(buffer, 1, 1, codificacao);
        }
    }
}


/*Função que conta o número de bits (nodos) do caractér codificado em causa*/
int conta_listas(Caracter lista){
    int number_of_bits= 0;
    while(lista){
      number_of_bits++;
      lista= lista-> prox;
    }
    return number_of_bits;
}

/*Função que conta o número total de bits (nodos) dos 256 caractéres por cada bloco*/
int conta_Chars(Caracter *array, unsigned char *bloco, int tam_block){
    int number_of_bits= 0;
    for(int i= 0; i< tam_block; i++){
        number_of_bits+= conta_listas(array[bloco[i]]);
    }
    return number_of_bits;
}


/*Função que vai preenchendo o bloco correspondente do ficheiro .shaf (em binário e antes da codificação)*/
int preenche_bloco_shaf(unsigned char *bloco_shaf, unsigned char *bloco, Caracter *array, int tam_bloco){
    int indice= 0;

    for(int i= 0; i< tam_bloco; i++){
        Caracter lista= array[bloco[i]];
        while(lista){
            bloco_shaf[indice++]= lista-> value;
            lista= lista-> prox;
        }
    }
    return indice;
}

/*Função que transforma, a partir de sequências binárias de 8 bits, nos bytes correspondentes que irão estar no bloco respetivo
do ficheiro .shaf.*/
unsigned char *divide_bloco(unsigned char *bloco_shaf, int tam_bloco_shafa){
    int byte= 0, count= 0;
    unsigned char *array_bytes= malloc(tam_bloco_shafa);
    //int fim= (tam_bloco_shafa/8)*8;
    for(int i= 0; i< tam_bloco_shafa; i+= 8){
        int elevado= 0;
        for(int j= i+ 7; j>= i; j--){
                if(bloco_shaf[j]== '1') byte+= pow(2, elevado);
                elevado++;
        }
        array_bytes[count++]= (unsigned char) byte;
        byte= 0;
        elevado= 0;
    }
    /*for(int i= fim; i< tam_bloco_shafa; i++){
        array_bytes[count++]= bloco_shaf[i];
    }*/
    return array_bytes;
}

/*Função que imprime um prompt do que foi feito ao longo do módulo.*/
void promptC(int n_blocks, clock_t begin, clock_t end, int *lista_tam_blocos, int *lista_tam_blocos_compress, char *shafa){
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("Grupo 10, MIEI/CD, %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Modulo: C (codificacao dum ficheiro de simbolos)\n");
    printf("Numero de blocos: %d\n", n_blocks);

    int tam_global= 0, tam_global_cod= 0;
    for(int i= 0, j= 1; i< n_blocks; j++, i++){
        tam_global+= lista_tam_blocos[i];
        tam_global_cod+= lista_tam_blocos_compress[i];
        printf("Tamanho antes/depois & taxa de compressao (bloco %d): %d/%d\n", j, lista_tam_blocos[i], lista_tam_blocos_compress[i]);
    }
    int compressao_global= ((double)(tam_global- tam_global_cod)/ (double)tam_global)*100;

    printf("Taxa de compressao global: %.0lf%%\n", round(compressao_global));
    printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
    printf("Ficheiro gerado: %s\n", shafa);
    printf("\n");
}


/* Função que é responsável por toda a execução do módulo C.*/
void mainModuloC(char *file, int flagA){
    if(flagA== 2){
        char *auxiliar= malloc(strlen(file)+ 5);
        strcpy(auxiliar, file);
        strcat(auxiliar,".rle");
        file= auxiliar;
    }
    char *cod= malloc(strlen(file)+ 5);
    strcpy(cod, file);
    strcat(cod,".cod");

    if (!fopen(cod, "rb")){
        printf("Ficheiro .cod inexistente. O ficheiro .cod correspondente ao seu ficheiro de entrada deve estar na diretoria\n");
        return;
    }

    char *shafa= malloc(strlen(file)+ 6);
    strcpy(shafa, file);
    strcat(shafa,".shaf");


    clock_t begin = clock();
    FILE *shaf= fopen(shafa, "wb");
    FILE *entry= fopen(file, "rb");
    codificacao= fopen(cod, "rb");

    unsigned char lixo[3];
    fread(lixo, 1, 3, codificacao);

    int n_blocks= give_Number_C();
    //printf("N_blocks: %d\n", n_blocks);

    int tam = (int)((trunc(log10(n_blocks))+1)*sizeof(unsigned char));
    unsigned char *number= digits_in_ASCII(n_blocks, tam), *arroba= "@", *ponto_virgula= ";";
    fwrite(arroba, 1, 1, shaf);
    fwrite(number, 1, tam, shaf);
    fwrite(arroba, 1, 1, shaf);

    int *lista_tam_blocos= malloc(sizeof(n_blocks));
    int *lista_tam_blocos_compress= malloc(sizeof(n_blocks));
    printf("Numero de blocos: %d\n", n_blocks);
    printf("A gerar o ficheiro %s (esta execucao pode demorar algum tempo)\n", shafa);
    for(int i= 1, j= 0; i<= n_blocks; j++, i++){
        printf("Bloco %d a comprimir...\n", i);
        clock_t begin = clock();
        int tam_block= give_Number_C();
        lista_tam_blocos[j]= tam_block;


        unsigned char *bloco= malloc(tam_block);
        fread(bloco, 1, tam_block, entry);

        Caracter *array= malloc(sizeof(Caracter)*256);
        for(int i= 0; i< 256; i++) array[i]= NULL;

        preenche_Array_Cod_C(array);

        int tam_bloco_shafa= conta_Chars(array, bloco, tam_block);

        lista_tam_blocos_compress[j]= tam_bloco_shafa/8;

        tam_bloco_shafa= (tam_bloco_shafa/8)*8 +8;
        unsigned char *bloco_shaf= malloc(tam_bloco_shafa);

        int indice= preenche_bloco_shaf(bloco_shaf, bloco, array, tam_block);
        for(; indice< tam_bloco_shafa; indice++) bloco_shaf[indice]= '0';
        //tam_bloco_shafa= (tam_bloco_shafa/8)*8;
        unsigned char *array_bytes= divide_bloco(bloco_shaf, tam_bloco_shafa);

        int tam = (int)((trunc(log10(tam_bloco_shafa/ 8))+1)*sizeof(unsigned char));
        unsigned char *number= digits_in_ASCII(tam_bloco_shafa/ 8, tam);
        fwrite(number, 1, tam, shaf);
        fwrite(arroba, 1, 1, shaf);

        fwrite(array_bytes, 1, tam_bloco_shafa/8, shaf);

        clock_t end = clock();
        double time_spent = (double)(end - begin)/ CLOCKS_PER_SEC;
        printf("Tempo de compressao (milissegundos): %.01f\n", time_spent*1000);

        free(bloco);
        free(array);
        free(bloco_shaf);
        free(array_bytes);
        if(i!= n_blocks) fwrite(arroba, 1, 1, shaf);
    }
    fclose(shaf);
    fclose(codificacao);
    fclose(entry);
    clock_t end = clock();
    promptC(n_blocks, begin, end, lista_tam_blocos, lista_tam_blocos_compress, shafa);

}
