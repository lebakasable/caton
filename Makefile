CFLAGS=-Wall -Wextra -pedantic

all: rule110

rule110: rule110.c
	@$(CC) $(CFLAGS) -o $@ $^
