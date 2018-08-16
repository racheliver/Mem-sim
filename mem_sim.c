#include "mem_sim.h"

#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

#define CLEAN_CHAR '0'
#define FRAME_NO_EXISTS_IN_RAM -1
#define  CODE_AREA_IN_RAM 0
#define  DATA_BSS_IN_RAM 1
#define HEAP_STACK_IN_RAM 2
#define OUT_LIMIT -2
#define CODE_AREA -10
#define IN_SWAP -20
#define DATA_BSS -30
#define HEAP_STACK -40
#define NOT_ALLOCATED -50

int execOrder[4];//array from frame to logic address 
int beenHere=0;//flag that let us now if the memory start to be full
int Store=0;
int indexRam=0;
int numberPage;
/**
 * 
 * 
 * In this exercise we will simulate a processor's approach to memory. 
 * we will use the paging mechanism, which allows you to run a program by parts
 *Program memory is divided into pages that are brought to the main memory

*The findpage function finds the logical location of the page where the page came from, meaning that it came from the SWAP, RAM, and so on.

*The load function receives a logical address to access for reading. The function ensures that the relevant page of the address is in memory. If the address does not exist in memory, it will bring it.

  
*STORE function Ensure that the page that corresponds to the address is in the main memory and then access the physical address and store the value of the character in that address.

*The ReadFrom / Swap / Exce functions read from the files received into the program.

*The get_frame in ram function finds the frame from the master memory

*The clearsystem function releases the program resources and allocations at the end

*The cleanswap function clears the file values.


 */

//--------------------------------------------------------------------------------------------
/**
 * 
 * @param exe_file_name 
 * @param swap_file_name 
 * @param text_size 
 * @param data_bss_size 
 * @param heap_stack_size 
 * @return 
 */

sim_database* init_system(char exe_file_name[],char swap_file_name[],int text_size,int data_bss_size,int heap_stack_size )
{
    int i;
    struct sim_database * sim_db = (struct sim_database *) malloc( sizeof(struct sim_database ) ) ;

    //psudo code:
    /*
    steps:

    1-  if name of file == null
      ###
    2- open both of files and check if file no exists
      ###

    3-  sim_db -> main memory -> restart with '0'

    4-  sim_db -> page_descriptor > restart with 0 and -1
    */
    if(exe_file_name==NULL || swap_file_name==NULL){
        puts("name of one of the files is null");
        free(sim_db);
        exit (1);
    }

    sim_db->program_fd = open(exe_file_name, O_RDONLY);
    if (sim_db->program_fd < 0) {
        perror("Cannot open executable file! \n");
        exit(1);
    }

    sim_db->swapfile_fd =open(swap_file_name, O_RDWR|O_CREAT|O_TRUNC ,0600);
    if (sim_db->swapfile_fd < 0) {
        perror("Cannot open swap file! \n");

    }

    for (i = 0; i < MEMORY_SIZE; i++)
        sim_db->main_memory[i] = '0';

    for (int i = 0; i < NUM_OF_PAGES; i++) {
        sim_db->page_table[i].D = 0;
        sim_db->page_table[i].frame = -1;
        sim_db->page_table[i].V = 0;

        if(i < text_size/PAGE_SIZE)
        {sim_db->page_table[i].P = 0;}
        else if( i > (text_size/PAGE_SIZE)-1 && i < (text_size + data_bss_size)/PAGE_SIZE)
        {sim_db->page_table[i].P = 1;}
        else
        {sim_db->page_table[i].P = 2;}

    }

    sim_db->data_bss_size = data_bss_size ;
    sim_db->heap_stack_size = heap_stack_size ;
    sim_db->text_size = text_size ;
//initiation order_in_ram
    for(int i =0; i< NUM_OF_PAGES;i++)
    {order_in_ram[i]=100;}
    order=0;
    for(int i=0;i<4;i++){
      execOrder[i]=0;}
    cleanSwap(sim_db);
    return sim_db ;

}

//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 * @param address 
 * @param value 
 */


