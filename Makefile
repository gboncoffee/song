CC = clang
CFLAGS = -g -Wall -Wextra -pedantic -std=c99
CLIBS = -lX11 -lGL -lm -lpthread -ldl
RAYLIB = /usr/local/lib/libraylib.a

build/song: build/main.o build/channels.o build/music_theory.o build/player.o
	$(CC) $(CFLAGS) $(CLIBS) $^ $(RAYLIB) -o $@

build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm build/*.o
	-rm build/song
