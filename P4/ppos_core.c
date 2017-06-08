//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P3/P4 - Dispatcher e Scheduler.
// by Fernando Claudecir Erd - GRR20152936
//------------------------------------------------------------------------------

#include "ppos.h"
#include "queue.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768


//Prioridade Estatica = usuario
//Prioridade Dinamica = fatores internos + usuario

task_t taskMain, *taskCurrent, Dispatcher, *queueReady, *taskNext; 
int id=0; 

task_t *scheduler () {
    task_t *taskSmallPriority, *taskAux;

	taskSmallPriority = queueReady;
	taskAux = queueReady;
	
	do {
        
		//Envelhecimento
        taskAux->dynamicPriority--;

        //Procura o de menor prioridade dinamica
		if (taskSmallPriority->dynamicPriority >= taskAux->dynamicPriority)
			taskSmallPriority = taskAux;
		
		taskAux = taskAux->next;
    
	} while (taskAux != queueReady);

    
	#ifdef DEBUG
    printf ("Scheduler diz que o proximo processo é %d, com prioridade %d\n", taskSmallPriority->tid, taskSmallPriority->dynamicPriority) ;
    #endif
    
	taskSmallPriority->dynamicPriority = taskSmallPriority->staticPriority;
    		
	return taskSmallPriority;
}

void dispatcher_body () {
   
	while ( (queue_t *) queueReady > 0 ) {
      
		taskNext = scheduler() ;  // scheduler é uma função
      
		if (taskNext) {
			queue_remove ((queue_t **) &queueReady, ((queue_t *) taskNext));
			task_switch (taskNext) ; // transfere controle para a tarefa "next"
		}
  
	}
   
	task_exit(0) ; // encerra a tarefa dispatcher
}

void ppos_init () {
    
    setvbuf (stdout, 0, _IONBF, 0);

    taskMain.next = NULL;
    taskMain.prev = NULL;
    taskMain.tid = 0;

    taskCurrent = &taskMain;

    task_create (&Dispatcher, (void*) (dispatcher_body),NULL);

}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task,	void (*start_func)(void *), void *arg) {
    char *stack;

    getcontext(&(task->context));
    stack = malloc (STACKSIZE);

    if (stack) {
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
        makecontext (&(task->context), (void*)(*start_func), 1, arg);
        task->tid = ++id;
		
		#ifdef DEBUG
        if (task->tid != 1)
			printf ("task_create: criou tarefa %d\n", task->tid);
		else
			printf ("task_create: criou dispatcher\n");
        #endif
        
		if (task != &Dispatcher) {
			task->staticPriority = 0;
			task->dynamicPriority = 0;
            queue_append ((queue_t **) &queueReady, (queue_t*) task);
		}
    
        return task->tid;
    } 
    
	perror ("Erro na criação da pilha");
    return -1;
    
}    

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {

        #ifdef DEBUG
        printf ("task_exit: tarefa %d sendo encerrada\n", taskCurrent->tid) ;
        #endif

        //Não existe mais tarefas, ira para a main
        if (taskCurrent == &Dispatcher) {
			free (Dispatcher.context.uc_stack.ss_sp);
            task_switch (&taskMain);
        }

		//Se a fila de prontos nao for nula, remove e volta o controle para o dispatcher  
        else if (queueReady != NULL)  {
			queue_remove ((queue_t **) &queueReady, (queue_t *) taskCurrent);
			free (taskCurrent->context.uc_stack.ss_sp);
			task_switch(&Dispatcher);
		}

		//Se nao, volta para o Dispatcher
		else
			task_switch(&Dispatcher);
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {
        task_t *taskAux;

        taskAux = taskCurrent;
        taskCurrent = task;
        
	    #ifdef DEBUG
        printf ("task_switch: trocando contexto %d -> %d\n", taskAux->tid, task->tid) ;
        #endif 
        
        if (swapcontext (&(taskAux->context), &(task->context)) == -1) {
            printf ("Erro na Troca de Contexto\n");
            return -1;
        }

        else
            return 0;
}
   
// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    return taskCurrent->tid;
}

// libera o processador para a próxima tarefa, retornando à fila de tarefas
// prontas ("ready queue")
void task_yield () {

	if (taskCurrent->tid != 0 && taskCurrent->tid != 1)
		queue_append((queue_t **) &queueReady, (queue_t *) taskCurrent);
		task_switch (&Dispatcher);
}

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {

	if (task != NULL) {
		task->dynamicPriority = prio;
		task->staticPriority = prio;
	}
	
	else {
		taskCurrent->dynamicPriority = prio;
		taskCurrent->staticPriority = prio;
	}
}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {
	
	if (task != NULL)
		return task->staticPriority;
	
	return taskCurrent->staticPriority;
}
