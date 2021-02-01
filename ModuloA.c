#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "ModuloA.h"
#include "fsize.h"

FILE *file;
char *nomeDoFile;
char *file_original_freq; //FILE ORIGINAL
char *file_freq_nome; //FILE RLE
int forcarCompressao= 0;
unsigned long block_size= 64*1024;


/*Função que altera o nome do ficheiro com a terminação .rle.*/
void alteraNomeDoFile(char *nome){
    nomeDoFile= malloc( strlen(nome)+ 5);
    strcpy(nomeDoFile, nome);
    strcat(nomeDoFile,".rle");
}


/*Função que altera o tamanho dos blocos consoante a flag -c.*/
void alteraTamanho(int tam){
    block_size= tam;
}


/*Função que altera a flag da comppressão caso o utilizador force a compressão.*/
void alteraForcarCompressao(int valor){
    forcarCompressao= valor;
}


/*Função que transforma o tamanho do bloco comprimido (inteiro) em unsigned char para puder escrever no ficheiro .freq.*/
unsigned char *digits_in_ASCII(int tam_block_compress, int tam){
    char tam_compress_char[tam];
    sprintf(tam_compress_char, "%d", tam_block_compress);
    unsigned char *valor= malloc(tam);
    for(int i= 0; i< tam; i++) valor[i]= (unsigned char) tam_compress_char[i];
    return valor;
}


/*Função que pega bloco a bloco do ficheiro de entrada.*/
unsigned char *buscaBloco(int bloco, int total_blocks, int size_last_block){
    int tamBuffer;
    if(bloco+ 1== total_blocks && size_last_block< 1024) tamBuffer= block_size+ size_last_block;
    else if(bloco== total_blocks) tamBuffer= size_last_block;
    else tamBuffer= block_size;

    unsigned char *buffer= malloc(tamBuffer);
    fread(buffer, 1, tamBuffer, file);

    return buffer;
}



/*Fnção que verifica a compressão do 1º bloco.*/
double verificaCompressao(unsigned char *bloco, int total_blocks, int size_last_block, int number_block){
    double compressao;
    int i; //Variável de controlo que vamos ter de por no ciclo (que é o tamanho do 1º bloco)
    if(number_block+ 1== total_blocks && size_last_block< 1024) i= block_size+ size_last_block;
    else if(number_block== total_blocks) i= size_last_block;
    else i= block_size;
    //Vamos para a função tamanhoDoBlocoComprimido(ONDE ESTÁ O ERRO!!!!!!!!) que calcula o tamanho que o 1º bloco irá ter após a compressão
    int tamBlockCompress= tamanhoDoBlocoComprimido(bloco, i);
    compressao= ((double)(i- tamBlockCompress)/ (double)i)*100;
    //printf("Taxa de compressao: %.0lf%%\n", round(compressao));
    return compressao;

}


/*Função que calcula o tamanho que cada bloco irá ter após a compressão.*/
int tamanhoDoBlocoComprimido(unsigned char *bloco, int buffer_size){
    int length= 0, tamBlocoComprimido= 0, i= 0;
    unsigned char nulo= '\0';

    //Ciclo que calcula o tamanho do bloco comprimido
    while(i< buffer_size){
            //Ciclo que conta os elementos repetidos consecutivos
        for(int j= i; j< buffer_size && memcmp(bloco +j, bloco+ i, 1)==0; j++) length++;
        if(length> 255) length= 255;
        if(memcmp(bloco+ i, &nulo, 1)== 0) tamBlocoComprimido+= 3;
        //if(bloco[i]== '\0') tamBlocoComprimido+= 3;
        //Se o comprimento for menor que 4 não se comprime

        else if(length< 4) tamBlocoComprimido+= length;

        else tamBlocoComprimido+= 3;
        //if(length== 0) i++;
        //else
        i+= length;
        //if(length> 255) i+= 255;
        length= 0;
    }

    //printf("Valor do bloco comprimido: %d\n", tamBlocoComprimido);

    return tamBlocoComprimido;
}


