//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P5/P6 - Preempção de Tempo e Contabilização.
// by Fernando Claudecir Erd - GRR20152936
//------------------------------------------------------------------------------

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include "queue.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

#define STACKSIZE 32768
#define quantumValue 20
#define READY 0
#define SUSPEND 1
#define FINISH 2
// estrutura que define um tratador de sinal (deve ser global ou static)
struct sigaction action ;

// estrutura de inicialização to timer
struct itimerval timer;

// Estrutura que define uma tarefa
typedef struct task_t {
	struct task_t *prev, *next;   // para usar com a biblioteca de filas (cast)
	int tid;           // ID da tarefa
	ucontext_t context;
	int staticPriority;
	int dynamicPriority;
	int quantum;
	unsigned int executionTime;
	unsigned int processorTime;
	int activations;
	struct task_t *queueSuspend;
	int exitcode;
	int state;
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
