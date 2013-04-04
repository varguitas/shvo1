/* José Pablo Matamoros / Victor Vargas 
 * Programa transmisor de archivos mediante TCP/IP
 * Lenguajes de Programación / I semestre 2013
 * Instituto Tecnológico de Costa Rica*/
 
 
//Librerias Utilizadas
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//Inicio del programa (MAIN principal)
int main(int cantArgs,char *args[])
{
	/* Identificador del proceso creado */
	pid_t idProceso;

	/* Variable para comprobar que se copia inicialmente en cada proceso y que
	 * luego puede cambiarse independientemente en cada uno de ellos. */
	int variable = 10;

	/* Estado devuelto por el hijo */
	int estadoHijo;

	/* Se crea el proceso hijo. En algún sitio dentro del fork(), nuestro
	 * programa se duplica en dos procesos. Cada proceso obtendrá una salida
	 * distinta. */
	idProceso = fork();

	/* Si fork() devuelve -1, es que hay un error y no se ha podido crear el
	 * proceso hijo. */
	if (idProceso == -1)
	{
		perror ("No se puede crear proceso");
		exit (-1);
	}

	/* fork() devuelve 0 al proceso hijo.*/
	if (idProceso == 0)
	{
		sleep(1);
		printf("Soy el hijo. Mi variable vale %d \n",variable);
		while(variable!=0){
			printf("Hijo >>> Le resto 1 a variable, ahora vale %d.\n", variable);
			variable--;
			sleep(2);
		}
		printf("Finaliza hijo.\n");
		exit(33);
	}

	/* fork() devuelve un número positivo al padre. Este número es el id del
	 * hijo. */
	if (idProceso > 0)
	{
		variable+=10;
		printf("Padre >>> Mi variable vale %d.\n", variable);
		while(variable!=0){
			printf("Padre >>> Le resto 2 a variable, ahora vale %d.\n", variable);
			variable=variable-2;
			sleep(2);
		}
		printf("Finaliza Padre.\n");
		wait (&estadoHijo);

		/* Comprueba la salida del hijo */
		if (WIFEXITED(estadoHijo) != 0)
		{
			printf ("Padre : Mi hijo ha salido. Devuelve %d\n", 
				WEXITSTATUS(estadoHijo));
		}
	}
	return 0;
}
