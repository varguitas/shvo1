/* CODIGO FUENTE: servidor.c */
/*
 * El siguiente codigo consiste en el codigo utilizado por el lado del servidor.
 * Este es quien recibe el mensaje, y genera un respuesta.
 * 
 ******************************************************************************/
 /****************Includes necesarios para el programa.***********************/
#include <stdio.h>			// Contiene declaraciones utilizadas para entrada y salida de datos
#include <unistd.h>			// 
#include <errno.h>
#include <stdlib.h>			// Libreria estandar de c (es utilizada por ej: gethostbyname)
#include <string.h>			// Contiene definiciones utilizadas para la manipulacion de cadenas de caracteres
#include <sys/socket.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <sys/types.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <netinet/in.h>		// Contiene definiciones de constantes y estructuras que son necesarias para el uso de direcciones de dominion en internet
 /*--------------------------------------------------------------------------*/

/*	Funcion: error(const char *mensajeError)
 * 	Recibe como parametro un puntero a una secuencia de caracteres.
 * 	Esta funcion consiste en enviar al standart output un mensaje de error y salir del programa.
 * */
void error(const char *mensajeError)
{
    perror(mensajeError);
    exit(0);
}

int main(int argc, char *argv[])
{
     if (argc < 2) {
         fprintf(stderr,"ERROR, No se proporciono ningun puerto.\n");
         exit(1);
     }
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR al abrir el socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR al realizar el bind");
     while(1){
		 listen(sockfd,5);
		 clilen = sizeof(cli_addr);
		 newsockfd = accept(sockfd, 
					 (struct sockaddr *) &cli_addr, 
					 &clilen);
		 if (newsockfd < 0) 
			  error("ERROR al aceptar cliente");
		 bzero(buffer,256);
		 //----------------Se guardara archivo recibido en una direccion fija------------//
		char* fr_name = "/home/vargas/Escritorio/receive.txt";
		FILE *fr = fopen(fr_name, "a");
		if(fr == NULL)
			printf("El archivo %s no puede ser accedido por el servidor.\n", fr_name);
		else
		{
			bzero(buffer, 256); 
			int fr_block_sz = 0;
			while((fr_block_sz = recv(newsockfd, buffer, 255, 0)) > 0) 
			{
			    int write_sz = fwrite(buffer, sizeof(char), fr_block_sz, fr);
				if(write_sz < fr_block_sz)
			    {
			        error("Se ha generado un error al escribir en el cliente.\n");
			    }
				bzero(buffer, 256); 
				if (fr_block_sz == 0 || fr_block_sz != 512) 
				{
					break;
				}
			}
			if(fr_block_sz < 0)
		    {
		        if (errno == EAGAIN)
	        	{
	                printf("El envio a caducado.\n");
	            }
	            else
	            {
	                fprintf(stderr, "Fallo la lectura. errno = %d\n", errno);
					exit(1);
	            }
        	}
			printf("Documento recibido con exito!\n");
			fclose(fr); 
		}
	 }
     close(newsockfd);
     close(sockfd);
     return 0; 
}
