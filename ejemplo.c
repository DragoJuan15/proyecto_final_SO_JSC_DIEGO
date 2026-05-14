#include <stdlib.h>
#include <stdio.h>

int main(void){
	int i = 1;
	int *p;

	printf("Antes i vale: %d\n", i);

	p = &i; //p apunta a i
	*p = 2; //se le asigna a donde este apuntando p (i) el valor 2

	printf("Ahora i vale: %d y el contenido de p vale: %d\n", i, *p);
	return 0;
}