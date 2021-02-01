#ifndef MODULOC_H_INCLUDED
#define MODULOC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "ModuloB.h"
#include "ModuloA.h"

int give_Number_C();
void preenche_Array_Cod_C(Caracter *array);
int conta_listas(Caracter lista);
int conta_Chars(Caracter *array, unsigned char *bloco, int tam_block);
int preenche_bloco_shaf(unsigned char *bloco_shaf, unsigned char *bloco, Caracter *array, int tam_bloco);
unsigned char *divide_bloco(unsigned char *bloco_shaf, int tam_bloco_shafa);
void promptC(int n_blocks, clock_t begin, clock_t end, int *lista_tam_blocos, int *lista_tam_blocos_compress, char *shafa);
void mainModuloC(char *file, int flagA);


#endif // MODULOC_H_INCLUDED
