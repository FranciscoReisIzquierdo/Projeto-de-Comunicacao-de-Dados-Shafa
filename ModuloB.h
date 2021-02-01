#ifndef MODULOB_H_INCLUDED
#define MODULOB_H_INCLUDED


typedef struct nodo{
    unsigned char value;
    struct nodo *prox;
}*Caracter;

int give_Number();
const char *get_filename_ext(const char *filename);
int adiciona_digito(unsigned char buffer, int number);
void recursive_SF(Caracter *array, int *pares, int inicio, int fim);
void insere_Bit(Caracter *array, unsigned char bit, int posicao);
void strip_ext(char *fname);



#endif // MODULOB_H_INCLUDED
