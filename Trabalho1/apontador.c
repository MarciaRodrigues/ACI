#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


main()  {
int a=33;
int *ap=&a; // &a, devolve o endereço de memória de uma variável
printf("Valor a: %d\n", a);
printf("Endereço a: %d\n", &a);
printf("Valor apontador: %d\n", ap);
printf("Endereço apontador: %d\n", &ap);
printf("Valor na memória apontada pelo apontador: %d\n", *ap);
}