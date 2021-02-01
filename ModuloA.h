#ifndef MODULO1_H_INCLUDED
#define MODULO1_H_INCLUDED

int TAMANHO;
char *nomeDoFile;
char *file_original_freq; //FILE ORIGINAL
char *file_freq_nome; //FILE RLE

void alteraTamanho(int tam);
unsigned char *buscaBloco(int bloco, int total_blocks, int size_last_block);
int mainModuloA(char *nome_ficheiro);
double verificaCompressao(unsigned char *bloco, int total_blocks, int size_last_block, int number_block);
int tamanhoDoBlocoComprimido(unsigned char *bloco, int buffer_size);
void alteraForcarCompressao(int valor);
unsigned char *compressaoRLE(int number_of_block, int total_blocks, int size_of_last_block, unsigned char *bloco, int buffer_size, int tam_block_compressed);
unsigned char *digits_in_ASCII(int tam_block_compress, int tam);
int *conta_freq(unsigned char *bloco, int tam_block_compress);

#endif // MODULO1_H_INCLUDED
