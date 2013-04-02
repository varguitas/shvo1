/* CODIGO FUENTE: cliente.c */
/*
 * El siguiente codigo consiste en el codigo utilizado por el lado del cliente.
 * Este es quien envia el mensaje.
 * 
 ******************************************************************************/
 /****************Includes necesarios para el programa.***********************/
#include <errno.h>
#include <stdio.h>			// Contiene declaraciones utilizadas para entrada y salida de datos
#include <unistd.h>			// 
#include <stdlib.h>			// Libreria estandar de c (es utilizada por ej: gethostbyname)
#include <string.h>			// Contiene definiciones utilizadas para la manipulacion de cadenas de caracteres
#include <sys/socket.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <sys/types.h>		// Contiene definiciones de estructuras que son utilizadas por la funcion socket
#include <netdb.h>			// (Network Device) Contiene definiciones de 
#include <netinet/in.h>		// Contiene definiciones de constantes y estructuras que son necesarias para el uso de direcciones de dominion en internet
 /*--------------------------------------------------------------------------*/
#define LEN 256
/********************FUNCIONES**************************/

/*	Funcion: error(const char *mensajeError)
 * 	Recibe como parametro un puntero a una secuencia de caracteres.
 * 	Esta funcion consiste en enviar al standart output un mensaje de error y salir del programa.
 * */
void error(const char *mensajeError)
{
    perror(mensajeError);
    exit(0);
}
// Los argumentos de la funcion main son: [cantidadArgumentos, .nombrePrograma, direccionIP,numeroPuerto]
int main(int cantidadArgumentos, char *argumentos[])
{
    int familiaDirPuerto, numPuerto;
    //La siguiente linea consiste en crear una estructura de tipo sockaddr_in la cual es utilizada para especificar el punto de acceso
    //local o remoto al cual se desea conectar el socket. La estructura posee informacion del socket como:
    /*
     *  struct sockaddr_in{ 
	 *	   short sin_family; 			Familia de direccion
	 *	   unsigned short sin_port; 	Puerto IP
	 *	   struct in_addr sin_addr; 	Direccion IP
	 *	   char sin_zero[8]; 			Para hacer la estructura del mismo tamanio que el socket.
	 *	};
     * 
     * */
    struct sockaddr_in serv_addr;
    // la siguiente linea crea un puntero llamado server que es de tipo hostent el cual almacena informacion del servidor, como 
    /*	se observa en la siguiente estructura:
    * 
    * struct hostent {
	*   char    *h_name;         nombre del host 
    *   char    **h_aliases;     Alias del host
    *   int     h_addrtype;      Tipo de direccion del host
    *   int     h_length;        largo de la direccion
    *   char    **h_addr_list;   lista de direcciones.
	*	}
    * 
    * */
    struct hostent *server;

    //char buffer[256];
    //Si la cantidad de argumentos es diferente que 3 (1-nombre del programa, 2-dir IP server, 3-puerto server) entonces indique como utilizar programa y salga del mismo.
    if (cantidadArgumentos != 3) {
       fprintf(stderr,"Uso del programa: %s dirServer NumPuerto\n", argumentos[0]);
       exit(0);
    }
    numPuerto = atoi(argumentos[2]);					// Se convierte argumento correspondiente a puerto a numero entero.

     while(1){
		char nombre[50];
		char *fs_name=nombre;
		printf("Digite la direccion del archivo: (Ej: /home/usuario/Escritorio.txt)\n");
		printf(">>> ");
		scanf("%s",fs_name);
    /*
     * La siguiente linea consiste en la creacion del socket. Para esto se utiliza la funcion socket la cual recibe 3 parametros.
     * 	1) El tipo de socket a crear (IPv4,IPv6...)
     *  2) El tipo de conexion, en este caso SOCK_STREAM, que utiliza TPC para enviar y recibir datos.
     *  3) El protocolo de trasnferencia. En este caso es 0 (cero), lo cual significa que el socket utilizara un protocolo sin especificar
     * 
     * */
		familiaDirPuerto = socket(AF_INET, SOCK_STREAM, 0);
		/*---------------------------------------*/
		if (familiaDirPuerto < 0) {
			error("ERROR: No se logro abrir el socket");
		}
		//La funcion gethostbyname retorna un puntero a una estructura de tipo hostent, explicada anteriormente.
		//Esta toma como argumento la direccion dada por el usuario y una vez procesada, devuelve la estructura.
		server = gethostbyname(argumentos[1]);
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
		serv_addr.sin_port = htons(numPuerto);	//esta funcion convierte el tipo de dato de serv_addr (int)(short) a un orden de bytes de red.
		if (connect(familiaDirPuerto,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
			error("ERROR: Conexion fallida.");
		}
		//*---------------------------------------------------------------
		char sdbuf[LEN]; 
		printf("[Client] Enviando archivo %s al servidor... ", fs_name);
		FILE *fs = fopen(fs_name, "r");
		if(fs == NULL)
		{
			printf("ERROR: El archivo %s no existe o no se encuentra en la ubicacion establecida.\n", fs_name);
			exit(1);
		}

		bzero(sdbuf, LEN); 
		int fs_block_sz; 
		while((fs_block_sz = fread(sdbuf, sizeof(char), LEN, fs)) > 0)
		{
		    if(send(familiaDirPuerto, sdbuf, fs_block_sz, 0) < 0)
		    {
		        fprintf(stderr, "ERROR: Fallo al enviar el archivo %s. (errno = %d)\n", fs_name, errno);
		        break;
		    }
		    bzero(sdbuf, LEN);
		}
		printf("El archivo %s se envio con exito!\n", fs_name);
	}
    close(familiaDirPuerto);	//En caso de que todo marche bien, cierre el puerto.
    return 0;
}
