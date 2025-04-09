
#include "mem.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <stdio.h>
// Thêm
#include "common.h"
/////////
static BYTE _ram[RAM_SIZE];

static struct {
	uint32_t proc;	// ID of process currently uses this page
	int index;	// Index of the page in the list of pages allocated
			// to the process.
	int next;	// The next page in the list. -1 if it is the last
			// page.
} _mem_stat [NUM_PAGES];

static pthread_mutex_t mem_lock;

void init_mem(void) {
	memset(_mem_stat, 0, sizeof(*_mem_stat) * NUM_PAGES);
	memset(_ram, 0, sizeof(BYTE) * RAM_SIZE);
	pthread_mutex_init(&mem_lock, NULL);
}

/* get offset of the virtual address */
static addr_t get_offset(addr_t addr) {
	return addr & ~((~0U) << OFFSET_LEN);
}

/* get the first layer index */
static addr_t get_first_lv(addr_t addr) {
	return addr >> (OFFSET_LEN + PAGE_LEN);
}

/* get the second layer index */
static addr_t get_second_lv(addr_t addr) {
	return (addr >> OFFSET_LEN) - (get_first_lv(addr) << PAGE_LEN);
}

/* Search for page table table from the a segment table */
static struct trans_table_t * get_trans_table(
		addr_t index, 	// Segment level index
		struct page_table_t * page_table) { // first level table
	
	/* DO NOTHING HERE. This mem is obsoleted */

	int i; 
	for (i = 0; i < page_table->size; i++) {
		// Enter your code here
		if(page_table->table[i].v_index == index) {
			return page_table->table->next_lv;
		}
	}
	return NULL;

}

/* Translate virtual address to physical address. If [virtual_addr] is valid,
 * return 1 and write its physical counterpart to [physical_addr].
 * Otherwise, return 0 */
static int translate(
		addr_t virtual_addr, 	// Given virtual address
		addr_t * physical_addr, // Physical address to be returned
		struct pcb_t * proc) {  // Process uses given virtual address

	/* Offset of the virtual address */
	addr_t offset = get_offset(virtual_addr);
        offset++; offset--;
	/* The first layer index */
	addr_t first_lv = get_first_lv(virtual_addr);
	/* The second layer index */
	addr_t second_lv = get_second_lv(virtual_addr);
	
	/* Search in the first level */
	struct trans_table_t * trans_table = NULL;
	trans_table = get_trans_table(first_lv, proc->page_table);
	if (trans_table == NULL) {
		return 0;
	}

	int i;
	for (i = 0; i < trans_table->size; i++) {
		if (trans_table->table[i].v_index == second_lv) {
			/* DO NOTHING HERE. This mem is obsoleted */
            addr_t frame_number = trans_table->table[i].p_index;

            *physical_addr = frame_number * PAGE_SIZE + offset;
            return 1;
		}
	}
	return 0;	
}

addr_t alloc_mem(uint32_t size, struct pcb_t * proc) {
	pthread_mutex_lock(&mem_lock);
	addr_t ret_mem = 0;
	/* DO NOTHING HERE. This mem is obsoleted */

	uint32_t num_pages = (size % PAGE_SIZE) ? size / PAGE_SIZE :
		size / PAGE_SIZE + 1; // Number of pages we will use
	int mem_avail = 0; // We could allocate new memory region or not?

	/* First we must check if the amount of free memory in
	 * virtual address space and physical address space is
	 * large enough to represent the amount of required 
	 * memory. If so, set 1 to [mem_avail].
	 * Hint: check [proc] bit in each page of _mem_stat
	 * to know whether this page has been used by a process.
	 * For virtual memory space, check bp (break pointer).
	 * */
	
	 if (proc->bp + num_pages * PAGE_SIZE <= USER_MEM_LIMIT) {  
        int free_frames[num_pages]; 
        int found_pages = 0;

        for (int i = 0; i < NUM_PAGES && found_pages < num_pages; i++) {
            if (_mem_stat[i].proc == 0) { // Nếu trang này chưa được sử dụng
                free_frames[found_pages++] = i;
            }
        }

        if (found_pages == num_pages) {
            mem_avail = 1;  // Đủ bộ nhớ để cấp phát
        }
    }
	
	if (mem_avail) {
		/* We could allocate new memory region to the process */
		ret_mem = proc->bp;
		proc->bp += num_pages * PAGE_SIZE;
		/* Update status of physical pages which will be allocated
		 * to [proc] in _mem_stat. Tasks to do:
		 * 	- Update [proc], [index], and [next] field
		 * 	- Add entries to segment table page tables of [proc]
		 * 	  to ensure accesses to allocated memory slot is
		 * 	  valid. */
		for (int i = 0; i < num_pages; i++) {
			int fpn = free_frames[i]; 
			_mem_stat[fpn].proc = proc->pid;
			_mem_stat[fpn].index = i;
			_mem_stat[fpn].next = (i == num_pages - 1) ? -1 : free_frames[i + 1];

			// Cập nhật bảng trang
			uint32_t page_num = (ret_mem / PAGE_SIZE) + i;
			proc->page_table[page_num] = fpn;
		}
	}
	pthread_mutex_unlock(&mem_lock);
	return ret_mem;
}

int free_mem(addr_t address, struct pcb_t *proc) {
    pthread_mutex_lock(&mem_lock);

    uint32_t page_num = address / PAGE_SIZE;
    
    // Kiểm tra xem địa chỉ có hợp lệ không
    if (proc->page_table[page_num] == 0) {
        pthread_mutex_unlock(&mem_lock);
        return -1; // Không có bộ nhớ nào được cấp phát ở đây
    }

    int fpn = GETVAL(proc->page_table[page_num], PAGING_PTE_FPN_MASK, PAGING_PTE_FPN_LOBIT);

    // Giải phóng từng trang
    while (fpn != -1) {
        int next_fpn = _mem_stat[fpn].next;

        // Đánh dấu frame này là trống
        _mem_stat[fpn].proc = 0;
        _mem_stat[fpn].index = 0;
        _mem_stat[fpn].next = -1;

        // Xóa entry trong bảng trang
        proc->page_table[page_num] = 0;

        // Chuyển sang frame tiếp theo
        fpn = next_fpn;
        page_num++;
    }

    // Cập nhật break pointer nếu giải phóng vùng cuối cùng
    if (address + PAGE_SIZE >= proc->bp) {
        proc->bp = address;
    }

    pthread_mutex_unlock(&mem_lock);
    return 0;
}

int read_mem(addr_t address, struct pcb_t * proc, BYTE * data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		*data = _ram[physical_addr];
		return 0;
	}else{
		return 1;
	}
}

int write_mem(addr_t address, struct pcb_t * proc, BYTE data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		_ram[physical_addr] = data;
		return 0;
	}else{
		return 1;
	}
}

void dump(void) {
	int i;
	for (i = 0; i < NUM_PAGES; i++) {
		if (_mem_stat[i].proc != 0) {
			printf("%03d: ", i);
			printf("%05x-%05x - PID: %02d (idx %03d, nxt: %03d)\n",
				i << OFFSET_LEN,
				((i + 1) << OFFSET_LEN) - 1,
				_mem_stat[i].proc,
				_mem_stat[i].index,
				_mem_stat[i].next
			);
			int j;
			for (	j = i << OFFSET_LEN;
				j < ((i+1) << OFFSET_LEN) - 1;
				j++) {
				
				if (_ram[j] != 0) {
					printf("\t%05x: %02x\n", j, _ram[j]);
				}
					
			}
		}
	}
}


