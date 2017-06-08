//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P0 - Implementação de uma fila.
// by Fernando Claudecir Erd - GRR20152936
//------------------------------------------------------------------------------

#include "queue.h"
#include <stdio.h>


//------------------------------------------------------------------------------
// Verifica se o elemento é nulo
// Verifica se o elemento esta em outra fila
// Se a fila estiver vazia, cria uma nova
// Se não, adiciona no final da fila

void queue_append (queue_t **queue, queue_t *elem) {

    queue_t *aux;
    
    if (elem == NULL)
        printf ("Erro, impossivel inserir. O elemento não existe\n");

    else if (elem->next != NULL || elem->prev != NULL)
        printf ("Erro, impossivel inserir. O elemento pertence a outra fila\n");
    
    else if (*queue == NULL) {
        aux = elem;
        aux->prev = elem;
        aux->next = elem;
        (*queue) = aux;
    } 
    
    else {
        aux = (*queue)->prev;
        elem->prev = aux;;
        elem->next = (*queue);
        aux->next = elem;
        (*queue)->prev = elem;
    }

}

//------------------------------------------------------------------------------
// Verifica se a fila é vazia
// Verifica se o elemento é nulo
// 3 casos de remoção
//   - Primeiro e unico da fila
//   - Primeiro da fila
//   - Outro elemento da fila

queue_t *queue_remove (queue_t **queue, queue_t *elem) {
    
    queue_t *aux;
    
    if ((*queue) == NULL) {
        printf ("Erro, impossivel remover. A fila encontra-se vazia\n");
    }
    else if (elem == NULL) 
        printf ("Erro, impossivel remover. O elemento nao existe\n");

    //Caso seja primeiro e unico elemento da fila
    else if (elem == (*queue) && queue_size(*queue) == 1) {
            (*queue) = NULL;
            elem->next = NULL;
            elem->prev = NULL;
            return elem;
    }
    
    //Caso seja primeiro elemento da fila
    else if (elem == (*queue)){
            aux = (*queue);
            (*queue) = (*queue)->next;
            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
            aux->next = NULL;
            aux->prev = NULL;
            return aux;;
    }

    //Caso seja outro elemento da fila;
    else {
        
        for (aux = (*queue)->next; aux->next != (*queue)->next; aux=aux->next)
            
                if (aux == elem) {
                aux->prev->next = aux->next;
                aux->next->prev = aux->prev;
                aux->next = NULL;
                aux->prev = NULL;
                return aux;
            }
    }
    return NULL;
}

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila

int queue_size (queue_t *queue) {
 
    queue_t *aux; int cont;

    cont = 1;

    if (queue == NULL)
        return 0;
 
    for (aux = queue; aux->next != queue; aux=aux->next, cont++);
    
    return cont; 
}
            
//------------------------------------------------------------------------------
// Verifica se a fila esta vazia
// Imprime os elementos da fila
        
void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {

    queue_t *aux;

    if (queue == NULL)
        printf ("%s: []\n", name);

    else {
        printf ("%s: [", name);
        print_elem (queue);           
        
        for (aux = queue->next; aux != queue; aux = aux->next) {
            printf (" ");
            print_elem (aux);           
        }
    
        printf ("]\n");
    }
}
