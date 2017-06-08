	//------------------------------------------------------------------------------
	// Sistemas Operacionais - UFPR - 2016/2.
	// P7/P8 - Tarefa Main e Join
	// by Fernando Claudecir Erd - GRR20152936
	//------------------------------------------------------------------------------

	#include "ppos.h"

task_t taskMain, *taskCurrent, Dispatcher, *queueReady = NULL, *taskNext; 
int id=0;
unsigned int time = 0, initTime =0, initTimeAnt = 0; 

void tratador (int signum) {
	if (taskCurrent != &Dispatcher) {
		
		if (taskCurrent->quantum == 0) {

			#ifdef DEBUG
			printf ("A tarefa %d chegou ao final do qunatum\n", taskCurrent->tid);
			#endif

			task_yield();
			taskCurrent->processorTime += systime() - initTimeAnt;
			taskCurrent->activations++;
		}
		
		else
			taskCurrent->quantum--;
	}
	time++;
}

task_t *scheduler () {
	task_t *taskSmallPriority, *taskAux;

	taskSmallPriority = queueReady;
	taskAux = queueReady;
	
	do {
		
		//Envelhecimento
		taskAux->dynamicPriority--;

		//Procura o de menor prioridade dinamica
		if (taskSmallPriority->dynamicPriority > taskAux->dynamicPriority)
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
	
	#ifdef DEBUG
	printf("O tamanho da fila eh = %d\n",queue_size((queue_t *) queueReady));
	#endif


	while ( (queue_t *) queueReady > 0 ) {
		taskCurrent->activations++; 
		taskNext = scheduler() ;  // scheduler é uma função
	  
		if (taskNext) {
			#ifdef DEBUG
			printf("O tamanho da fila eh = %d\n",queue_size((queue_t *) queueReady));
			#endif
			initTimeAnt = initTime;
			initTime = systime ();
			taskNext->quantum = quantumValue;
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
	taskMain.dynamicPriority = 0;
	taskMain.staticPriority = 0;
	taskMain.executionTime = systime ();
	taskMain.processorTime = 0;
	taskMain.activations = 0;
	taskMain.state = READY;
	taskMain.quantum = quantumValue;

	getcontext (&taskMain.context);
	taskCurrent = &taskMain;
	
	queue_append ((queue_t **) &queueReady, (queue_t *) &taskMain);
	
	task_create (&Dispatcher, (void*) (dispatcher_body),NULL);

	// registra a a��o para o sinal de timer SIGALRM
	action.sa_handler = tratador ;
	sigemptyset (&action.sa_mask) ;
	action.sa_flags = 0 ;
	if (sigaction (SIGALRM, &action, 0) < 0) {
		perror ("Erro em sigaction: ") ;
		exit (1) ;
	}

	// ajusta valores do temporizador
	timer.it_value.tv_usec = 100 ;      // primeiro disparo, em micro-segundos
	timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
	timer.it_interval.tv_usec = 1000 ;   // disparos subsequentes, em micro-segundos
	timer.it_interval.tv_sec  = 0;   // disparos subsequentes, em segundos

	// arma o temporizador ITIMER_REAL (vide man setitimer)
	if (setitimer (ITIMER_REAL, &timer, 0) < 0) {
		perror ("Erro em setitimer: ") ;
		exit (1) ;
	}
	task_yield();

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
		task->tid = ++id;
		task->executionTime = systime ();
		task->processorTime = 0;
		task->activations = 0;
		task->state = READY;
		makecontext (&(task->context), (void*)(*start_func), 1, arg);
		
		#ifdef DEBUG
		if (task->tid != 1)
			printf ("task_create: criou tarefa %d\n", task->tid);
		else
			printf ("task_create: criou dispatcher\n");
		#endif
		
		if (task != &Dispatcher) {
			task->staticPriority = 0;
			task->dynamicPriority = 0;
			task->quantum = quantumValue;
			queue_append ((queue_t **) &queueReady, (queue_t*) task);
		}
	
		return task->tid;
	} 
	
	perror ("Erro na criação da pilha");
	return -1;
	
}    

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {

		if (queue_size ((queue_t *)taskCurrent->queueSuspend) > 0){
			task_t *aux = taskCurrent->queueSuspend, *remove = NULL;
		
			//Caso a fila tenha um único elemento	
			if (queue_size ((queue_t *) taskCurrent->queueSuspend) == 1) {
				remove = (task_t *) queue_remove ( (queue_t **) &taskCurrent->queueSuspend, (queue_t *) aux);
				queue_append ((queue_t **) &queueReady, (queue_t *) remove);
			}
			//Caso a fila tenha mais de um elemento
			else {
				for (aux = taskCurrent->queueSuspend; taskCurrent->queueSuspend != NULL; aux = taskCurrent->queueSuspend->next) {
					remove = (task_t *) queue_remove ( (queue_t **) &taskCurrent->queueSuspend, (queue_t *) aux);
					queue_append ((queue_t **) &queueReady, (queue_t *) remove);
				}
			}

		}
		taskCurrent->exitcode = exitCode; 
		
		taskCurrent->state = FINISH;
		#ifdef DEBUG
		printf ("task_exit: tarefa %d sendo encerrada\n", taskCurrent->tid) ;
		#endif
		
		taskCurrent->executionTime = systime() - taskCurrent->executionTime;

		printf ("Task %d exit: running time %d ms, cpu time %d ms, %d activations\n", taskCurrent->tid, taskCurrent->executionTime, taskCurrent->processorTime, taskCurrent->activations);
		
		//Não existe mais tarefas, ira para a main
		if (taskCurrent == &Dispatcher) {
			//free (Dispatcher.context.uc_stack.ss_sp);
			task_switch (&taskMain);
		}

		//Se a fila de prontos nao for nula, remove e volta o controle para o dispatcher  
	else if (queueReady != NULL)  {
			queue_remove ((queue_t **) &queueReady, (queue_t *) taskCurrent);
			//free (taskCurrent->context.uc_stack.ss_sp);
			task_switch(&Dispatcher);
		 }

	//Se nao, volta para o Dispatcher
	else {
		task_switch(&Dispatcher);
		}
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

	#ifdef DEBUG
	printf ("A tarefa %d esta chamando o Dispatcher\n", taskCurrent->tid);
	#endif

	if (taskCurrent->tid != 1 && taskCurrent->state != SUSPEND) {
		queue_append((queue_t **) &queueReady, (queue_t *) taskCurrent);
	}
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

//retorna o relógio atual (em milisegundos)
unsigned int systime () {
	return time;
}

int task_join (task_t *task) {
	if (task == NULL) return -1;
	
	taskCurrent->state = SUSPEND;
	queue_append ((queue_t **) &task->queueSuspend, (queue_t *) taskCurrent);
	task_switch (&Dispatcher);
	return task->exitcode;
	
}

