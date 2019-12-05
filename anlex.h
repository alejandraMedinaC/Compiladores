#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define TAMBUFF 5
#define TAMCMLEX 50
#define TAMLEX 50
#define TAMHASH 101



typedef struct entrada{
    char compLex[TAMCMLEX];
    char lexema[TAMLEX];	
    struct entrada *tipoDato; // null puede representar variable no declarada	

} entrada;

typedef struct {
    char compLex[50];
    entrada *pe;
} token;

void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
int linea();
