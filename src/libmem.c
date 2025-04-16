/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

// #ifdef MM_PAGING
/*
 * System Library
 * Memory Module Library libmem.c 
 */

#include "string.h"
#include "mm.h"
#include "syscall.h"
#include "libmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

static pthread_mutex_t mmvm_lock = PTHREAD_MUTEX_INITIALIZER;

/*enlist_vm_freerg_list - add new rg to freerg_list
 *@mm: memory region
 *@rg_elmt: new region
 *
 */
int enlist_vm_freerg_list(struct mm_struct *mm, struct vm_rg_struct *rg_elmt)
{
  struct vm_rg_struct *rg_node = mm->mmap->vm_freerg_list;

  if (rg_elmt->rg_start >= rg_elmt->rg_end)
    return -1;

  if (rg_node != NULL)
    rg_elmt->rg_next = rg_node;

  /* Enlist the new region */
  mm->mmap->vm_freerg_list = rg_elmt;

  return 0;
}

void VM_DUMP(struct mm_struct *mm) {
  printf("*************************VM_DUMP***********************\n");
  printf("-------------------------------------------------------\n");
  for(int i=0; i<PAGING_MAX_SYMTBL_SZ; i++) {
    printf("|SYM %d: [%lu, %lu]", i, mm->symrgtbl[i].rg_start, mm->symrgtbl[i].rg_end);
  }
  printf("|\n");
  printf("-------------------------------------------------------\n");
  struct vm_area_struct *vmait = mm->mmap;
  int vmaid = 0;
  while (vmait != NULL) {
    printf("VMA %d (Range: [%lu, %lu]):\n", vmaid, vmait->vm_start, vmait->vm_end);
    
    struct vm_rg_struct *rg_free = vmait->vm_freerg_list;
    while (rg_free != NULL) {
      printf("  Free: [%lu, %lu]", rg_free->rg_start, rg_free->rg_end);
      rg_free = rg_free->rg_next;
      if (rg_free) printf(",");
      printf("\n");
    }
    
    vmaid++;
    vmait = vmait->vm_next;
  }
  printf("*******************************************************\n");

}

void PGD_DUMP(struct pcb_t *caller) {
  int pagenum, fpn;
  uint32_t pte;
  printf("*****************************PGD_DUMP*************************\n");
  for(pagenum = 0; pagenum < PAGING_MAX_PGN; pagenum++)
  {
    pte= caller->mm->pgd[pagenum];
    if(pte!=0) {
      printf("PGD %d: ", pagenum);
      if (PAGING_PAGE_PRESENT(pte))
      {
        fpn = PAGING_PTE_FPN(pte);
        printf("(FPN)%d", fpn);
      } else {
        fpn = PAGING_PTE_SWP(pte);
        printf("(SWAP)%d", fpn);   
      }
      printf("\n");
    }
  }
  printf("**************************************************************\n");
}

<<<<<<< HEAD
void write_int(struct pcb_t* proc, uint32_t source, uint32_t offset, int val) {
  for (int i = 0; i < 4; i++) {
    BYTE data = (val >> (i * 8));
    libwrite(proc, data, source, offset + i);
  }
}

