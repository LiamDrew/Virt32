#include <stdio.h>
#include "driver.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    init_memory_system(0);

    // vs_malloc



    printf("Hi there\n");
    return 0;
}
