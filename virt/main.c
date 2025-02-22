#include <stdio.h>
#include "driver.h"


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t kernel_size = 100000;

    Mem_T *mem = init_memory_system(kernel_size);

    // vs_malloc
    uint32_t mal_size = 100;
    uint32_t first_malloc_result = vs_malloc(mem, mal_size);

    uint32_t res2 = vs_malloc(mem, 25);

    printf("Address of first malloc %u\n", first_malloc_result);
    printf("Address of second malloc %u\n", res2);
    
    vs_free(mem, first_malloc_result);

    printf("Hi there\n");
    return 0;
}
