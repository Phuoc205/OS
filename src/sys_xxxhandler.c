// From src/sys_xxxhandler.c
#include "common.h"
#include "syscall.h"
#include "stdio.h"

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
    result[i] = '\0';
}

int __sys_xxxhandler(struct pcb_t * caller, struct sc_regs * regs)
{
    char name[100];
    get_name(caller, name);
    printf("%s\n", name);
    /* TODO: implement syscall job */
    printf("The first system call parameter %d\n", regs->a1);
    return 0;
}