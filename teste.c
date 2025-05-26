#include <stdio.h>
#include <stdlib.h>
#define CAP 10

typedef struct{
    int size;
    int data[CAP];
}List;


int inserir(int val, int index, List* list){
    // verifica o se existe espaço
    if(list->size==CAP)
        return 1; // retorna 1 para indicar erro de espaço
    // verifica se o índice é inválido
    if(index < 0 || index > list->size)
        return 2; // retorna 2 para indicar erro de índice

    int i = list->size; // cria apontador para o final
    while(i!= index){ // repetição que abre espaço
        list->data[i] = list->data[i-1];
        i--; //decrementa o apontador em direção ao índice
    }
    list->data[i] = val; // coloca o valor no índice
    list->size++;
    return 0; // retorna 0 para indicar sucesso
}

int get(int val, List* list){
    // percorrer todas os valores da lista
    for(int i=0; i<list->size; i++){
      if(val == list->data[i]){
        return i; // retorna a posição na qual a informação está contida
      }
    }
    return -1; // simboliza que não existe informação
}

int removeIndex(int index, List* list) {
    if(list->size==0) return 1; // lista vazia
    if(index<0||index>=list->size) return 2; // índice
    int i = index; // cria apontador para index
    while(i<list->size){ // repetição que abre espaço
        list->data[i] = list->data[i-1];
        i++; // incrementa em direção ao fundo
    }
    list->size--;
    return 0; // retorna 0 para indicar sucesso
}

int removeValue(int val, List* list) {
    if(list->size==0) return 1; // lista vazia
    int index = get(val); // busca o índice
    if(index==-1) return 2; // valor inexistente
    int i = index; // cria apontador para index
    while(i<list->size){ // repetição que remove
         list->data[i] = list->data[i-1];
         i++; // incrementa em direção ao fundo
    }
    list->size--;
    return 0; // retorna 0 para indicar sucesso
}