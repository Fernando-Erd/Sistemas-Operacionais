//------------------------------------------------------------------------------
// Sistemas Operacionais - UFPR - 2016/2.
// P2 - Gerenciamento de Tarefas.
// by Fernando Claudecir Erd - GRR20152936
//------------------------------------------------------------------------------

#include "ppos.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768

task_t taskMain, *taskCurrent; int id=0; 

void ppos_init () {
    
    setvbuf (stdout, 0, _IONBF, 0);
    taskMain.next = NULL;
    taskMain.prev = NULL;
    taskMain.tid = 0;
    taskCurrent = &taskMain;

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
        printf ("task_create: criou tarefa %d\n", task->tid) ;
        #endif
        return task->tid;
    } 

    else {
	perror ("Erro na criação da pilha");
	return -1;
    }
    
}    

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
        #ifdef DEBUG
        printf ("task_exit: tarefa %d sendo encerrada\n", taskCurrent->tid) ;
        #endif  
	task_switch(&taskMain);
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
