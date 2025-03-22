#include "stack.h"
#include <assert.h>
#include <stdlib.h>

/* stack_expand() is private to the stack module. */
// void stack_expand(Stack_T* s);

// void stack_expand(Stack_T s);

/* My current plan to make the stack more performant it to make all the
 * functions take in a copy of the stack struct instead of a pointer to it
 * allocated on the heap. The heap allocation is definitely slowing things down
 */

// Stack_T* stack_init(uint32_t size)
// {
//     assert(size > 0);
//     Stack_T* s = malloc(sizeof(Stack_T));
//     s->stack = malloc(size * sizeof(uint32_t));
//     s->capacity = size;
//     s->size = 0;

//     return s;
// }

// uint32_t stack_top(Stack_T *s)
// {
//     assert(s->size > 0);
//     return s->stack[s->size - 1];
// }

// void stack_push(Stack_T *s, uint32_t elem)
// {
//     // assert(s != NULL);
//     if (s->size == s->capacity) {
//         stack_expand(s);
//     }

//     s->stack[s->size++] = elem;
// }

// void stack_expand(Stack_T* s)
// {
//     s->capacity = s->capacity * 2;
//     uint32_t* temp = malloc(s->capacity * sizeof(uint32_t));
//     for (uint32_t i = 0; i < s->size; i++) {
//         temp[i] = s->stack[i];
//     }

//     free(s->stack);
//     s->stack = temp;
// }

// uint32_t stack_pop(Stack_T* s)
// {
//     // assert(s->size > 0);
//     uint32_t top = s->stack[--s->size];
//     return top;
// }

// bool stack_is_empty(Stack_T* s)
// {
//     return s->size == 0;
// }

// void stack_free(Stack_T* s)
// {
//     free(s->stack);
//     free(s);
// }

// // void stack_expand(Stack_T *s)
// // {
// //     s->capacity = s->capacity * 2;
// //     uint32_t *temp = malloc(s->capacity * sizeof(uint32_t));
// //     for (uint32_t i = 0; i < s->size; i++)
// //     {
// //         temp[i] = s->stack[i];
// //     }

// //     free(s->stack);
// //     s->stack = temp;
// // }

// inline Stack_T stack_init(uint32_t size)
// {
//     assert(size > 0);

//     Stack_T s;
//     s.stack = malloc(size * sizeof(uint32_t));
//     s.capacity = size;
//     s.size = 0;

//     return s;
// }

// inline uint32_t stack_top(Stack_T s)
// {
//     assert(s.size > 0);
//     return s.stack[s.size - 1];
// }

// inline Stack_T stack_push(Stack_T s, uint32_t elem)
// {
//     if (s.size == s.capacity)
//     {
//         s.capacity *= 2;
//         uint32_t *temp = malloc(s.capacity * sizeof(uint32_t));
//         for (uint32_t i = 0; i < s.size; i++)
//         {
//             temp[i] = s.stack[i];
//         }

//         free(s.stack);
//         s.stack = temp;
//     }

//     s.stack[s.size++] = elem;
//     return s;
// }

// inline uint32_t stack_pop(Stack_T *s)
// {
//     // assert(s->size > 0);
//     uint32_t top = s->stack[--s->size];
//     return top;
// }

// inline bool stack_is_empty(Stack_T s)
// {
//     return s.size == 0;
// }

// inline void stack_free(Stack_T *s)
// {
//     free(s->stack);
//     free(s);
// }
