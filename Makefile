CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm

TARGET = algoritmo_genetico

SRC = main.c genetico.c instancia.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)