
#include "anlex.h"


token t; 
FILE *archivo; 
FILE *output; 
char msg[41];
char trad[41];
short error_flag=0;
short nivel=0; 

void json(void);
void element(void);
void array(void);
void ar(void);
void element_list(void);
void el(void);
void object(void);
void o(void);
void attribute_list(void);
void attribute(void);
void al(void);
void attribute_name(void);
void attribute_value(void);


char* tab(char*);
void destab(char*);


void error_msg(char* mensaje){
    error_flag=1;
    printf("Linea %d: Error Sintáctico. %s.\n",linea(),mensaje);	
}


char* tab(char* texto){
    char *tabs= malloc(sizeof(texto));
    for(short i=0;i<nivel;i++){
        strcat(tabs,"\t");
    }
    strcat(tabs,texto);
    return tabs;
}

void destab(char* texto){   
    char *pr = texto, *pw = texto;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != '\t');
    }
    *pw = '\0';
}

void getToken(void) {
    sigLex();
}

void match(char* c){
    if (strcmp(t.compLex, c)==0) getToken(); 
    else error_msg("Error en el matching.");
}

void json(){
    element();
    match("EOF");
}

void element(){  
    if(strcmp(t.compLex,"L_LLAVE")==0){
        object();
    }
    else if (strcmp(t.compLex,"L_CORCHETE")==0){
        array();
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Token esperado \"{\" o \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void array(){
    if(strcmp(t.compLex,"L_CORCHETE")==0){
        match("L_CORCHETE");
        ar();  
        //nivel++;
        strcpy(trad,tab(trad));
        nivel--;    
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void ar(){
    if(strcmp(t.compLex,"R_CORCHETE")==0){
        match("R_CORCHETE"); 
    }
    else if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        element_list();
        match("R_CORCHETE");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(msg,"Se esperaba un \"[\" o \"]\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }   
    else getToken();
}

void element_list(){
    if(strcmp(t.compLex,"L_CORCHETE")==0||strcmp(t.compLex,"L_LLAVE")==0){
        element();
        el();    
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0){
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
    
}

void el(){
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        nivel++; 
        element();
        el();
    }
    else if(strcmp(t.compLex,"R_CORCHETE")!=0){       
        getToken(); 
    }
}

void object(){
    if(strcmp(t.compLex,"L_LLAVE")==0){
        match("L_LLAVE");
        o(); 
        nivel--; 
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Token esperado \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();
}

void o(){
    if(strcmp(t.compLex,"R_LLAVE")==0){
        match("R_LLAVE");   
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute_list();
        
        match("R_LLAVE");
    }
    else if(strcmp(t.compLex,"R_CORCHETE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Se esperaba un \"}\" o \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();     
}

void attribute_list(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        attribute();
        al(); 
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0){
        sprintf(msg,"Token esperado \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }   
    else getToken(); 
}

void attribute(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        char* aux=t.pe->lexema;
        aux++;
        aux[strlen(aux)-1] = '\0';
        
        sprintf(trad,"<%s>",aux);
        strcpy(trad,tab(trad)); 
        fputs(trad,output);
        attribute_name();
        match("DOS_PUNTOS");
        attribute_value(); 
        destab(trad); 
        sprintf(trad,"</%s>\n", aux);
        fputs(trad,output); 
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0){
        sprintf(msg,"Token esperado \"string\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken();    
}

void al(){
    if(strcmp(t.compLex,"COMA")==0){
        match("COMA");
        attribute();
        al();
    }
    else if(strcmp(t.compLex,"R_LLAVE")!=0){ 
        getToken(); 
    }       
}

void attribute_name(){
    if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        match("LITERAL_CADENA");
    }
    else getToken();
}

void attribute_value(){
    if(strcmp(t.compLex,"L_LLAVE")==0||strcmp(t.compLex,"L_CORCHETE")==0){
        fputs("\n",output);
        nivel++; 
        element();
    }
    else if(strcmp(t.compLex,"LITERAL_CADENA")==0){
        fputs(t.pe->lexema,output);
        match("LITERAL_CADENA");
    }
    else if(strcmp(t.compLex,"LITERAL_NUM")==0){
        fputs(t.pe->lexema,output);
        match("LITERAL_NUM");
    }
    else if(strcmp(t.compLex,"PR_TRUE")==0){
        fputs(t.pe->lexema,output);
        match("PR_TRUE");
    }
    else if(strcmp(t.compLex,"PR_FALSE")==0){
        fputs(t.pe->lexema,output);
        match("PR_FALSE");
    }
    else if(strcmp(t.compLex,"PR_NULL")==0){
        fputs(t.pe->lexema,output);
        match("PR_NULL");
    }
    else if(strcmp(t.compLex,"R_LLAVE")==0||strcmp(t.compLex,"COMA")==0||strcmp(t.compLex,"DOS_PUNTOS")==0){
        sprintf(msg,"Token esperado \"{\" o \"[\" o \"string\" o \"number\" o \"true\" o \"false\" o \"null\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
    else getToken(); 
}

void inicio(void){
    json(); 
    if(strcmp(t.compLex,"EOF")!=0) error_msg("No se esperaba fin del archivo.");
}

void parser(){
    while (strcmp(t.compLex,"EOF")!=0){
        getToken();
        inicio();
    }   
}

int main(int argc,char* args[]){
    initTabla();
    initTablaSimbolos();
     
    if(argc > 1){
	if (!(archivo=fopen(args[1],"rt"))){
            printf("No se encontro el archivo.\n");
            exit(1);
	}
	while (strcmp(t.compLex,"EOF")!=0){
            output = fopen( "output.txt", "w" );
            parser();
            if(error_flag==0)
                printf("El fuente es sintácticamente correcto.\n");
            fclose(output);
	}
	fclose(archivo);
    }else{
	printf("Debe pasar el archivo fuente\n");
	exit(1);
    }
    return 0;
}
