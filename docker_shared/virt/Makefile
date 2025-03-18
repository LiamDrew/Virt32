
# I am a little opinionated about this, but I'm open to having my mind changed:
# I want to use the clang compiler because it's natively supported by MacOS,
# which does all its low level stuff with the LLVM toolchain.

CC = clang
CFLAGS = -g -Wall -Wextra -Werror -Wpedantic -O2

um: emulator.o driver.o recycler.o mem_state.o stack.o
	$(CC) $(CFLAGS) -o um emulator.o driver.o recycler.o mem_state.o stack.o

all: virt test

test: test_stack test_recycler test_mem

virt: main.o driver.o recycler.o mem_state.o stack.o
	$(CC) $(CFLAGS) -o virt main.o driver.o recycler.o mem_state.o stack.o

test_stack: stack_test.o stack.o
	$(CC) $(CFLAGS) -o test_stack stack_test.o stack.o

test_recycler: recycler_test.o recycler.o stack.o mem_state.o driver.o
	$(CC) $(CFLAGS) -o test_recycler recycler_test.o recycler.o stack.o mem_state.o driver.o

test_mem: mem_test.o driver.o mem_state.o recycler.o stack.o
	$(CC) $(CFLAGS) -o test_mem mem_test.o driver.o mem_state.o recycler.o stack.o

emulator.o: emulator.c
	$(CC) $(CFLAGS) -c emulator.c

mem_test.o: mem_test.c
	$(CC) $(CFLAGS) -c mem_test.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

stack_test.o: stack_test.c
	$(CC) $(CFLAGS) -c stack_test.c

driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

recycler.o: recycler.c
	$(CC) $(CFLAGS) -c recycler.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

mem_state.o: mem_state.c
	$(CC) $(CFLAGS) -c mem_state.c

clean:
	rm -f *.o virt test_stack test_recycler test_mem

testall:
	./test_stack 
	./test_recycler