/*Função que comprime bloco a bloco, dando origem ao bloco (rle) para ser escrito no ficheiro .rle de saída.*/
unsigned char *compressaoRLE(int number_of_block, int total_blocks, int size_of_last_block, unsigned char *bloco, int buffer_size, int tam_block_compressed){
    unsigned char nulo= '\0';

    int i= 0, length= 0, h= 0;

    unsigned char *block_compress= malloc(tam_block_compressed);

    while(i< buffer_size){
        for(int j= i; j< buffer_size && memcmp(bloco +j, bloco+ i, 1)==0; j++) length++;
        if(length> 255) length= 255;
        if(memcmp(bloco+ i, &nulo, 1)== 0){
            memcpy(block_compress+h, &nulo, 1);
            memcpy(block_compress+h+ 1, &nulo, 1);
            unsigned char tam= (unsigned char) length;
            memcpy(block_compress+h+ 2, &tam, 1);
            h+= 3;
        }
        else if(length< 4){
          memcpy(block_compress+ h, bloco+ i, length); h+= length;
        }
        else{
            memcpy(block_compress+h, &nulo, 1);
            memcpy(block_compress+h +1, bloco+ i, 1);
            unsigned char tam= (unsigned char) length;
            memcpy(block_compress+h+ 2, &tam, 1);
            h+= 3;
        }
        //if(length> 255) i+= 255;
        i+= length;
        length= 0;
    }
    return block_compress;
}


/*Função que conta as frequências de cada caracter.*/
int *conta_freq(unsigned char *bloco, int tam_block_compress){
    int *frequencias= malloc(256*sizeof(int)), posicao= 0;
    for(int i= 0; i< 256; i++) frequencias[i]= 0;
    for(int i= 0; i< tam_block_compress; i++){
        posicao= (int) bloco[i];
        frequencias[posicao]++;
    }
    return frequencias;
}


/*Função que caso o utilizador não force a compressão e a compressão seja menor que 5%, cria apenas o ficheiro .freq do ficheiro
de entrada.*/
char *freq_Original(int size_of_last_block, int n_blocks, char *nome_ficheiro){
    file= fopen(nome_ficheiro, "rb");
    int buffer_size, *frequencias, corret_number_blocks= n_blocks;
    if(size_of_last_block<1024) corret_number_blocks= n_blocks- 1;
    unsigned char init[1]= {'@'}, *ponto_virgula= ";", *zero= "0";

    file_original_freq= malloc(strlen(nome_ficheiro)+ 5);
    strcpy(file_original_freq, nome_ficheiro);
    strcat(file_original_freq,".freq");
    FILE *freq= fopen(file_original_freq, "wb");
    printf("A gerar o ficheiro %s\n(esta execucao pode demorar algum tempo)\n", file_original_freq);
    unsigned char inicio[3]={'@', 'N', '@'};
    int tam = (int)((trunc(log10(corret_number_blocks))+1)*sizeof(unsigned char));
    unsigned char *number= digits_in_ASCII(corret_number_blocks, tam);
    fwrite(inicio, 1, 3, freq);
    fwrite(number, 1, tam, freq);

    for(int number_block= 1; number_block<= corret_number_blocks; number_block++){
        printf("Bloco %d a calcular as frequencias...\n", number_block);
        unsigned char *bloco= buscaBloco(number_block, n_blocks, size_of_last_block);
        if(number_block+ 1== n_blocks && size_of_last_block< 1024) buffer_size= block_size+ size_of_last_block;
        else if(number_block== n_blocks) buffer_size= size_of_last_block;
        else buffer_size= block_size;
        frequencias= conta_freq(bloco, buffer_size);
        free(bloco);
        fwrite(init, 1, 1, freq);

        int tam = (int)((trunc(log10(buffer_size))+1)*sizeof(unsigned char));
        unsigned char *valor= digits_in_ASCII(buffer_size, tam);
        fwrite(valor, 1, tam, freq);
        fwrite(init, 1, 1, freq);
        int buffer= -1;
        for(int i= 0; i< 256; i++){
            if(buffer!= frequencias[i]){
                if(frequencias[i]== 0) fwrite(zero, 1, 1, freq);
                else{
                    tam = (int)((trunc(log10(frequencias[i]))+ 1)*sizeof(unsigned char));
                    valor= digits_in_ASCII(frequencias[i], tam);
                    fwrite(valor, 1, tam, freq);
                }
            }
            if(i!= 255) fwrite(ponto_virgula, 1, 1, freq);
            buffer= frequencias[i];
        }
    }
    unsigned char *fim= "@0";
    fwrite(fim, 1, 2, freq);
    fclose(freq);
    return file_original_freq;
}


