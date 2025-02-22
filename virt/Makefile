
# I am a little opinionated about this, but I'm open to having my mind changed:
# I want to use the clang compiler because it's natively supported by MacOS,
# which does all it's low level stuff with the LLVM toolchain.

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic # we can optimize later

virt: main.o driver.o recycler.o stack.o
	$(CC) $(CFLAGS) -o virt main.o recycler.o driver.o stack.o

main.o: main.c stack.c
	$(CC) $(CFLAGS) -c main.c stack.c

driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

recycler.o: recycler.c stack.c
	$(CC) $(CFLAGS) -c recycler.c stack.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

clean:
	rm -f *.o virt

