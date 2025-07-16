# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra
OMP_FLAGS = -fopenmp
MPI_FLAGS = -lmpi

# Directories
SEQ_DIR = Sequential
OMP_DIR = OpenMP
MPI_DIR = MPI

# Target executables
SEQ_EXEC = $(SEQ_DIR)/SqMV
OMP_EXEC = $(OMP_DIR)/SqMV_OpenMP
MPI_EXEC = $(MPI_DIR)/SqMV_MPI.exe

# Source files
SEQ_SRC = $(SEQ_DIR)/SqMV.c
OMP_SRC = $(OMP_DIR)/SqMV_OpenMP.c
MPI_SRC = $(MPI_DIR)/SqMV_MPI.c

# Default target
all: sequential openmp mpi

# Build sequential version
sequential: $(SEQ_SRC)
	$(CC) $(CFLAGS) -o $(SEQ_EXEC) $(SEQ_SRC)

# Build OpenMP version
openmp: $(OMP_SRC)
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o $(OMP_EXEC) $(OMP_SRC)

# Build MPI version
mpi: $(MPI_SRC)
	cd $(MPI_DIR) && mpicc -o SqMV_MPI.exe SqMV_MPI.c

# Clean build files
clean:
	rm -f $(SEQ_EXEC) $(OMP_EXEC) $(MPI_DIR)/SqMV_MPI.exe

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build all versions (sequential, OpenMP, MPI)"
	@echo "  sequential - Build only the sequential version"
	@echo "  openmp     - Build only the OpenMP version"
	@echo "  mpi        - Build only the MPI version"
	@echo "  clean      - Remove all built executables"
	@echo "  help       - Show this help message"

.PHONY: all sequential openmp mpi clean help