/*Função que imprime um prompt do que foi feito ao longo do módulo.*/
void promptA(int flag, char *nome_ficheiro, int n_blocks, int block_size, int size_of_last_block, int total, char *nomeDoFile, float compress, int corret_number_blocks, int *lista_tam_compress, char *file_freq_nome, clock_t begin, clock_t end){
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("Grupo 10, MIEI/CD, %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Modulo: A (calculo das frequencias dos simbolos) \n");
    printf("Nome do file: %s\n", nome_ficheiro);
    if(!flag){
        printf("Tamanho da compressao menor que 5%%\n");
        printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
        printf("Ficheiros gerados: %s\n", file_freq_nome);
    }


    else{
    printf("Numero de blocos: %d\n", n_blocks);
    if(n_blocks>1) printf("Tamanho dos blocos analisados no ficheiro original: %d/%d bytes\n", block_size, size_of_last_block);
    else printf("Tamanho dos blocos analisados no ficheiro original: %d bytes\n", total);
    printf("Compressao RLE: %s (%.0lf %% compressao)\n", nomeDoFile, ((total-compress)/ total)*100);
    printf("Tamanho dos blocos analisados no ficheiro RLE: ");
        for(int i= 0; i< corret_number_blocks; i++){
            if(corret_number_blocks== i+ 1) printf("%d bytes\n", lista_tam_compress[i]);
            else printf("%d/", lista_tam_compress[i]);
        }
    printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
    printf("Ficheiros gerados: %s, %s\n", nomeDoFile, file_freq_nome);
    printf("\n");
    }
}

