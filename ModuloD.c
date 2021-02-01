#include "ModuloD.h"
#include "ModuloB.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

//Variáveis globais: a flag da descompressão (comando -d s), o ficheiro .cod e o ficheiro .shaf.
int flagD= 0;
FILE *codific;
FILE *shaf;


/*Função que altera a flag da descompressão caso o utilizador apenas queira o ficheiro rle e não o original.
Isto acontece quando o utilizador tem um ficheiro do tipo .rle.shaf e invoca a flag -d s.*/
void alteraDescompressao(int valor){
    flagD= valor;
}


/*Função que, dada o valor da flag (flag= 0 corresponde ao ficheiro .cod, flag= 1 corresponde ao ficheiro .shaf), lê números
do ficheiro, isto é, o número de blocos/ tamanho do bloco em causa para pudermos saber quantos blocos o ficheiro tem e/ou o tamanho
do bloco que vamos analisar.*/
int give_Number_D(int flag){
    int number= -1;
    unsigned char arroba= '@', ponto_virgula= ';', buffer[1]= "t";
    if(!flag) fread(buffer, 1, 1, codific);
        else fread(buffer, 1, 1, shaf);
    //if(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0) number= 0;
    while(memcmp(buffer, &arroba, 1)!= 0 && memcmp(buffer, &ponto_virgula, 1)!= 0){
        number= adiciona_digito(buffer[0], number);
        if(!flag) fread(buffer, 1, 1, codific);
        else fread(buffer, 1, 1, shaf);
    }
    return number;
}


/*Função que vai ler do ficheiro cod, bloco a bloco, a sequência de bits que codifica cada caractér e a guarda na posição correta
da struct Caracter que está preparada para guardar, por índice (cada índice corresponde ao caractér na tabela ASCII, por exemplo
índice 0 corresponde ao caractér NULO) essa sequência de bits.*/
void preenche_Array_Cod_D(Caracter *array){
    unsigned char buffer[1];
    unsigned char *arroba= "@", *ponto_virgula= ";";

    for(int i= 0; i< 256; i++){
        fread(buffer, 1, 1, codific);
        while(memcmp(buffer, ponto_virgula, 1)!= 0 && memcmp(buffer, arroba, 1)!= 0){
            insere_Bit(array, buffer[0], i);
            fread(buffer, 1, 1, codific);
        }
    }
}

/*Função que preenche a árvore binária com todas as codificações de cada caractér do ficheiro .cod.*/
ABin preenche_ABin(){
    unsigned char buffer[1];
    unsigned char *arroba= "@", *ponto_virgula= ";";

    ABin tree= malloc(sizeof(struct TreeBinary));
    tree-> letra= '\0';
    tree-> esq= tree-> dir= NULL;
    ABin auxiliar= tree;

    for(int i= 0; i< 256; i++){
        fread(buffer, 1, 1, codific);
        auxiliar= tree;
        while(memcmp(buffer, ponto_virgula, 1)!= 0 && memcmp(buffer, arroba, 1)!= 0){
            if(buffer[0]== '0'){
                if(!auxiliar-> esq){
                    ABin novo_zero= malloc(sizeof(struct TreeBinary));
                    novo_zero-> letra= '0';
                    novo_zero-> esq= novo_zero-> dir= NULL;
                    auxiliar-> esq= novo_zero;
                    auxiliar= auxiliar-> esq;

                }
                else auxiliar= auxiliar-> esq;
            }
            else{
                if(!auxiliar-> dir){
                    ABin novo_um= malloc(sizeof(struct TreeBinary));
                    novo_um-> letra= '1';
                    novo_um-> esq= novo_um-> dir= NULL;
                    auxiliar-> dir= novo_um;
                    auxiliar= auxiliar-> dir;
                }
                else auxiliar= auxiliar-> dir;
            }
            fread(buffer, 1, 1, codific);
        }
        auxiliar-> letra= (unsigned char) i;
    }
    return tree;
}


/*Função que, dada o bloco a analisar do ficheiro shaf, traduz cada byte na sequência binária correspondente.*/
unsigned char *translate_2_bits(unsigned char *bloco, int tam_block_shaf){
    unsigned char *bloco_shaf= malloc(tam_block_shaf* 8);
    int indice= 0;
    for(int i= 0; i< tam_block_shaf; i++){
        int byte= (int) bloco[i];
        for(int j= 8*i+ 7; j>= 8*i; j--){
            bloco_shaf[j]= (byte%2)+ 48;
            byte/= 2;
        }
    }
    return bloco_shaf;
}


