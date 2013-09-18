
CC     = gcc
CFLAGS = -Wall -ansi
SRC    = main.c
TARGET = bf2c

default: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean
clean:
	-rm -f *.exe *.stackdump *~
	-rm -f $(TARGET)

