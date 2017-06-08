//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P2 - Gerenciamento de Tarefas.
// by Fernando Claudecir Erd - GRR20152936
//------------------------------------------------------------------------------

// Estruturas de dados internas do sistema operacional

#include <ucontext.h>

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

// Estrutura que define uma tarefa
typedef struct task_t {
   struct task_t *prev, *next;   // para usar com a biblioteca de filas (cast)
   int tid;           // ID da tarefa
   ucontext_t context;
   int staticPriority;
   int dynamicPriority;
} task_t;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando for necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando for necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando for necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando for necessário
} mqueue_t ;

#endif
