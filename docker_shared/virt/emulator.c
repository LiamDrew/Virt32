/**
 * @file main.c
 * @author Liam Drew
 * @date January 2025
 * @brief
 * This program implements a Universal Machine (UM) emulator. The UM is an
 * extremely simple virtual machine. For more information about the UM
 * specification, please see the project README.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

#include "driver.h"
#include "mem_state.h"

#define NUM_REGISTERS 8
#define POWER ((uint64_t)1 << 32) // for preventing overflow with add and div
typedef uint32_t Instruction;

void initialize_memory(FILE *fp, size_t fsize, uint8_t *umem);
uint64_t assemble_word(uint64_t word, unsigned width, unsigned lsb,
                       uint64_t value);

void handle_instructions(uint8_t *mem);
static inline bool exec_instr(Instruction word, 
                              uint32_t *regs, uint8_t *umem, 
                              uint32_t *pc);
uint32_t map_segment(uint32_t size);
void unmap_segment(uint32_t segmentm);
void load_segment(uint32_t index, uint8_t *umem);

/* In order to be properly fast, set_at() and get_at() need to be inlined by
 * the compiler. These functions are defined in the driver module, and I don't
 * know enough C to know if that is possible. So I am going to define those
 * functions inline here for now, and figure it out later. */

/* We can consider having these defined as globals, but they are needed so much
 * I think it makes sense to make them local variables */

// static uint8_t *umem = NULL;
// extern uint8_t *usable;

static inline void *my_convert_address(uint8_t *umem, uint32_t addr)
{
    // void *out = umem + addr;
    return umem + addr;
}


static inline void set_at(uint8_t *umem, uint32_t addr, uint32_t value)
{
    // convert v^2 address to virtual address
    uint32_t *dest = (uint32_t *)(umem + addr);
    *dest = value;
}

static inline uint32_t get_at(uint8_t *umem, uint32_t addr)
{
    // convert v^2 address to virtual address
    uint32_t *src = (uint32_t *)(umem + addr);
    return *src;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./um [executable.um]\n");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        fprintf(stderr, "File %s could not be opened.\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* NOTE: For some reason, this is returning that the size of the file is 4
     * bytes larger than we think it should be it.*/
    size_t fsize = 0;
    struct stat file_stat;
    if (stat(argv[1], &file_stat) == 0)
        fsize = file_stat.st_size;

    uint32_t kern_size = 524288; // this is 2^19
    Mem_T *mem = init_memory_system(kern_size);
    uint8_t *umem = mem->usable_mem;

    initialize_memory(fp, fsize + sizeof(Instruction), umem);
    
    handle_instructions(umem);

    terminate_memory_system();
    
    return EXIT_SUCCESS;
}

void initialize_memory(FILE *fp, size_t fsize, uint8_t *umem)
{
    /* NOTE: Here, fsize is already adjusted to account for the fact that each
     * UM instruction is 4 bytes. Future allocations will have to take this into
     * account. */
    kern_recalloc(fsize);
    uint32_t word = 0;
    int c;
    int i = 0;
    unsigned char c_char;

    for (c = getc(fp); c != EOF; c = getc(fp))
    {
        c_char = (unsigned char)c;
        if (i % 4 == 0)
            word = assemble_word(word, 8, 24, c_char);
        else if (i % 4 == 1)
            word = assemble_word(word, 8, 16, c_char);
        else if (i % 4 == 2)
            word = assemble_word(word, 8, 8, c_char);
        else if (i % 4 == 3)
        {
            word = assemble_word(word, 8, 0, c_char);

            // storing in the zero segment here
            set_at(umem, 0 + (i / 4) * sizeof(uint32_t), word);
            word = 0;
        }

        i++;
    }

    fclose(fp);
}

uint64_t assemble_word(uint64_t word, unsigned width, unsigned lsb,
                       uint64_t value)
{
    uint64_t mask = (uint64_t)1 << (width - 1);
    mask = mask << 1;
    mask -= 1;
    mask = mask << lsb;
    mask = ~mask;

    uint64_t new_word = (word & mask);
    value = value << lsb;
    uint64_t return_word = (new_word | value);
    return return_word;
}

void handle_instructions(uint8_t *umem)
{
    uint32_t regs[NUM_REGISTERS] = {0};
    uint32_t pc = 0;
    Instruction word;

    bool exit = false;

    while (!exit)
    {
        word = get_at(umem, 0 + pc * sizeof(uint32_t));
        // uint32_t opcode = word >> 28;
        // printf("Getting word at segment 0 at index %u and opcode is %u\n", pc, opcode);
        pc++;
        exit = exec_instr(word, regs, umem, &pc);
    }
}

void print_registers(uint32_t *regs)
{
    printf("\n______\n");
    for (int i = 0; i < 8; i++) {
        printf("Register %d is %u\n", i, regs[i]);
    }
    printf("______\n");
}