/*Função que, dado um intervalo (de i a f), dá uma sub-sequência de bits da sequência de bits.*/
unsigned char *get_sub_string(unsigned char *bloco_shaf, int i, int f){
    unsigned char *substring= malloc(f- i);
    int indice= 0;
    for(; i< f; i++) substring[indice++]= bloco_shaf[i];
    return substring;
}


/*Função que, dada uma sequência de bits (neste caso será sempre a sub-sequência dada pela função supramencionada "get_sub_string")
e dada a struct array, procura a correspondência na struct de algum caractér com essa codificação (sub-string).*/
int find_Char(unsigned char *sub_string, Caracter *array, int tam){
    int indice= 0;
    Caracter lista= NULL;
    for(int i= 0; i< 256; i++){
        lista= array[i];
        while(lista){
            if(lista-> value== sub_string[indice] && !lista-> prox && indice== tam- 1) return i;
            else if(lista-> value== sub_string[indice]){
              lista= lista-> prox; indice++;
            }
            else lista= NULL;
        }
        indice= 0;
    }
    return -1;
}


int encontra_Sequencia(Caracter *array, unsigned char *bloco_shaf, int i, int p, int tam_block_shaf){
     if(!array[p]) return -1;
     int tamanho_byte= 0;
     Caracter lista= array[p];
     //printf("rr\n");
     while(lista-> value== bloco_shaf[i] && i< tam_block_shaf*8){
        tamanho_byte++;
        if(!lista-> prox) return tamanho_byte;
        lista= lista-> prox; i++;
     }
     return -1;
}


unsigned char *descodificaSF_Otimizada(Caracter *array, unsigned char *bloco_shaf, int tam_block_shaf, int tam_block_cod){
    int i= 0, indice= 0;
    int tam_geral= tam_block_shaf* 8;
    unsigned char *bloco_decompress= malloc(tam_block_cod);
    while(i< tam_geral){
        int p= 0;
        int tamanho_byte= encontra_Sequencia(array, bloco_shaf, i, p, tam_block_shaf);
        while(tamanho_byte== -1 && p< 256){
        p++;
        tamanho_byte= encontra_Sequencia(array, bloco_shaf, i, p, tam_block_shaf);
        }
        if(p< 256){
          bloco_decompress[indice++]= (unsigned char) p;
          if(indice>= tam_block_cod) return bloco_decompress;
          i+= tamanho_byte;
        }
        else break;
    }
    return bloco_decompress;
}

/*Função que descodifica bloco a bloco, cada bloco do ficheiro shaf, recorrendo à arvore binária construida com as codificações
do ficheiro .cod.*/
unsigned char *descodificaSF_Super_Otimizada(ABin tree, unsigned char *bloco_shaf, int tam_block_shaf, int tam_block_cod){
    int i= 0, indice= 0;
    int tam_geral= tam_block_shaf* 8;
    unsigned char *bloco_decompress= malloc(tam_block_cod);

    ABin auxiliar= tree;
    for(int i= 0; i< tam_geral; i++){
        if(indice>= tam_block_cod) return bloco_decompress;
        if(bloco_shaf[i]== '0'){
            if(!auxiliar-> esq){
                bloco_decompress[indice++]= auxiliar-> letra;
                auxiliar= tree-> esq;
            }
            else auxiliar= auxiliar-> esq;
        }
        else{
            if(!auxiliar-> dir){
                bloco_decompress[indice++]= auxiliar-> letra;
                auxiliar= tree->dir;
            }
            else auxiliar= auxiliar-> dir;
            }
    }
    return bloco_decompress;
}


/*Função que descodifica bloco a bloco, cada bloco do ficheiro shaf, tendo de chamar as funções "get_sub_string" e "find_Char".*/
unsigned char *descodificaSF(Caracter *array, unsigned char *bloco_shaf, int tam_block_shaf, int tam_block_cod){
    unsigned char *bloco_decompress= malloc(tam_block_cod);
    int i= 0, f= 1, indice= 0;
    while(i< tam_block_shaf*8 && f<= tam_block_shaf*8){
        unsigned char *sub_string= get_sub_string(bloco_shaf, i, f);
        int valor= find_Char(sub_string, array, f- i);
        if(valor== -1) f++; //nao encontrou
        else{
            unsigned char byte= (unsigned char) valor;
            bloco_decompress[indice++]= byte;
            i= f; f++;
        }
    }
    return bloco_decompress;
}



