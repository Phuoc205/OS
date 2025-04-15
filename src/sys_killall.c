/*
* Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
*/

/* Sierra release
* Source Code License Grant: The authors hereby grant to Licensee
* personal permission to use and modify the Licensed Source Code
* for the sole purpose of studying while attending the course CO2018.
*/

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include "queue.h"


void get_name(struct pcb_t * caller, char * result)
{
    //init
    char buffer[33];
    sprintf(buffer, "%d", caller->pid);

    // ex:
    // process id = 1
    // result = "P1"
    result[0] = 'P';
    int i = 0;
    while (buffer[i]!='\0')
    {
        result[i+1] = buffer[i];
        i++;
    }
    result[i+1] = '\0';
}

int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    // init
    // char current_name[100];
    // get_name(caller, current_name);
    // printf("%s\n", current_name);
    char to_kill_proc_name[100];
    uint32_t data;
    uint32_t memrg = regs->a1;
    
    // Get to kill process name, read from memory region
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        to_kill_proc_name[i]= data;
        if(data == -1) to_kill_proc_name[i]='\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, to_kill_proc_name);

    // traverse the running list and terminate matching
    struct queue_t *run_q = caller->running_list;
    // printf("run_q size = %d", run_q->size);
    for (int j = 0; j < run_q->size; j++) 
    {
        char name[100];
        get_name(run_q->proc[j], name);
        // printf("run_q name = %s\n", name);
        if (strcmp(name, to_kill_proc_name) == 0) // matching logic
        { 
            free(run_q->proc[j]);
            for (int k = j; k < run_q->size - 1; k++) 
            {
                run_q->proc[k] = run_q->proc[k + 1];
            }
            run_q->size--;
            j--;
        }
    }

#ifdef MLQ_SCHED
    struct queue_t * mlq_ready_queue = caller->mlq_ready_queue;
    for (int prio = 0; prio < MAX_PRIO; prio++) 
    {
        struct queue_t * queue = &mlq_ready_queue[prio];
        for (int j = 0; j < queue->size; j++) 
        {
            char name[100];
            get_name(queue->proc[j], name);
            // printf("queue name = %s\n", name);
            if (strcmp(name, to_kill_proc_name) == 0) {
                free(queue->proc[j]);
                for (int k = j; k < queue->size - 1; k++) {
                    queue->proc[k] = queue->proc[k + 1];
                }
                queue->size--;
                j--;
            }
        }
    }
#endif  
#ifndef MLQ_SCHED
    struct queue_t * ready_queue = caller->ready_queue;
    for (int j = 0; j < ready_queue->size; j++) 
    {
        char name[100];
        get_name(ready_queue->proc[j], name);
        // printf("ready_queue name = %s\n", name);
        if (strcmp(name, to_kill_proc_name) == 0) { // matching
            free(ready_queue->proc[j]);
            for (int k = j; k < ready_queue->size - 1; k++) {
                ready_queue->proc[k] = ready_queue->proc[k + 1];
            }
            ready_queue->size--;
            j--;
        }
    }
#endif

    return 0; 
}
