SRC = $(wildcard src/*.c)
OBJ = $(patsubst src%,out%,$(SRC:.c=.o))
INCLUDE = -Isrc
LDFLAGS = -lm

bin/hasm: $(OBJ)
	gcc $(CFLAGS) $(LDFLAGS) -o bin/hasm $^

out/%.o: src/%.c
	gcc -std=c99 -g3 $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

install:
	cp bin/hasm $(PREFIX)/bin