/*Função que calcula o tamanho do ficheiro original antes da compressão RLE.*/
int calcula_tam_bloco_original(int tam_block_cod, unsigned char *bloco_decompress){
    unsigned char nulo= '\0';
    int tamanho= 0;
    for(int i= 0; i< tam_block_cod; i++){
        if(memcmp(bloco_decompress+ i, &nulo, 1)== 0){
            if(i+ 2< tam_block_cod) tamanho+= (int)bloco_decompress[i+ 2];
            //printf("%d\n", bloco_decompress[i+ 2]);
            i+= 2;
        }
        else tamanho++;
    }
    return tamanho;
}


/*Função que faz o processo inverso à compressão RLE, bloco a bloco, obtendo assim cada bloco original do ficheiro original.*/
unsigned char *descomprime_RLE(int tam_bloco_final, unsigned char *bloco_decompress, int tam_bloco_shaf){
    unsigned char *bloco_final= malloc(tam_bloco_final);
    unsigned char nulo ='\0';
    int indice= 0;
    for(int i= 0; i< tam_bloco_shaf; i++){
        if(memcmp(bloco_decompress+ i, &nulo, 1)== 0){
            if(i+ 2< tam_bloco_shaf){
                int tam= (int)bloco_decompress[i+ 2];
                unsigned char letra= bloco_decompress[i+ 1];
                for(int j= 0; j< tam; j++) bloco_final[indice++]= letra;
                i+= 2;
                }
        }
        else{
            bloco_final[indice++]= bloco_decompress[i];
        }
    }
    return bloco_final;
}


/*Função que imprime um prompt do que foi feito ao longo do módulo.*/
void promptD(int n_blocks, int *block_size_shaf, int *block_size_original, clock_t begin, clock_t end, char *original){
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("Grupo 10, MIEI/CD, %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Modulo: D (descodificacao dum ficheiro shaf)\n");
    printf("Numero de blocos: %d\n", n_blocks);

    for(int i= 0, j= 1; i< n_blocks; j++, i++){
        printf("Tamanho antes/depois do ficheiro gerado (bloco %d): %d/%d\n", j, block_size_shaf[i], block_size_original[i]);
    }
    printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
    printf("Ficheiro gerado: %s\n", original);
    printf("\n");
}


/*Função que cria o ficheiro original quando é passado ao módulo D um ficheiro .rle.*/
void cria_File_Original(char *file){
    clock_t begin = clock();
    FILE *file_rle= fopen(file, "rb");

    char *file_original= malloc(sizeof(char)* strlen(file)+ 2);
    strcpy(file_original, file);
    strip_ext(file_original);
    FILE *original= fopen(file_original, "wb");
    unsigned long long total;
    long long n_blocks;
    unsigned long size_of_last_block;
    int block_size= 64*1024;
    n_blocks= fsize(file_rle, NULL, &block_size, &size_of_last_block);
    file_rle= fopen(file, "rb");
    total = (((n_blocks-1) * block_size)+ size_of_last_block);
    int totalito= (int) total;
    unsigned char *bloco_decompress= malloc(totalito+ 2);

    fread(bloco_decompress, 1, totalito, file_rle);
    int tam_bloco_final= calcula_tam_bloco_original(totalito, bloco_decompress);

    printf("A descomprimir ficheiro .rle (esta execucao pode demorar algum tempo)...\n");
    unsigned char *bloco_final= descomprime_RLE(tam_bloco_final, bloco_decompress, total);
    fwrite(bloco_final, 1, tam_bloco_final, original);
    free(bloco_final);
    free(bloco_decompress);
    fclose(original);
    fclose(file_rle);
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Grupo 10, MIEI/CD, %02d-%02d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Modulo: D (descodificacao dum ficheiro shaf)\n");
    printf("Tamanho antes/depois do ficheiro gerado: %d/%d\n", (total/1024)+ 1, (tam_bloco_final/1024)+ 1);
    printf("Tempo de execucao do modulo (milissegundos): %.01f\n", time_spent*1000);
    printf("Ficheiro gerado: %s\n", file_original);
    printf("\n");
}