void store(struct sim_database * mem_sim , int address, char value){

    int physicAdd,physicBlc;
    int numPage =address/PAGE_SIZE;

    numberPage=numPage;
    int offset = address % PAGE_SIZE;
    Store=1;
    int pagePlace= findPage(mem_sim,address,numPage);

    if(pagePlace==OUT_LIMIT)
        return;
    if(pagePlace==CODE_AREA || pagePlace==CODE_AREA_IN_RAM){
        perror("ERROR: read permission only");
        return;
    }

    if(pagePlace!=CODE_AREA_IN_RAM && pagePlace!= DATA_BSS_IN_RAM && pagePlace!=HEAP_STACK_IN_RAM){

        if(pagePlace==CODE_AREA)
        {
            enterToRam( mem_sim , numPage, CODE_AREA);//need to delete adress from function
        }
        else if(pagePlace== DATA_BSS )
        {
            enterToRam( mem_sim ,numPage, DATA_BSS);
        }
        else if(pagePlace==HEAP_STACK)
        {
            enterToRam( mem_sim , numPage, HEAP_STACK);
        }
        else if(pagePlace==IN_SWAP)
        {
            enterToRam( mem_sim ,numPage, IN_SWAP);
        }



    }


    mem_sim->page_table[numPage].D=1;

    physicBlc = get_frame_in_ram(mem_sim ,address);
    physicAdd = physicBlc*PAGE_SIZE + offset;
    if(pagePlace!=CODE_AREA)
        mem_sim->main_memory[physicAdd]=value;


}
//--------------------------------------------------------------------------------------------

/**
 * 
 * @param mem_sim 
 * @param address 
 * @return 
 */
int get_frame_in_ram(struct sim_database *mem_sim, int address){
//calc logical address to physically address without offset (calc the place of the block)

    int numPage= address / PAGE_SIZE;
    if(mem_sim->page_table[numPage].V==1){//the page exists in ram
        return mem_sim->page_table[numPage].frame;
    }

    return FRAME_NO_EXISTS_IN_RAM;
}
//--------------------------------------------------------------------------------------------

/**
 * 
 * @return 
 */

int theNewOne(){
    int min = 26;
    int index =-1;
    for(int i =0 ; i< NUM_OF_PAGES ; i++){
        if(order_in_ram[i]<min){
            min = order_in_ram[i];
            index = i;}
    }
    return index;
}



//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 */
void cleanSwap(struct sim_database * mem_sim ){
    int i=0;
    char buffer[(SWAP_SIZE)+1];

    for(i=0;i<SWAP_SIZE;i++){
        buffer[i]=CLEAN_CHAR;
    }
    WriteToSwap(mem_sim,SWAP_SIZE, 0,buffer);
}



//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 * @param numOfWrite 
 * @param from 
 * @param buffer 
 */
void WriteToSwap(struct sim_database * mem_sim,int numOfWrite, int from, char *buffer){

    buffer[numOfWrite]='\0';
    if(lseek(mem_sim->swapfile_fd,from,SEEK_SET)!=from){
        clear_system(mem_sim);
        exit(-1);//check if need
    }
    if(write(mem_sim->swapfile_fd,buffer,numOfWrite)!=numOfWrite){
        clear_system(mem_sim);
        puts("write no success");
        exit (-1);
    }

}

//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 */
void clear_system(struct sim_database * mem_sim){

    close(mem_sim->program_fd);
    close(mem_sim->swapfile_fd);
    free(mem_sim);
}
/**************************************************************************************/
void print_memory(sim_database* mem_sim) {
    int i;
    printf("\n Physical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", mem_sim->main_memory[i]);
    }
}
/************************************************************************************/
void print_swap(sim_database* mem_sim) {
    char str[PAGE_SIZE];
    int i;
    printf("\n Swap memory\n");
    lseek(mem_sim->swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(mem_sim->swapfile_fd, str, PAGE_SIZE) == PAGE_SIZE) {
        for(i = 0; i < PAGE_SIZE; i++) {
            printf("[%c]\t", str[i]);
        }
        printf("\n");
    }
}
/***************************************************************************************/
void print_page_table(sim_database* mem_sim) {
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
    for (i = 0; i < NUM_OF_PAGES; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n", mem_sim->page_table[i].V,
               mem_sim->page_table[i].D,
               mem_sim->page_table[i].P, mem_sim->page_table[i].frame);
    }
}


