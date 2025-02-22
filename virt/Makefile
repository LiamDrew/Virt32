
# I am a little opinionated about this, but I'm open to having my mind changed:
# I want to use the clang compiler because it's natively supported by MacOS,
# which does all it's low level stuff with the LLVM toolchain.

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic # we can optimize later

virt: main.o driver.o recycler.o mem_state.o
	$(CC) $(CFLAGS) -o virt main.o recycler.o driver.o mem_state.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

recycler.o: recycler.c
	$(CC) $(CFLAGS) -c recycler.c

mem_state.o: mem_state.c
	$(CC) $(CFLAGS) -c mem_state.c

clean:
	rm -f *.o virt

