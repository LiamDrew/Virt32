#include <stdio.h>
#include <stdint.h>

#include "driver.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t kernel_size = 100000;

    Mem_T *mem = init_memory_system(kernel_size);
    (void)mem;

    // vs_malloc
    uint32_t mal_size = 100;
    uint32_t first_malloc_result = vs_malloc(mem, mal_size);

    uint32_t res2 = vs_malloc(mem, 25);

    printf("Address of first malloc %u\n", first_malloc_result);
    printf("Address of second malloc %u\n", res2);

    uint32_t a1 = get_blocks_from_alloc_size(23);
    uint32_t a2 = get_blocks_from_alloc_size(24);
    uint32_t a3 = get_blocks_from_alloc_size(25);

    printf("Block size should be 1 %u\n", a1);
    printf("Block size should be 1 %u\n", a2);
    printf("Block size should be 2 %u\n", a3);
    vs_free(mem, first_malloc_result);
    
}
