//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P9 - Task Sleep.
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
#include <string.h>

#define STACKSIZE 32768
#define quantumValue 20
#define READY 0
#define SUSPEND 1
#define FINISH 2
#define SLEEP 3
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
	unsigned int wakeUp;
} task_t;

// estrutura que define um semáforo
typedef struct semaphore_t {
	int counter;
	task_t *queue;
	int state;
} semaphore_t;

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
typedef struct {
    int maxSize;
    int status; 
    int msgActually;
    int bytesSize;
    int head;
    int end;  
    void *buffer;   
    semaphore_t s_item;
    semaphore_t s_buffer;
    semaphore_t s_vaga;
} mqueue_t ;

#endif
