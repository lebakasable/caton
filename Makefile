CFLAGS=-Wall -Wextra -pedantic

all: viz rule110

viz: viz.c
	@$(CC) $(CFLAGS) -o $@ $^ -lglfw -lGL

rule110: rule110.c
	@$(CC) $(CFLAGS) -o $@ $^
