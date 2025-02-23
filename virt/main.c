#include <stdio.h>
#include <stdint.h>

#include "driver.h"
#include "mem_state.h"

#include "stack.h"
#include "recycler.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // Should this be virtual address kernel 
    uint32_t kernel_virtual_size = (uint32_t)1 << 16;

    Mem_T *mem = init_memory_system(kernel_virtual_size);

    uint32_t mal_size = 100;
    uint32_t first_malloc_result = vs_malloc(mem, mal_size);
    printf("Trying to malloc %u bytes\n", mal_size);
    printf("Address of first malloc %u\n", (first_malloc_result - kernel_virtual_size));


    uint32_t mal_size2 = 96;
    uint32_t second_malloc_result = vs_malloc(mem, mal_size2);
    printf("Trying to malloc %u bytes\n", mal_size2);
    printf("Address of second malloc %u\n", (second_malloc_result - kernel_virtual_size));

    uint32_t mal_size3 = 88;
    uint32_t third_malloc_result = vs_malloc(mem, mal_size3);
    printf("Trying to malloc %u bytes\n", mal_size3);
    printf("Address of third malloc %u\n", (third_malloc_result - kernel_virtual_size));

    uint32_t mal_size4 = 89;
    uint32_t fourth_malloc_result = vs_malloc(mem, mal_size4);
    printf("Trying to malloc %u bytes\n", mal_size4);
    printf("Address of fourth malloc %u\n", (fourth_malloc_result - kernel_virtual_size));

    uint32_t mal_size5 = 24;
    uint32_t fifth_malloc_result = vs_malloc(mem, mal_size5);
    printf("Trying to malloc %u bytes\n", mal_size5);
    printf("Address of fifth malloc %u\n", (fifth_malloc_result - kernel_virtual_size));
    // uint32_t res2 = vs_malloc(mem, 25);

    

    // printf("Address of second malloc %u\n", res2);

    // printf("The testing main does nothing anymore lol. We can change that if need be.\n");

    return 0;
}