/***************************************************************************************/

/***************************************************************************************/
/**
 * 
 * @param mem_sim 
 * @param LogicPage 
 * @param from 
 */
void enterToRam(struct sim_database * mem_sim,int LogicPage, int from) {
    int logicAddressBlock = LogicPage * PAGE_SIZE;

    int l=execOrder[beenHere];
    int offset=logicAddressBlock%PAGE_SIZE;

    int i;//the address of the page without offset
    char buffer[PAGE_SIZE + 1];
    buffer[PAGE_SIZE] = '\0';
    mem_sim->page_table[LogicPage].V = 1;
    mem_sim->page_table[LogicPage].frame = indexRam / PAGE_SIZE;
    int frame=mem_sim->page_table[LogicPage].frame;

    order++;
    int index = theNewOne();
    order_in_ram[LogicPage] = order;
    if (indexRam == 20 || beenHere != 0) {

        if (indexRam == 20)
            indexRam = 0;

        char buffer[PAGE_SIZE + 1];
        buffer[PAGE_SIZE] = '\0';
        int k = 0;
        for (int i = indexRam; i < indexRam + 5; i++) {
            buffer[k] = mem_sim->main_memory[i];
            k++;
        }

        order_in_ram[index] = 100;

        if (mem_sim->page_table[index].D == 1) {
            WriteToSwap(mem_sim, PAGE_SIZE, (l*PAGE_SIZE), buffer);

        }

        if (order_in_ram[index] == 100) {
            mem_sim->page_table[index].V = 0;
            mem_sim->page_table[index].frame = -1;
        }
        mem_sim->page_table[index].V = 0;
        mem_sim->page_table[index].frame = -1;

        mem_sim->page_table[LogicPage].frame = indexRam / PAGE_SIZE;
        beenHere++;


    }

    if ((from == CODE_AREA || from == DATA_BSS)) {
        //copy block to the ram:

        readFromExe(mem_sim, PAGE_SIZE, buffer, logicAddressBlock);

        for (i = 0; i < PAGE_SIZE; i++) {
            mem_sim->main_memory[indexRam + i] = buffer[i];
        }


    } else if (from == IN_SWAP) {
        readFromSwap(mem_sim, PAGE_SIZE,numberPage*PAGE_SIZE, buffer);

        for (i = 0; i < PAGE_SIZE; i++) {
            mem_sim->main_memory[indexRam + i] = buffer[i];
        }
    } else if ( from == HEAP_STACK) {

        for (i = 0; i < PAGE_SIZE; i++) {
            buffer[i] = '0';
            mem_sim->main_memory[indexRam + i] = buffer[i];
        }

    }
    else if(Store==0 &&(from == NOT_ALLOCATED ))
    {
        readFromExe(mem_sim, PAGE_SIZE, buffer, logicAddressBlock);
        for (i = 0; i < PAGE_SIZE; i++) {
            if(i==offset)
                mem_sim->main_memory[indexRam + i] = buffer[i];
        }
    }
    execOrder[indexRam/5]=LogicPage;
    indexRam = indexRam + 5;

    numberPage=execOrder[indexRam/5];
}

/***************************************************************************************/

/***************************************************************************************/
/**
 * 
 * @param mem_sim 
 * @param address 
 * @param LogicalPage 
 * @return 
 */
