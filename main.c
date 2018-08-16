#include<stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "mem_sim.h"

        
int main(){
    
    char val;
    struct sim_database * mem_sim = init_system("exec_file", "swap_file" ,25, 50, 50) ;


printf("------try64----------\n");
val = load (mem_sim , 64);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}

print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);


printf("------try66----------\n");
val = load (mem_sim , 66);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}

print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);



printf("------try2----------\n");

val = load (mem_sim , 2);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);

printf("------store98----------\n");
store(mem_sim , 98,'X');
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);




printf("------load16----------\n");
val = load (mem_sim ,16);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);





printf("------load70---------\n");
val = load (mem_sim ,70);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);






printf("------store32----------\n");
store(mem_sim ,32,'Y');
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);






printf("------store15----------\n");


store (mem_sim ,15,'Z');

for(int i=0;i<NUM_OF_PAGES;i++){
printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);



printf("------try23----------\n");
val = load (mem_sim ,23);
   for(int i=0;i<NUM_OF_PAGES;i++){
    printf("%d,",order_in_ram[i]);}
print_memory(mem_sim);
print_page_table(mem_sim);
print_swap(mem_sim);
clear_system(mem_sim); 
}