/* Função que é responsável por toda a execução do módulo D.*/
void mainModuloD(char *file){
    if(strcmp(get_filename_ext(file), "rle")== 0){
      cria_File_Original(file);
      return;
    }
    char *cod= (char *)malloc(sizeof (char)*(strlen(file)+ 1));
    strcpy(cod, file);
    strip_ext(cod);
    char *nome_final= malloc(sizeof (char)*(strlen (cod)+ 5));
    strcpy(nome_final, cod);


    strcat(cod,".cod");
    codific= fopen(cod, "rb");
    shaf= fopen(file, "rb");

    if (!codific){
        printf("Ficheiro .cod inexistente. Tera de cria-lo primeiro.\n");
        return;
    }
    if (!shaf){
        printf("Ficheiro .shaf inexistente. Tera de cria-lo primeiro.\n");
        return;
    }


    /*Precisamos disto para ler os 3 primeiros bytes ("@N|R@") dos ficheiros .cod e .shaf (de certa forma, só nos interessa ler
    a partir do @, que é quando temos o número de blocos (no caso do ficheiro .cod) e o tamanho de cada bloco de cada ficheiro.*/
    unsigned char lixo_cod[3];
    unsigned char lixo_shaf[1];
    fread(lixo_cod, 1, 3, codific);
    fread(lixo_shaf, 1, 1, shaf);

    int n_blocks= give_Number_D(0); //0 é para file codificação (.cod)
    n_blocks= give_Number_D(1); //1 é para o file shaf (.shaf)

    /*Em ambos os "if's" é posta a flag correspondente se o utilizador quer o ficheiro original ou apenas o RLE.*/
    if(lixo_cod[1]== 'N') flagD= 1;
    if(lixo_cod[1]== 'R' && flagD== 0)strip_ext(nome_final);

    clock_t begin = clock();
    FILE *original= fopen(nome_final, "wb");

    /*Lista com o tamanho dos blocos shaf e original para o prompt do módulo.*/
    int *block_size_shaf= malloc(sizeof(n_blocks));
    int *block_size_original= malloc(sizeof(n_blocks));

    printf("Numero de blocos: %d\n", n_blocks);
    printf("A descomprimir os blocos (esta execucao pode demorar algum tempo)\n");
    /*Mesmo processo bloco a bloco.*/
    for(int i= 1, j= 0; i<= n_blocks; j++, i++){
        unsigned char lixote[1];
        printf("Bloco %d a descomprimir...\n", i);
        clock_t begin = clock();
        /*Tamanho do bloco i do ficheiro .cod (tam_block_cod) e do ficheiro .shaf (tam_block_shaf).*/
        int tam_block_cod= give_Number_D(0);
        int tam_block_shaf= give_Number_D(1);
        block_size_shaf[j]= tam_block_shaf;

        /*Alocamos memória para a struct Caracter com 256 nodos (cada nodo vai ter um caractér).*/
        //Caracter *array= malloc(sizeof(Caracter)*256);
        //for(int i= 0; i< 256; i++) array[i]= NULL;


        //preenche_Array_Cod_D(array);
        ABin tree= preenche_ABin();
        //imprimeTree(tree);



        /*Alocamos memória com o tamanho do bloco i do ficheiro shaf, onde vai ser guardado o bloco i do ficheiro .shaf.*/
        unsigned char *bloco= malloc(tam_block_shaf);
        fread(bloco, 1, tam_block_shaf, shaf);

        /*O "bloco_shaf" irá conter a sequência binária de bits do bloco .shaf que irá depois ser descodificado (processo inverso
        ao Shannon-Fano) dando origem ao "bloco_decompress".*/

        unsigned char *bloco_shaf= translate_2_bits(bloco, tam_block_shaf);

        free(bloco);
        unsigned char *bloco_decompress;
        //bloco_decompress= descodificaSF_Otimizada(array, bloco_shaf, tam_block_shaf, tam_block_cod);
        bloco_decompress= descodificaSF_Super_Otimizada(tree, bloco_shaf, tam_block_shaf, tam_block_cod);

        unsigned char *bloco_final= bloco_decompress;
        int tam_bloco_final= tam_block_cod;

        if(flagD== 0){
                tam_bloco_final= calcula_tam_bloco_original(tam_block_cod, bloco_decompress);
                bloco_final= descomprime_RLE(tam_bloco_final, bloco_decompress, tam_block_cod);
                free(bloco_decompress);
        }
        block_size_original[j]= tam_bloco_final;
        fwrite(bloco_final, 1, tam_bloco_final, original);
        if(i!= n_blocks) fread(lixote, 1, 1, shaf);
        clock_t end = clock();
        double time_spent = (double)(end - begin)/ CLOCKS_PER_SEC;
        printf("Tempo de descompressao (milissegundos): %.01f\n", time_spent*1000);
        free(bloco_final);
        //free(array);
    }
    fclose(original);
    fclose(codific);
    fclose(shaf);
    clock_t end = clock();
    promptD(n_blocks, block_size_shaf, block_size_original, begin, end, nome_final);
}
