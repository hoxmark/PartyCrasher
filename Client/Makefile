CC		= gcc
FLAGS   = -Wall -O3
TARGET	= client.o
OBJS 	= ./algorithms/%.c ./counters/%.c network.c globals.h

all: $(TARGET)

.PHONY: .FORCE
$(TARGET): .FORCE
	$(CC) $(FLAGS) -o $(TARGET) nextwork.c

.PHONY: clean
clean:
	rm -f $(TARGET)
