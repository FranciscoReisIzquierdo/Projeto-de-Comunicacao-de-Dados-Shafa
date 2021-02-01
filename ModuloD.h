#ifndef MODULOD_H_INCLUDED
#define MODULOD_H_INCLUDED
#include "ModuloB.h"

typedef struct TreeBinary{
    unsigned char letra;
    struct TreeBinary *esq, *dir;
}*ABin;

void alteraDescompressao(int valor);
int give_Number_D(int flag);
void preenche_Array_Cod_D(Caracter *array);
unsigned char *translate_2_bits(unsigned char *bloco, int tam_block_shaf);
unsigned char *get_sub_string(unsigned char *bloco_shaf, int i, int f);
int find_Char(unsigned char *sub_string, Caracter *array, int tam);
unsigned char *descodificaSF(Caracter *array, unsigned char *bloco_shaf, int tam_block_shaf, int tam_block_cod);
int calcula_tam_bloco_original(int tam_block_cod, unsigned char *bloco_decompress);
unsigned char *descomprime_RLE(int tam_bloco_final, unsigned char *bloco_decompress, int tam_bloco_shaf);
void mainModuloD(char *file);



#endif // MODULOD_H_INCLUDED

