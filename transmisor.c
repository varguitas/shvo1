/* José Pablo Matamoros / Victor Vargas 
 * Programa transmisor de archivos mediante TCP/IP
 * Lenguajes de Programación / I semestre 2013
 * Instituto Tecnológico de Costa Rica*/
 
/* DOCUMENTACION INTERNA: ESTRUCTURAS UTILIZADAS
 * 
 * La estructura sockaddr_in posee informacion del socket como:
 *  struct sockaddr_in{ 
 *	   short sin_family; 			Familia de direccion
 *	   unsigned short sin_port; 	Puerto IP
 *	   struct in_addr sin_addr; 	Direccion IP
 *	   char sin_zero[8]; 			Para hacer la estructura del mismo tamanio que el socket.
 *	};
 * 
 * La estructura hostent es la siguiente:
 *  struct hostent {
 *	   char    *h_name;         nombre del host 
 *	   char    **h_aliases;     Alias del host
 *	   int     h_addrtype;      Tipo de direccion del host
 *	   int     h_length;        largo de la direccion
 *	   char    **h_addr_list;   lista de direcciones.
 *	}
 * 
 ******************************************************************************/
 
//Librerias Utilizadas
 /****************Includes necesarios para el programa.***********************/
#include <errno.h>			// Para identificar los tipos de errores generados.
#include <stdio.h>			// Contiene declaraciones utilizadas para entrada y salida de datos
#include <unistd.h>			// 
#include <sys/wait.h>
#include <stdlib.h>			// Libreria estandar de c (es utilizada por ej: gethostbyname)
#include <string.h>			// Contiene definiciones utilizadas para la manipulacion de cadenas de caracteres
#include <sys/socket.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <sys/types.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <netdb.h>			// (Network Device) Contiene definiciones de 
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

