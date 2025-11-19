CC = gcc
CFLAGS = -Wall -g -s
SRCS = a8.c dijkstra.c
OBJS = $(SRCS:.c=.o)
TARGET = a8
	
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)