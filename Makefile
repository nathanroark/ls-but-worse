# Makefile

# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -g
# Source file
SRC = src/ls.c
# Target executable
TARGET = ls
# Directory parameter
DIR = .

# Build and run target with parameter
run: $(TARGET)
	./$(TARGET) $(DIR)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean target
clean:
	rm -f $(TARGET)
