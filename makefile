# Makefile for Checkers Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Target executable name
TARGET = checkers

# Source files
SRCS = correct.c bitops.c bitops64.c

# Object files (compiled versions of each .c file)
OBJS = $(SRCS:.c=.o)

# Default rule: build the program
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile .c files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build with debug info (adds -g and disables optimization)
debug: CFLAGS += -g -O0
debug: clean $(TARGET)

# Clean up (remove compiled files)
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild from scratch
rebuild: clean all
