# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra
OMP_FLAGS = -fopenmp

# Directories
SEQ_DIR = Sequential
OMP_DIR = OpenMP

# Target executables
SEQ_EXEC = $(SEQ_DIR)/SqMV
OMP_EXEC = $(OMP_DIR)/SqMV_OpenMP

# Source files
SEQ_SRC = $(SEQ_DIR)/SqMV.c
OMP_SRC = $(OMP_DIR)/SqMV_OpenMP.c

# Default target
all: sequential openmp

# Build sequential version
sequential: $(SEQ_SRC)
	$(CC) $(CFLAGS) -o $(SEQ_EXEC) $(SEQ_SRC)

# Build OpenMP version
openmp: $(OMP_SRC)
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o $(OMP_EXEC) $(OMP_SRC)

# Clean build files
clean:
	rm -f $(SEQ_EXEC) $(OMP_EXEC)

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build both sequential and OpenMP versions"
	@echo "  sequential - Build only the sequential version"
	@echo "  openmp     - Build only the OpenMP version"
	@echo "  clean      - Remove all built executables"
	@echo "  help       - Show this help message"

.PHONY: all sequential openmp clean help