static inline bool exec_instr(Instruction word, uint32_t *regs, uint8_t *umem, 
                              uint32_t *pc)
{
    uint32_t a = 0, b = 0, c = 0, val = 0;
    uint32_t opcode = word >> 28;

    // printf("Opcode is %u\n", opcode);

    /* Load Value */
    if (__builtin_expect(opcode == 13, 1))
    {
        a = (word >> 25) & 0x7;
        val = word & 0x1FFFFFF;
        regs[a] = val;
        // fprintf(stderr, "Load Value\n");
        return false;
    }

    c = word & 0x7;
    b = (word >> 3) & 0x7;
    a = (word >> 6) & 0x7;

    /* Segmented Load */
    if (__builtin_expect(opcode == 1, 1))
    {
        regs[a] = get_at(umem, regs[b] + regs[c] * sizeof(uint32_t));
        // regs[a] = get_at(mem, regs[b], regs[c] * sizeof(uint32_t));
        // fprintf(stderr, "Segmented Load\n");
    }

    /* Segmented Store */
    else if (__builtin_expect(opcode == 2, 1))
    {
        set_at(umem, regs[a] + regs[b] * sizeof(uint32_t), regs[c]);
        // set_at(mem, regs[a], regs[b] * sizeof(uint32_t), regs[c]);
        // fprintf(stderr, "Segmented Store\n");
    }

    /* Bitwise NAND */
    else if (__builtin_expect(opcode == 6, 1))
    {
        regs[a] = ~(regs[b] & regs[c]);
        // fprintf(stderr, "Bitwise NAND\n");
    }

    /* Load Segment */
    else if (__builtin_expect(opcode == 12, 0))
    {
        // printf("Loading segment, regs[c] is: %u\n", regs[c]);
        // load_segment(regs[b], regs, c);
        load_segment(regs[b], umem);
        *pc = regs[c];      // intentionally not multiplying
        // fprintf(stderr, "Load Segment\n");
    }

    /* Addition */
    else if (__builtin_expect(opcode == 3, 0))
    {
        regs[a] = (regs[b] + regs[c]) % POWER;
    }

    /* Conditional Move */
    else if (__builtin_expect(opcode == 0, 0))
    {
        if (regs[c] != 0)
            regs[a] = regs[b];
        // fprintf(stderr, "Conditional Move\n");
    }

    /* Map Segment */
    else if (__builtin_expect(opcode == 8, 0))
    {
        regs[b] = map_segment(regs[c]);
        // fprintf(stderr, "Map Segment\n");
    }

    /* Unmap Segment */
    else if (__builtin_expect(opcode == 9, 0))
    {
        unmap_segment(regs[c]);
        // fprintf(stderr, "Unmap Segment\n");
    }

    /* Division */
    else if (__builtin_expect(opcode == 5, 0))
    {
        regs[a] = regs[b] / regs[c];
        // fprintf(stderr, "Division\n");
    }

    /* Multiplication */
    else if (__builtin_expect(opcode == 4, 0))
    {
        regs[a] = (regs[b] * regs[c]) % POWER;
        // fprintf(stderr, "Multiplication\n");
    }

    /* Output */
    else if (__builtin_expect(opcode == 10, 0))
    {
        putchar((unsigned char)regs[c]);
        // fprintf(stderr, "Output\n");
    }

    /* Input */
    else if (__builtin_expect(opcode == 11, 0))
    {
        regs[c] = getc(stdin);
        // fprintf(stderr, "Input\n");
    }
    /* Stop or Invalid Instruction */
    else
    {
        // fprintf(stderr, "Stopcode: %d\n", opcode);
        return true;
    }

    return false;
}

uint32_t map_segment(uint32_t size)
{
    return vs_calloc(size * sizeof(uint32_t));
}

void unmap_segment(uint32_t segment)
{
    (void)segment;
    
    /* NOTE: The recycler currently segfaults and is completely unusable
     * I suspect this has to do more with it's usage/integration than with it's
     * implementation in the first place. */
    // vs_free(mem, segment);
}

void load_segment(uint32_t index, uint8_t *umem)
{
    // Return immediately if loading elsewhere in the zero segment
    if (index == 0) return;

    // NOTE: We need to get this right to get sandmark to run
    // assert(false);

    /* I am intentionally leaving this broken. I need to do a much more careful
     * job of going through and making sure that I'm changing interfaces to be
     * what I want*/

    // added for debugging
    //print_registers(regs);
    //printf("Index here is %u\n", regs[c]);

    // get the size of the segment
    // printf("Seg addr is %p\n", seg_addr);
    void *seg_addr = my_convert_address(umem, index);
    // void *seg_addr = NULL; // need a better plan here
    uint32_t *usable_kern_addr = (uint32_t *)my_convert_address(umem, 0);
    // uint32_t *usable_kern_addr = NULL;
    // get segment size
    uint32_t *my_addr = seg_addr;
    uint32_t copy_size = my_addr[-1];

    kern_recalloc(copy_size);

    // TODO: Need to correctly duplicate the segment we want into the kernel space

    // printf("Copying segment of size %u\n", copy_size);
    uint32_t loop_size = copy_size / 4;
    for (uint32_t i = 0; i < loop_size; i++) {
        uint32_t my_temp = my_addr[i];
        uint32_t temp_opcode = my_temp >> 28;
        (void)temp_opcode;

        // printf("Getting word at segment 0 at index %u with opcode: %u\n", i, temp_opcode);
        usable_kern_addr[i] = my_addr[i];
    }
}
