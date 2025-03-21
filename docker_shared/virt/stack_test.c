#include <stdio.h>
#include <stdint.h>
#include "stack.h"

void general_use(void);

int main(void)
{
    general_use();
}

/* Stack Tests */

/* General Use */
void general_use(void)
{
    Stack_T *s = stack_init(10);

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

/* Try to pop from empty */

/* Insert, remove all, insert more */

/* Insert 1000 elems */
