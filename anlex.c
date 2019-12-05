
#include "anlex.h"


int consumir; 

char cad[5*TAMLEX]; 
token t; 

FILE *archivo; 
char buff[2*TAMBUFF]; 
char id[TAMLEX]; 
int delantero=-1; 
int fin=0; 
int numLinea=1; 
char espacio[100];


void error(const char* mensaje){
    printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex(){
    int i=0;
    char c=0;
    int acepto=0;
    int estado=0;
    char msg[41];
    entrada e;
    
    while((c=fgetc(archivo))!=EOF){
		
	if (c==' ' || c=='\t'){
            strcat(espacio,&(c));
            continue; //eliminar espacios en blanco
        }
	else if(c=='\n'){
            //incrementar el numero de linea
            numLinea++;
            //strcat(espacio,&(c));
            continue;
	}
	else if (isalpha(c)){
            //es un boolean
            i=0;
            do{
                id[i]=tolower(c);
                i++;
		c=fgetc(archivo); // Avanza al siguiente caracter
		if (i>=TAMLEX)
                    error("Longitud de Identificador excede tamaño de buffer");
            }while(isalpha(c));
            id[i]='\0';
            if (c!=EOF)
		ungetc(c,archivo); // Vuelve al caracter anterior
            else
                c=0;
            
            t.pe=buscar(id); 
            strcpy(t.compLex,t.pe->compLex);
            if (strcmp(t.pe->compLex,"-1")==0){ 
                strcpy(e.lexema,id); 
                strcpy(e.compLex,"LITERAL_CADENA");
		insertar(e);
		t.pe=buscar(id);
                strcpy(t.compLex,"LITERAL_CADENA");
            }
            break;
	}
	else if (isdigit(c)){
    
            i=0;
            estado=0;
            acepto=0;
            id[i]=c;
            while(!acepto){
                switch(estado){
                    case 0: 
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            estado=0;
                        }
                        else if(c=='.'){
                            id[++i]=c;
                            estado=1;
                        }
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            estado=3;
                        }
                        else{
                            estado=6;
                        }
                        break;
                    case 1:
                        c=fgetc(archivo);						
                        if (isdigit(c)){
                            id[++i]=c;
                            estado=2;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 2:
                        c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=2;
			}
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            estado=3;
			}
			else
                            estado=6;
			break;
                    case 3:
                        c=fgetc(archivo);
                        if (c=='+' || c=='-'){
                            id[++i]=c;
                            estado=4;
			}
			else if(isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 4://necesariamente debe venir por lo menos un digito
			c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
			}
			break;
                    case 5://una secuencia de digitos correspondiente al exponente
			c=fgetc(archivo);
			if (isdigit(c)){
                            id[++i]=c;
                            estado=5;
			}
			else{
                            estado=6;
			}break;
                    case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
			if (c!=EOF)
                            ungetc(c,archivo);
			else
                            c=0;
			id[++i]='\0';
			acepto=1;
			t.pe=buscar(id);
			if (strcmp(t.pe->compLex,"-1")==0){ // Si no encuentra el número lo agrega a la TS.
                            strcpy(e.lexema,id);
                            strcpy(e.compLex,"LITERAL_NUM");
                            insertar(e);
                            t.pe=buscar(id);
			}
                        strcpy(t.compLex,"LITERAL_NUM");
			break;
                    case -1:
			if (c==EOF)
                            error("No se esperaba el fin de archivo");
			else
                            error(msg);
                        exit(1);
                }
            }
            break;
	}
	else if (c==':'){
            strcpy(t.compLex,"DOS_PUNTOS");
            t.pe=buscar(":");
            break;
	}

	else if (c==','){
            strcpy(t.compLex,"COMA");
            t.pe=buscar(",");
            break;
	}
	else if (c=='['){
            strcpy(t.compLex,"L_CORCHETE");
            t.pe=buscar("[");
            break;
	}
	else if (c==']'){
            strcpy(t.compLex,"R_CORCHETE");
            t.pe=buscar("]");
            break;
	}
        
        else if (c=='{'){
            strcpy(t.compLex,"L_LLAVE");
            t.pe=buscar("{");
            break;
	}
	else if (c=='}'){
            strcpy(t.compLex,"R_LLAVE");
            t.pe=buscar("}");
            break;
	}
	else if (c=='\"'){
            i=0;
            id[i]=c;
            i++;
            do{
                c=fgetc(archivo);
		if (c=='\"'){
                    c=fgetc(archivo);
                    if (c=='\"'){
                        id[i]=c;
                        i++;
                        id[i]=c;
                        i++;
                    }
                    else{
                        id[i]='\"';
                        i++;
                        break;
                    }
		}
		else if(c==EOF){
                    error("Se llego al fin de archivo sin finalizar un literal");
		}
		else{
                    id[i]=c;
                    i++;
		}
            }while(isascii(c));
            id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            else
		c=0;
            t.pe=buscar(id);
            strcpy(t.compLex,t.pe->compLex);
            if (strcmp(t.pe->compLex,"-1")==0){
		strcpy(e.lexema,id);
                strcpy(e.compLex,"LITERAL_CADENA");
                insertar(e);
		t.pe=buscar(id);
                strcpy(t.compLex,e.compLex);
            }
            break;
	}
        else if (c!=EOF){
            sprintf(msg,"%c no esperado",c);
            error(msg);
        }
    }
    if (c==EOF){
        strcpy(t.compLex,"EOF");
        sprintf(e.lexema,"eof");
        t.pe=&e;
    }    
}

int linea(){
    return numLinea;
}

