#include <stdlib.h>
#include <stdio.h>
#include "simples_lista_ligada.h"


lista* lista_nova()
{

	lista *lst = (lista*) malloc(sizeof(lista));
	if(lst == NULL)
		return NULL;
	lst->raiz = NULL;

    return lst; 
}

void lista_apaga(lista *lst)
{
	elemento *aux;
	
	if(lst == NULL)
		return;

	while(lst->raiz)
	{
		aux = lst->raiz;
		lst->raiz = lst->raiz->proximo;
		free(aux);
	}
	
	free(lst);

	return;
}

int lista_insere_inicio(lista *lst, int valor)
{
	elemento *inserir=NULL, *aux;
	if (lst == NULL)
		return -1;
	aux= lst->raiz;
	inserir = (elemento*) malloc(sizeof(elemento)) ;
	if (inserir==NULL)
		return -1;
	inserir->valor=valor;
	inserir->proximo=aux;
	lst->raiz=inserir;
	return 0;
}

int lista_pesquisa(lista *lst, int valor)
{
	int i=0, pos=0;
	elemento *aux;
	if (lst == NULL)
		return -1;
	aux = lst->raiz;
	while(1)
	{
		if (aux==NULL)
			return -1;
		if (aux->valor == valor)
			return pos;
		aux = aux->proximo;
		pos++;
	}

}


int lista_remove(lista *lst, int valor)
{
	int pos=-1, i=0;
	elemento *curr, *prev;
	pos=lista_pesquisa(lst, valor);
	if (pos==-1)
		return -1;
	curr=lst->raiz;
	if(pos == 0)
	{
		lst->raiz = curr->proximo;
		free(curr);
		return 0;
	}
	for(i = 0; i < pos && curr->proximo; i++)
	{
		prev = curr;
		curr = curr->proximo;
	}
	prev->proximo = curr->proximo;
	free(curr);
	return 0;
}

int lista_imprime(lista *lst)
{
	int i = 0;
	elemento *aux;
	aux=lst->raiz;
	while(aux!=NULL)
	{
		printf("Posicao %d: valor=%d\r\n", i, aux->valor);
		i++;
		aux=aux->proximo;
	}
	return 0;
	
	
}