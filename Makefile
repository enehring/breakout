EXE = breakout
CC = gcc
CCFLAGS = -Wall

build:
	$(CC) $(CCFLAGS) -o $(EXE) main.c \
		-I/usr/local/include/SDL2 \
		-D_REENTRANT \
		-L/usr/local/lib \
		-Wl,-rpath,/usr/local/lib \
		-Wl,--enable-new-dtags -lSDL2

clean:
	rm $(EXE)
