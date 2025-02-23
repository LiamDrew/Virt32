#include <stdio.h>
#include <stdint.h>
#include "stack.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Stack Tests */
    Stack *s = stack_init(10);
    stack_push(s, 1);
    stack_push(s, 2);
    stack_push(s, 3);
    stack_push(s, 4);
    stack_push(s, 5);
    stack_push(s, 6);
    stack_push(s, 7);
    stack_push(s, 8);
    stack_push(s, 9);
    stack_push(s, 10);
    stack_push(s, 11);
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));
    printf("top: %d\n", stack_pop(s));

    stack_free(s);
}