/* Função que é responsável por toda a execução do módulo A.*/
int mainModuloA(char *nome_ficheiro){
    //Cálculo do tamanho do file, do número de blocos e do tamanho do último bloco (usando a função fsize()
    clock_t begin = clock();
    file= fopen(nome_ficheiro, "rb");

    unsigned long long total;
    long long n_blocks;
    unsigned long size_of_last_block;
    n_blocks= fsize(file, NULL, &block_size, &size_of_last_block);
    total = (n_blocks-1) * block_size + size_of_last_block;
    //printf("%d  %d  %d\n", n_blocks, total, size_of_last_block);
    if(total< 1024){
        printf("Tamanho do file menor que 1024 kBytes\n"); return;
    }  // File menor que 1kByte


    //file= malloc(total);
    file= fopen(nome_ficheiro, "rb");


        int buffer_size= 0, corret_number_blocks= n_blocks; double compress= 0;
        FILE *file_compressed= fopen(nomeDoFile, "wb"); //Abrir RLE

        file_freq_nome= malloc(sizeof (char) *(strlen(nomeDoFile)+ 6));
        strcpy(file_freq_nome, nomeDoFile);

        strcat(file_freq_nome,".freq");

        unsigned char init[1]= {'@'};
        unsigned char *ponto_virgula= ";";
        unsigned char *zero= "0";
        FILE *file_freq= fopen(file_freq_nome, "wb");


        if(size_of_last_block<1024) corret_number_blocks= n_blocks- 1;

        int lista_tam_compress[corret_number_blocks];
        printf("Numero de blocos: %d\n", corret_number_blocks);
        for(int number_of_block= 1; number_of_block<= corret_number_blocks; number_of_block++){
            unsigned char *bloco= buscaBloco(number_of_block, n_blocks, size_of_last_block);
            if(number_of_block== 1){
                double compressao= verificaCompressao(bloco, n_blocks, size_of_last_block, 1);
                //printf("%d\n", compressao);
                if(compressao< 5 && !forcarCompressao){
                    fclose(file_compressed);
                    fclose(file_freq);
                    remove(file_freq_nome);
                    remove(nomeDoFile);
                    char *file_original_freq= freq_Original(size_of_last_block, n_blocks, nome_ficheiro);
                    clock_t end = clock();
                    promptA(0, nome_ficheiro, 0, 0, 0, total, NULL, 0, 0, NULL, file_original_freq, begin, end);
                    return 1;
                }
                else{
                    printf("A gerar os ficheiros %s %s\n(esta execucao pode demorar algum tempo)\n", nomeDoFile, file_freq_nome);
                    unsigned char inicio[3]={'@', 'R', '@'};
                    int tam = (int)((trunc(log10(corret_number_blocks))+1)*sizeof(unsigned char));
                    unsigned char *number= digits_in_ASCII(corret_number_blocks, tam);
                    fwrite(inicio, 1, 3, file_freq);
                    fwrite(number, 1, tam, file_freq);
                }
            }
            printf("Bloco %d a comprimir em RLE e a calcular as frequencias...\n", number_of_block);
            if(number_of_block+ 1== n_blocks && size_of_last_block< 1024) buffer_size= block_size+ size_of_last_block;
            else if(number_of_block== n_blocks) buffer_size= size_of_last_block;
            else buffer_size= block_size;


            int tam_block_compress= tamanhoDoBlocoComprimido(bloco, buffer_size);


            lista_tam_compress[number_of_block- 1]= tam_block_compress;
            compress+= tam_block_compress;
            unsigned char *bloco_compress= compressaoRLE(number_of_block, n_blocks, size_of_last_block, bloco, buffer_size, tam_block_compress);
            free(bloco);
            fwrite(init, 1, 1, file_freq);
            int tam = (int)((trunc(log10(tam_block_compress))+1)*sizeof(unsigned char));
            unsigned char *valor= digits_in_ASCII(tam_block_compress, tam);
            fwrite(valor, 1, tam, file_freq);
            fwrite(init, 1, 1, file_freq);

            int *frequencias= conta_freq(bloco_compress, tam_block_compress);

            int buffer= -1;
            for(int i= 0; i< 256; i++){
                if(buffer!= frequencias[i]){
                    if(frequencias[i]== 0) fwrite(zero, 1, 1, file_freq);
                    else{
                      tam = (int)((trunc(log10(frequencias[i]))+ 1)*sizeof(unsigned char));
                    valor= digits_in_ASCII(frequencias[i], tam);
                    fwrite(valor, 1, tam, file_freq);
                    }
                }
                if(i!= 255) fwrite(ponto_virgula, 1, 1, file_freq);
                buffer= frequencias[i];
            }
            free(frequencias);
            fwrite(bloco_compress, 1, tam_block_compress, file_compressed);
            free(bloco_compress);
        }
        unsigned char *fim= "@0";
        fwrite(fim, 1, 2, file_freq);

        fclose(file_compressed);
        fclose(file_freq);
        fclose(file);
        clock_t end = clock();

        promptA(1, nome_ficheiro, n_blocks, block_size, size_of_last_block, total, nomeDoFile, compress, corret_number_blocks, lista_tam_compress, file_freq_nome, begin, end);

        return 2;
    }
