#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char *argv[]){
//Variables
int i;		
int size;	
char Nombre[50];
char ID[5];	
char *nombreArchivo = "Datalog.txt";
char *modo = "w";
char respuesta[2] = "Y";
FILE *archivo = fopen(nombreArchivo,modo); 
int ayuda;
if(archivo == NULL){ 
	printf("Error abriendo el archivo %s\n",archivo);
	return 1;
}else if(archivo != NULL){
	//Read command inputs
	for(i = 0; i < argc; i++){
		printf("argv[%d] = %s\n",i,argv[i]);
	}
	if(strcmp(argv[1],"-s")==0){
		
		size = atoi(argv[2]);		 	  
		
			for(int x = 0; x<size; x++){		 
				if(strcmp(respuesta,"Y")==0){	 
			
					printf("ENTER NAME: \n");	 
					scanf("%s",&Nombre);		 
						
					printf("ENTER ID: \n");	 	 
					scanf("%s",&ID);		 	 
					fprintf(archivo,"NAME: %s   ID: %s\n",&Nombre,&ID);
					if(x<size-1){
						printf("Do you wish to add more? [Y/n]\n");
						scanf("%s",&respuesta);
						
						if(strcmp(respuesta,"Y")!=0){	
							//Guardamos y cerramos
							printf("Students information stored in Datalog.txt\n");	
							fclose(archivo);					
							return 0;	
						}
					}
				}
				
			}//for		
							printf("Students information stored in Datalog.txt\n");	
							fclose(archivo);				
							return 0;	
	}
		else{
			printf("Falta especificar la dimension -s\n");
			return 0;	
		}
	}