int findPage(struct sim_database * mem_sim, int address,int LogicalPage){
    int frame =get_frame_in_ram(mem_sim, address);//find the frame in the main memory (if exicst)
    /*
     * cuclate all the space that we have in memory(heap,stack,bss,data,text) and thst is ourlimit
     * */
    int limit_size = mem_sim->text_size + mem_sim->heap_stack_size + mem_sim->data_bss_size;

/**
 * if we have HIT! then we check what is the frame
 */


    if (address < 0 || address >= limit_size) {
        perror("ERROR: Outside the limit");
        return OUT_LIMIT;
    }
    /***
     * if page in ram we return what his area....
     */
    if (frame != FRAME_NO_EXISTS_IN_RAM) {
        if (mem_sim->page_table[LogicalPage].P == 0)
            return CODE_AREA_IN_RAM;

        else if (mem_sim->page_table[LogicalPage].P == 1)
            return DATA_BSS_IN_RAM;
        else if (mem_sim->page_table[LogicalPage].P == 2)
            return HEAP_STACK_IN_RAM;
    }
    /**
     * else we have **MISS**... we see that page not in ram , then decide from where to take him
     */
    if (mem_sim->page_table[LogicalPage].P == 0)//page in code
        return CODE_AREA;

    if (mem_sim->page_table[LogicalPage].P == 1)//data_bss check now if it dirty
    {
        if (mem_sim->page_table[LogicalPage].D == 1)
            return IN_SWAP;
        if (mem_sim->page_table[LogicalPage].D == 0)
            return DATA_BSS;
    }
    if (mem_sim->page_table[LogicalPage].P == 2)//stack or heap!
        return HEAP_STACK;


    return NOT_ALLOCATED;
}


//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 * @param numOfRead 
 * @param from 
 * @param buffer 
 */
void readFromSwap(struct sim_database * mem_sim,int numOfRead, int from, char *buffer ){

    buffer[numOfRead]='\0';
    if(lseek(mem_sim->swapfile_fd,from,SEEK_SET)!=from){
        clear_system(mem_sim);
        perror("read no success");
        exit(-1);//check if need
    }
    if(read(mem_sim->swapfile_fd,buffer,numOfRead)!=numOfRead){
        clear_system(mem_sim);
        puts("read no success");
        exit (-1);//need exit if no read?
    }

}
//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 * @param numOfRead 
 * @param buffer 
 * @param logicAddressBlock 
 */

void readFromExe(struct sim_database * mem_sim,int numOfRead ,char *buffer,int logicAddressBlock ){

    buffer[numOfRead]='\0';
    if (lseek(mem_sim->program_fd, logicAddressBlock, SEEK_SET) != logicAddressBlock) {
        clear_system(mem_sim);
        exit(-1);//check if need
    }
    if (read(mem_sim->program_fd, buffer, numOfRead) != numOfRead) {
        clear_system(mem_sim);
        puts("read no success");
        exit(-1);//need exit if no read?
    }

}
//--------------------------------------------------------------------------------------------
/**
 * 
 * @param mem_sim 
 * @param address 
 * @return 
 */

char load (struct sim_database * mem_sim , int address) {

    Store=0;
    int offset = address%PAGE_SIZE;
    int LogicPage=address/PAGE_SIZE;

    numberPage=LogicPage;
    int physicFrame;int physicAdd;

    int pagePlace=findPage(mem_sim,address,LogicPage);

    if(pagePlace==OUT_LIMIT)
        return '\0';
    if(pagePlace==NOT_ALLOCATED){

        perror("ERROR: THIS PAGE IS NOT ALLOCATED");
        return'\0';
    }

    if(pagePlace!=CODE_AREA_IN_RAM && pagePlace!= DATA_BSS_IN_RAM && pagePlace!=HEAP_STACK_IN_RAM){

        if(pagePlace==CODE_AREA)
        {
            enterToRam( mem_sim , LogicPage, CODE_AREA);//need to delete adress from function
        }
        else if(pagePlace== DATA_BSS )
        {
            enterToRam( mem_sim ,LogicPage, DATA_BSS);
        }
        else if(pagePlace==HEAP_STACK)
        {
            enterToRam( mem_sim , LogicPage, HEAP_STACK);
        }
        else if(pagePlace==IN_SWAP)
        {
            enterToRam( mem_sim , LogicPage, IN_SWAP);
        }

    }
    physicFrame = get_frame_in_ram(mem_sim ,address);
    physicAdd = physicFrame*PAGE_SIZE + offset;

    return (mem_sim->main_memory[physicAdd]);


}

//--------------------------------------------------------------------------------------------