int read_int(struct pcb_t* proc, uint32_t source, uint32_t offset) {
  int val = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t data;
    libread(proc, source, offset + i, &data);
    if(data < 0) data = 256 + data;
    val |= ((int)(data & 0xFF)) << (i * 8);
=======
void write_int(BYTE* ram, int addr, int val) {
  for (int i = 0; i < 4; i++) {
      ram[addr + i] = (val >> (i * 8)) & 0xFF;
  }
}

int read_int(BYTE* ram, int addr) {
  int val = 0;
  for (int i = 0; i < 4; i++) {
      val |= ((int)(BYTE)ram[addr + i]) << (i * 8);
>>>>>>> b22f6b7d89c3edab3a01abe36dcf10a79a55ed1b
  }
  return val;
}

int libadd(struct pcb_t* proc, uint32_t source, uint32_t offset)
{
<<<<<<< HEAD
  // pthread_mutex_lock(&mmvm_lock);
  int data = read_int(proc, source, offset);
  printf("data: %d\n", data);
  data += 1;

  write_int(proc, source, offset, data);
#ifdef IODUMP
=======
  BYTE data;
  int value = 0;
  int count = 0;
  int c = 0;

  //
  int ofs = offset;
  do 
  {
    __read(proc, 0, source, ofs, &data);
    count ++;
    ofs++;
  }
  while (data!=0);
  printf("amount of byte read = %d\n", count);
  ofs = 0;
  do 
  {
    __read(proc, 0, source, ofs, &data);
    printf("offset %d, Data %d\n", ofs, data);
    value += ((int)data-48) * (int)pow(10, count-2-ofs);
    ofs++;
  }
  while (data!=0);
  printf("value read = %d\n", value);

  // add stage
  value ++;

  // write stage
  int ret = __write(proc, 0, source, offset, data);
  #ifdef IODUMP
>>>>>>> b22f6b7d89c3edab3a01abe36dcf10a79a55ed1b
  printf("===== PHYSICAL MEMORY AFTER ADDING =====\n");
  printf("read and add by 1 region=%d offset=%d value=%d\n", source, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
  struct mm_struct *mm = proc->mm;
  for(int i = 0; i < PAGING_MAX_PGN; i++) {
    uint32_t pte = mm->pgd[i];
    if(PAGING_PAGE_PRESENT(pte)) {
      int fpn = PAGING_PTE_FPN(pte);
      printf("Page Number: %d -> Frame Number: %d\n", i, fpn);
    }
  }
#endif /*PAGETBL_DUMP*/
  MEMPHY_dump(proc->mram);
<<<<<<< HEAD
#endif /*IODUMP*/
  // pthread_mutex_unlock(&mmvm_lock);
  return 0;
=======
#endif
  pthread_mutex_unlock(&mmvm_lock);
  return ret;
>>>>>>> b22f6b7d89c3edab3a01abe36dcf10a79a55ed1b
}

int shm_attach(struct pcb_t *proc, uint32_t va, uint32_t shm_key) {
  int pgn = PAGING_PGN(va);

  if (shm_key >= SHARED_MEM_SIZE) return -1;

  if (shm_table[shm_key] == -1) {
    int fpn;
    if (MEMPHY_get_freefp(proc->mram, &fpn) < 0) {
      return -1;
    }

    shm_table[shm_key] = fpn;
  }

  int shared_fpn = shm_table[shm_key];

  pte_set_fpn(&proc->mm->pgd[pgn], shared_fpn);

  enlist_pgn_node(&proc->mm->fifo_pgn, pgn);
  return 0;
}

/*get_symrg_byid - get mem region by region ID
 *@mm: memory region
 *@rgid: region ID act as symbol index of variable
 *
 */
struct vm_rg_struct *get_symrg_byid(struct mm_struct *mm, int rgid)
{
  if (rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
    return NULL;

  return &mm->symrgtbl[rgid];
}

/*__alloc - allocate a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *@alloc_addr: address of allocated memory region
 *
 */
int __alloc(struct pcb_t *caller, int vmaid, int rgid, int size, int *alloc_addr) // done
{
  /*Allocate at the toproof */
  struct vm_rg_struct rgnode;

  /* TODO: commit the vmaid */
  if(rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
    return -1;
  pthread_mutex_lock(&mmvm_lock);

  if (get_free_vmrg_area(caller, vmaid, size, &rgnode) == 0)
  {
    caller->mm->symrgtbl[rgid].rg_start = rgnode.rg_start;
    caller->mm->symrgtbl[rgid].rg_end = rgnode.rg_end;

    *alloc_addr = rgnode.rg_start;
    pthread_mutex_unlock(&mmvm_lock);
    return 0;
  }

  /* TODO get_free_vmrg_area FAILED handle the region management (Fig.6)*/

  /* TODO retrive current vma if needed, current comment out due to compiler redundant warning*/
  /*Attempt to increate limit to get space */
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);


  // int inc_sz = PAGING_PAGE_ALIGNSZ(size);

  /* TODO retrive old_sbrk if needed, current comment out due to compiler redundant warning*/
  int old_sbrk = cur_vma->sbrk;

  /* TODO INCREASE THE LIMIT as inovking systemcall 
   * sys_memap with SYSMEM_INC_OP 
   */
  struct sc_regs regs;
  regs.a1 = SYSMEM_INC_OP;
  regs.a2 = vmaid;
  regs.a3 = size;

  /* SYSCALL 17 sys_memmap */
  int syscall_ret = syscall(caller, 17, &regs);
  if(syscall_ret < 0) return -1;
  /* TODO: commit the limit increment */
  int new_sbrk = cur_vma->sbrk;
  /* TODO: commit the allocation address 
  // *alloc_addr = ...
  */

  caller->mm->symrgtbl[rgid].rg_start = old_sbrk;
  caller->mm->symrgtbl[rgid].rg_end = new_sbrk;

  *alloc_addr = old_sbrk;
  pthread_mutex_unlock(&mmvm_lock);
  return 0;

}

/*__free - remove a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __free(struct pcb_t *caller, int vmaid, int rgid) // done
{
  pthread_mutex_lock(&mmvm_lock);
  struct vm_rg_struct * rgnode = malloc(sizeof(struct vm_rg_struct));

  // Dummy initialization for avoding compiler dummay warning
  // in incompleted TODO code rgnode will overwrite through implementing
  // the manipulation of rgid later

  if(rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
    return -1;

  /* TODO: Manage the collect freed region to freerg_list */
  
  *rgnode = caller->mm->symrgtbl[rgid];
  rgnode->rg_next = NULL;
  caller->mm->symrgtbl[rgid].rg_start = 0;
  caller->mm->symrgtbl[rgid].rg_end = 0;
  
  /*enlist the obsoleted memory region */
  enlist_vm_freerg_list(caller->mm, rgnode);
  pthread_mutex_unlock(&mmvm_lock);
  return 0;
}

/*liballoc - PAGING-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int liballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index) // Đây là hàm tạm, còn sửa
{
  /* TODO Implement allocation on vm area 0 */
  int addr;
  /* By default using vmaid = 0 */
  int ret = __alloc(proc, 0, reg_index, size, &addr);
  if(ret == 0) {
    proc->regs[0] = addr;
  }

#ifdef IODUMP
  printf("===== PHYSICAL MEMORY AFTER ALLOCATION =====\n");
  printf("PID=%d - Region=%d - Address=%08x - Size=%d byte\n", proc->pid, reg_index, addr, size);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
  struct mm_struct *mm = proc->mm;
  for(int i = 0; i < PAGING_MAX_PGN; i++) {
    uint32_t pte = mm->pgd[i];
    if(PAGING_PAGE_PRESENT(pte)) {
      int fpn = PAGING_PTE_FPN(pte);
      printf("Page Number: %d -> Frame Number: %d\n", i, fpn);
    }
  }
#endif
  printf("================================================================\n");
#endif

  return ret;
}

/*libfree - PAGING-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */

int libfree(struct pcb_t *proc, uint32_t reg_index) // Đây là hàm tạm, còn sửa
{
  /* TODO Implement free region */
  /* By default using vmaid = 0 */
  int ret =  __free(proc, 0, reg_index);
  if(ret == 0) {
    proc->regs[0] = 0;
  }

#ifdef IODUMP
  printf("===== PHYSICAL MEMORY AFTER DEALLOCATION =====\n");
  printf("PID=%d - Region=%d\n", proc->pid, reg_index);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
  struct mm_struct *mm = proc->mm;
  for(int i = 0; i < PAGING_MAX_PGN; i++) {
    uint32_t pte = mm->pgd[i];
    if(PAGING_PAGE_PRESENT(pte)) {
      int fpn = PAGING_PTE_FPN(pte);
      printf("Page Number: %d -> Frame Number: %d\n", i, fpn);
    }
  }
#endif
  printf("================================================================\n");
#endif

  return ret;
}

/*pg_getpage - get the page in ram
 *@mm: memory region
 *@pagenum: PGN
 *@framenum: return FPN
 *@caller: caller
 *
 */
int pg_getpage(struct mm_struct *mm, int pgn, int *fpn, struct pcb_t *caller)
{
  uint32_t pte = mm->pgd[pgn];

  if (!PAGING_PAGE_PRESENT(pte))
  { /* Page is not online, make it actively living */
    int vicpgn, swpfpn; 
    int vicfpn;
    int tgtfpn = PAGING_PTE_SWP(pte);//the target frame storing our variable

    /* TODO: Play with your paging theory here */
    /* Find victim page */
    if(find_victim_page(caller->mm, &vicpgn) < 0) {
      return -3000;
    }

    /* Get free frame in MEMSWP */
    if (MEMPHY_get_freefp(caller->active_mswp, &swpfpn) < 0) {
      return -3000;
    }

    /* TODO: Implement swap frame from MEMRAM to MEMSWP and vice versa*/

    uint32_t vicpte = mm->pgd[vicpgn];
    vicfpn = PAGING_PTE_FPN(vicpte);

    /* TODO copy victim frame to swap 
    * SWP(vicfpn <--> swpfpn)
    * SYSCALL 17 sys_memmap 
    * with operation SYSMEM_SWP_OP
    */
    struct sc_regs regs;
    regs.a1 = SYSMEM_SWP_OP;
    regs.a2 = vicfpn;
    regs.a3 = swpfpn;
    /* SYSCALL 17 sys_memmap */
    syscall(caller, 17, &regs);

    /* TODO copy target frame form swap to mem 
    * SWP(tgtfpn <--> vicfpn)
    * SYSCALL 17 sys_memmap
    * with operation SYSMEM_SWP_OP
    */
    /* TODO copy target frame form swap to mem */ 
    // regs.a1 = SYSMEM_SWP_OP;
    // regs.a2 = swpfpn;
    // regs.a3 = tgtfpn;
    // /* SYSCALL 17 sys_memmap */
    // syscall(caller, 17, &regs);
    __swap_cp_page(caller->active_mswp, tgtfpn, caller->mram, vicfpn); // gọi hàm trực tiếp vì ko hỗ trợ swap ngược
    *fpn = vicfpn;

    /* Update page table */
    // pte_set_swap(&mm->pgd[vicpgn], 0, swpfpn);
    pte_set_swap(&mm->pgd[vicpgn], 0, tgtfpn);

    /* Update its online status of the target page */
    pte_set_fpn(&mm->pgd[pgn], vicfpn);

    enlist_pgn_node(&caller->mm->fifo_pgn,pgn);
  }

  *fpn = PAGING_FPN(mm->pgd[pgn]);

  return 0;
}

/*pg_getval - read value at given offset
 *@mm: memory region
 *@addr: virtual address to acess
 *@value: value
 *
 */
int pg_getval(struct mm_struct *mm, int addr, BYTE *data, struct pcb_t *caller) // done
{
  int pgn = PAGING_PGN(addr);
  int off = PAGING_OFFST(addr);
  int fpn;

  /* Get the page to MEMRAM, swap from MEMSWAP if needed */
  if (pg_getpage(mm, pgn, &fpn, caller) != 0)
    return -1; /* invalid page access */

  /* TODO 
   *  MEMPHY_read(caller->mram, phyaddr, data);
   *  MEMPHY READ 
   *  SYSCALL 17 sys_memmap with SYSMEM_IO_READ
   */
  // int phyaddr = fpn * PAGE_SIZE + off; // ko biết đúng ko
  int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off;

  struct sc_regs regs;
  regs.a1 = SYSMEM_IO_READ;
  regs.a2 = phyaddr;
  regs.a3 = 0;
  syscall(caller, 17, &regs);

  // Update data
  *data = (BYTE)regs.a3;

  return 0;
}

/*pg_setval - write value to given offset
 *@mm: memory region
 *@addr: virtual address to acess
 *@value: value
 *
 */
int pg_setval(struct mm_struct *mm, int addr, BYTE value, struct pcb_t *caller) // done
{
  int pgn = PAGING_PGN(addr);
  int off = PAGING_OFFST(addr);
  int fpn;

  /* Get the page to MEMRAM, swap from MEMSWAP if needed */
  if (pg_getpage(mm, pgn, &fpn, caller) != 0)
    return -1; /* invalid page access */

  /* TODO
   *  MEMPHY_write(caller->mram, phyaddr, value);
   *  MEMPHY WRITE
   *  SYSCALL 17 sys_memmap with SYSMEM_IO_WRITE
   */
  // int phyaddr = fpn * PAGE_SIZE + off;
  int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off;

  struct sc_regs regs;
  regs.a1 = SYSMEM_IO_WRITE;
  regs.a2 = phyaddr;
  regs.a3 = (uint32_t)value;
  syscall(caller, 17, &regs);

  // Update data
  // data = (BYTE)

  return 0;
}

/*__read - read value in region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@offset: offset to acess in memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __read(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE *data)
{
  struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
    return -1;

  pg_getval(caller->mm, currg->rg_start + offset, data, caller);

  return 0;
}

/*libread - PAGING-based read a region memory */
int libread(
    struct pcb_t *proc, // Process executing the instruction
    uint32_t source,    // Index of source register
    uint32_t offset,    // Source address = [source] + [offset]
    uint32_t* destination)
{
  pthread_mutex_lock(&mmvm_lock);
  BYTE data;
  int val = __read(proc, 0, source, offset, &data);

  /* TODO update result of reading action*/
  //destination 
  if(val==0) {
    *destination = data;
  }
#ifdef IODUMP
  printf("===== PHYSICAL MEMORY AFTER READING =====\n");
  printf("read region=%d offset=%d value=%d\n", source, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
  struct mm_struct *mm = proc->mm;
  for(int i = 0; i < PAGING_MAX_PGN; i++) {
    uint32_t pte = mm->pgd[i];
    if(PAGING_PAGE_PRESENT(pte)) {
      int fpn = PAGING_PTE_FPN(pte);
      printf("Page Number: %d -> Frame Number: %d\n", i, fpn);
    }
  }
#endif
  MEMPHY_dump(proc->mram);
#endif
  pthread_mutex_unlock(&mmvm_lock);
  return val;
}

/*__write - write a region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@offset: offset to acess in memory region
 *@rgid: memory region ID (used to identify variable in symbole table)
 *@size: allocated size
 *
 */
int __write(struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE value)
{
  struct vm_rg_struct *currg = get_symrg_byid(caller->mm, rgid);
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
    return -1;

  pg_setval(caller->mm, currg->rg_start + offset, value, caller);

  return 0;
}

/*libwrite - PAGING-based write a region memory */
int libwrite(
    struct pcb_t *proc,   // Process executing the instruction
    BYTE data,            // Data to be wrttien into memory
    uint32_t destination, // Index of destination register
    uint32_t offset)
{
  pthread_mutex_lock(&mmvm_lock);
  int ret = __write(proc, 0, destination, offset, data);
#ifdef IODUMP
  printf("===== PHYSICAL MEMORY AFTER WRITING =====\n");
  printf("write region=%d offset=%d value=%d\n", destination, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
  struct mm_struct *mm = proc->mm;
  for(int i = 0; i < PAGING_MAX_PGN; i++) {
    uint32_t pte = mm->pgd[i];
    if(PAGING_PAGE_PRESENT(pte)) {
      int fpn = PAGING_PTE_FPN(pte);
      printf("Page Number: %d -> Frame Number: %d\n", i, fpn);
    }
  }
#endif
  MEMPHY_dump(proc->mram);
#endif
  pthread_mutex_unlock(&mmvm_lock);
  return ret;
}

/*free_pcb_memphy - collect all memphy of pcb
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@incpgnum: number of page
 */
int free_pcb_memph(struct pcb_t *caller)
{
  int pagenum, fpn;
  uint32_t pte;


  for(pagenum = 0; pagenum < PAGING_MAX_PGN; pagenum++)
  {
    pte= caller->mm->pgd[pagenum];

    if (!PAGING_PAGE_PRESENT(pte))
    {
      fpn = PAGING_PTE_FPN(pte);
      MEMPHY_put_freefp(caller->mram, fpn);
    } else {
      fpn = PAGING_PTE_SWP(pte);
      MEMPHY_put_freefp(caller->active_mswp, fpn);    
    }
  }

  return 0;
}


/*find_victim_page - find victim page
 *@caller: caller
 *@pgn: return page number
 *
 */
int find_victim_page(struct mm_struct *mm, int *retpgn)
{
  struct pgn_t *pg = mm->fifo_pgn;
  struct pgn_t *prev = NULL;

  /* TODO: Implement the theorical mechanism to find the victim page */
  if(!pg) return -1;
  while(pg->pg_next != NULL) {
    prev = pg;
    pg = pg->pg_next;
  }

  *retpgn = pg->pgn;

  if(prev) {
    prev->pg_next = NULL;
  } else {
    mm->fifo_pgn = NULL;
  }

  free(pg);

  return 0;
}

/*get_free_vmrg_area - get a free vm region
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@size: allocated size
 *
 */
int get_free_vmrg_area(struct pcb_t *caller, int vmaid, int size, struct vm_rg_struct *newrg) // done
{
  struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

  struct vm_rg_struct *rgit = cur_vma->vm_freerg_list;

  if (rgit == NULL)
    return -1;

  /* Probe unintialized newrg */
  newrg->rg_start = newrg->rg_end = -1;

  /* TODO Traverse on list of free vm region to find a fit space */
  //while (...)
  // ..
  struct vm_rg_struct *rg_prev = NULL;
  while(rgit != NULL) {
    if (rgit->rg_end - rgit->rg_start >= size) {
      *newrg = *rgit;
      if(rgit == cur_vma->vm_freerg_list) {
        cur_vma->vm_freerg_list = rgit->rg_next;
      } else {
        rg_prev->rg_next = rgit->rg_next;
      }
      return 0;
    }
    rg_prev = rgit;
    rgit = rgit->rg_next;
  }

  return -1;
}



//#endif