//Inicio del programa (MAIN principal)
int main(int cantArgs,char *args[])
{
	//Si la cantidad de argumentos es diferente que 4 (1-nombre del programa, 2-dir IP remota, 3-puerto remoto, 4-puerto local) entonces indique como utilizar programa y salga del mismo.
    if (cantArgs != 4) {
       fprintf(stderr,"ERROR DE USO: Uso del programa: %s ipRemota puertoRemoto puertoLocal\n", args[0]);
       exit(0);
    }
    // Se inicializan variables necesarias para el socket
    int socketRemoto,puertoRemoto,puertoLocal;
    //Se analizan los argumentos
    puertoRemoto=atoi(args[2]);
    puertoLocal=atoi(args[3]);
    
    // Se obtiene id de proceso
	pid_t idProceso;  //ID del proceso creado
	idProceso = fork();  //Hacemos un bifurcación al proceso
	if (idProceso == -1) //Si se devuelve -1 hubo un error al crear el proceso
	{
		perror ("No se puede crear proceso"); //Despliega mensaje de error
		exit (-1); //Salida forzada
	}
	
	/* Proceso hijo creado por el fork() */
	if (idProceso == 0)
	
	{
		//---------------------------------Código del proceso hijo------------------------------------------------
		//La siguiente linea consiste en crear una estructura de tipo sockaddr_in la cual es utilizada para especificar el punto de acceso
		//local o remoto al cual se desea conectar el socket. 
		struct sockaddr_in serv_addr;
		// la siguiente linea crea un puntero llamado server que es de tipo hostent el cual almacena informacion del servidor, como 
		//	se observa en la siguiente estructura:
		struct hostent *server;
		
		
		while(1){
			char nombre[100];
			char *fs_name=nombre;
			printf("\nDigite la direccion del archivo: \n");
			printf(">>> ");
			scanf("%s",nombre);
			
			if(strcmp (nombre,"salir")==0){
				printf("\n Finalizando programa.");
				break;
			}
			
		/*
		 * La siguiente linea consiste en la creacion del socket. Para esto se utiliza la funcion socket la cual recibe 3 parametros.
		 * 	1) El tipo de socket a crear (IPv4,IPv6...)
		 *  2) El tipo de conexion, en este caso SOCK_STREAM, que utiliza TPC para enviar y recibir datos.
		 *  3) El protocolo de trasnferencia. En este caso es 0 (cero), lo cual significa que el socket utilizara un protocolo sin especificar
		 * 
		 * */
			socketRemoto = socket(AF_INET, SOCK_STREAM, 0);
			/*---------------------------------------*/
			if (socketRemoto < 0) {
				error("ERROR: No se logro abrir el socket");
			}
			//La funcion gethostbyname retorna un puntero a una estructura de tipo hostent, explicada anteriormente.
			//Esta toma como argumento la direccion dada por el usuario y una vez que la procesa, devuelve la estructura.
			server = gethostbyname(args[1]);
			// En caso de que la funcion gethostbyname presente un error, retornara NULL, en este caso se le informara al usuario del error y se
			// saldra del programa.
			if (server == NULL) {
				fprintf(stderr,"ERROR: No se ha podido encontrar el servidor al que intenta conectarse.\n");
				exit(0);
			}
			
			// Se utiliza la funcion bzero que convierte los primero n bytes de una cadena de caracteres en cero. 
			bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;		// Se cambia la entrada sin_family de serv_addr
			bcopy((char *)server->h_addr, 		// Se copian los primeros n cantidad de bytes de un 'string' a otro.
				 (char *)&serv_addr.sin_addr.s_addr,
				 server->h_length);
			serv_addr.sin_port = htons(puertoRemoto);	//esta funcion convierte el tipo de dato de serv_addr (int)(short) a un orden de bytes de red.
			if (connect(socketRemoto,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
				error("ERROR: Conexion fallida.");
			}
			//*---------------------------------------------------------------
			char buffer[256];
			FILE *fp;
			fp = fopen(fs_name, "rb");
			int n;
			while(!feof(fp))
			{
			   bzero(buffer,256);
			   n = fread(buffer,sizeof(char),255,fp);
			   send(socketRemoto,buffer,n,0);

			   if(n < 0)
				  error("ERROR writing to socket");
			}
			send(socketRemoto,"done",255,0);
			fclose(fp);
		}
		close(socketRemoto);
		return 0;
	}

	/* El padre tiene asignado un número positivo el cual es el ID del proceso padre */
	if (idProceso > 0)
	{
		/*-----------------------------------------CODIGO DEL PROCESO PADRE (SERVER)--------------------------*/
		 // Se inicializan variables necesarias.
		 int sockfd, newsockfd, portno;
		 socklen_t clilen;
		 // Se crean estructuras de sockets. (Explicadas en proceso hijo(cliente) )
		 struct sockaddr_in serv_addr, cli_addr;
		 sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd < 0) 
			error("ERROR al abrir el socket");
		 bzero((char *) &serv_addr, sizeof(serv_addr));
		 portno = puertoLocal;
		 serv_addr.sin_family = AF_INET;
		 serv_addr.sin_addr.s_addr = INADDR_ANY;
		 serv_addr.sin_port = htons(portno);
		 if (bind(sockfd, (struct sockaddr *) &serv_addr,
				  sizeof(serv_addr)) < 0) 
				  error("ERROR al realizar el bind");
		 while(1){
			 // El while se ejecuta siempre y cuando el usuario no escriba salir en la entrada de datos.
			 listen(sockfd,5);
			 clilen = sizeof(cli_addr);
			 newsockfd = accept(sockfd, 
						 (struct sockaddr *) &cli_addr, 
						 &clilen);
			 if (newsockfd < 0) 
				  error("ERROR al aceptar cliente");
			 char buffer[256];
			 bzero(buffer,256);
			 //----------------Se guardara archivo recibido en una direccion fija------------//
			char* fr_name = "/home/josepablo/Escritorio/recibe.txt";
			FILE *fp;
			fp = fopen(fr_name, "wb");
			int n;
			while(1)
			{
				n = recv(newsockfd,buffer,255,0);
				if(strcmp (buffer,"done" ) == 0){
					printf("\n<<<Tranferencia de archivo completa>>>");
					printf("Digite la direccion de archivo: ");
					printf(">>> \n");
					break;
				}
				fwrite(buffer,1,n,fp);
			}

			fclose(fp);
		 }
		 close(newsockfd);
		 close(sockfd);

	}
	return 0;
}
