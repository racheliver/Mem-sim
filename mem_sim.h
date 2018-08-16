
#ifndef MEM_SIM_H_INCLUDED
#define MEM_SIM_H_INCLUDED

#define PAGE_SIZE 5
#define MEMORY_SIZE 20
#define SWAP_SIZE 125
#define NUM_OF_PAGES 25

typedef struct page_descriptor{
    unsigned int V ;
    unsigned int D;
    unsigned int P;
    int frame;
}page_descriptor;

typedef  struct sim_database{
    page_descriptor page_table[NUM_OF_PAGES];
    int swapfile_fd;
    int program_fd;
    char main_memory[MEMORY_SIZE];
    int text_size;
    int data_bss_size;
    int heap_stack_size;
}sim_database;
void store(struct sim_database * mem_sim , int address, char value);
//the order that the pages entered to RAM
int theNewOne();
int order;
int indexRam;
int order_in_ram[25]; //aray that updating the order of entry to RAM

void cleanSwap(struct sim_database * mem_sim );
sim_database* init_system(char exe_file_name[],char swap_file_name[],int text_size,int data_bss_size,int heap_stack_size );


int findPage(struct sim_database *mem_sim, int address, int LogicalPage);
int get_frame_in_ram(struct sim_database *mem_sim, int address);
void readFromExe(struct sim_database * mem_sim,int numOfRead ,char *buffer,int logicAddressBlock );
void enterToRam(struct sim_database * mem_sim ,int LogicPage, int from);
void print_memory(sim_database* mem_sim);
void WriteToSwap(struct sim_database * mem_sim,int numOfWrite, int from, char *buffer);
void readFromSwap(struct sim_database * mem_sim,int numOfRead, int from, char *buffer );
char load (struct sim_database * mem_sim , int address);
void clear_system(struct sim_database * mem_sim);
void print_swap(sim_database* mem_sim);
void print_page_table(sim_database* mem_sim);
#endif // MEM_SIM_H_INCLUDED
