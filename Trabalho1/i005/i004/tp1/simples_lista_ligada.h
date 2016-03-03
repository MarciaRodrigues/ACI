typedef struct _elemento
{
	int valor;
    struct _elemento *proximo;
} elemento;

typedef struct
{
	elemento *raiz;
} lista;

lista* lista_nova();

void lista_apaga(lista *lst);

int lista_insere_inicio(lista *lst, int valor);

int lista_pesquisa(lista *lst, int valor);

int lista_remove(lista *lst, int valor);

int lista_imprime(lista *lst);



