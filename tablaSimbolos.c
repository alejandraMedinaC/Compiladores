#include "anlex.h"
entrada *tabla;			
int tamTabla=TAMHASH;		
int elems=0;			

int h(const char *k, int m){ 
    unsigned h=0,g;
    int i;
    for (i=0;i<strlen(k);i++){
	h=(h << 4) + k[i];
	if ( (g=h&0xf0000000) ){
        	h=h^(g>>24);
		h=h^g;
        }
    }
    return h%m;
}

void initTabla(){	
    int i=0;
    tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
    for(i=0;i<tamTabla;i++){
        strcpy(tabla[i].compLex,"-1");
    }
}

int esprimo(int n){
    int i;
    for(i=3;i*i<=n;i+=2)
	if (n%i==0)
            return 0;
    return 1;
}

int siguiente_primo(int n){
    if (n%2==0)
	n++;
    for (;!esprimo(n);n+=2);
    return n;
}


void rehash(){
    entrada *vieja;
    int i;
    vieja=tabla;
    tamTabla=siguiente_primo(2*tamTabla);
    initTabla();
    for (i=0;i<tamTabla/2;i++){
	if(strcmp(vieja[i].compLex,"-1")!=0)
            insertar(vieja[i]);
	}		
    free(vieja);
}


void insertar(entrada e){
    int pos;
    if (++elems>=tamTabla/2)
	rehash();
    pos=h(e.lexema,tamTabla);
    while (strcmp(tabla[pos].compLex,"-1")!=0){ 
	pos++;
	if (pos==tamTabla)
            pos=0;
    }
    tabla[pos]=e;

}


entrada* buscar(const char *clave){
    int pos;
    pos=h(clave,tamTabla);
    while(strcmp(tabla[pos].compLex,"-1")!=0 && strcmp(tabla[pos].lexema,clave)!=0 ){ 
	if (pos==tamTabla)
            pos=0;
    }
    return &tabla[pos];
}

void insertTablaSimbolos(const char *s, char *n){
    entrada e;
    strcpy(e.lexema,s);
    strcpy(e.compLex,n);
    insertar(e);
}

void initTablaSimbolos(){
    insertTablaSimbolos(",","COMA");
    insertTablaSimbolos(":","DOS_PUNTOS");
    insertTablaSimbolos("[","L_CORCHETE");
    insertTablaSimbolos("]","R_CORCHETE");
    insertTablaSimbolos("{","L_LLAVE");
    insertTablaSimbolos("}","R_LLAVE");
    insertTablaSimbolos("true","PR_TRUE");
    insertTablaSimbolos("false","PR_FALSE");
    insertTablaSimbolos("null","PR_NULL");
    insertTablaSimbolos("\0","EOF");
}
