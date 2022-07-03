CFLAGS=-Wall -Wextra
CC=clang
EXECS=4-example syscall1 syscall2 fork compare change_bucket order

all: $(EXECS)

4-example: 4-example.c

syscall1: syscall_basic.c syscall1.c
	$(CC) $(CFLAGS) $> -o $@

syscall2: syscall_basic.c syscall2.c
	$(CC) $(CFLAGS) $> -o $@

fork: fork.c

compare: compare.c

change_bucket: change_bucket.c

order: order.c

.PHONY: clean

clean:
	rm -f $(EXECS)
