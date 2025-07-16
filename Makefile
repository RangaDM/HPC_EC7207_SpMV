# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra
OMP_FLAGS = -fopenmp
MPI_FLAGS = -lmpi

# Directories
SEQ_DIR = Sequential
OMP_DIR = OpenMP
MPI_DIR = MPI
HYBRID_DIR = Hybrid\ MPI\ +\ OpenMP

# Target executables
SEQ_EXEC = $(SEQ_DIR)/SqMV.exe
OMP_EXEC = $(OMP_DIR)/SqMV_OpenMP.exe
MPI_EXEC = $(MPI_DIR)/SqMV_MPI.exe
HYBRID_EXEC = $(HYBRID_DIR)/SqMV_Hybrid.exe

# Source files
SEQ_SRC = $(SEQ_DIR)/SqMV.c
OMP_SRC = $(OMP_DIR)/SqMV_OpenMP.c
MPI_SRC = $(MPI_DIR)/SqMV_MPI.c
HYBRID_SRC = $(HYBRID_DIR)/SqMV_Hybrid.c

# Default target
all: sequential openmp mpi hybrid

# Build sequential version
sequential: $(SEQ_SRC)
	$(CC) $(CFLAGS) -o $(SEQ_EXEC) $(SEQ_SRC)

# Build OpenMP version
openmp: $(OMP_SRC)
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o $(OMP_EXEC) $(OMP_SRC)

# Build MPI version
mpi: $(MPI_SRC)
	cd $(MPI_DIR) && mpicc -o SqMV_MPI.exe SqMV_MPI.c

# Build Hybrid MPI+OpenMP version
hybrid: $(HYBRID_SRC)
	cd $(HYBRID_DIR) && mpicc -fopenmp -o SqMV_Hybrid.exe SqMV_Hybrid.c

# Clean build files
clean:
	rm -f $(SEQ_EXEC) $(OMP_EXEC) $(MPI_EXEC) $(HYBRID_EXEC)

# Time comparison target
# timecompare: all
# 	@echo "================== Time Comparison =================="
# 	@echo "Sequential:"
# 	@printf "2\n2\n100\n" | /usr/bin/time -f "Time: %e seconds" ./$(SEQ_EXEC)
# 	@echo "----------------------------------------------------"
# 	@echo "OpenMP:"
# 	@printf "2\n2\n100\n" | /usr/bin/time -f "Time: %e seconds" ./$(OMP_EXEC)
# 	@echo "----------------------------------------------------"
# 	@echo "MPI:"
# 	@printf "2\n100\n" | mpirun -np 4 ./$(MPI_EXEC)
# 	@echo "----------------------------------------------------"
# 	@echo "Hybrid MPI + OpenMP:"
# 	@printf "2\n100\n" | mpirun -np 4 ./$(HYBRID_EXEC)
# 	@echo "===================================================="


# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build all versions (sequential, OpenMP, MPI, Hybrid)"
	@echo "  sequential - Build only the sequential version"
	@echo "  openmp     - Build only the OpenMP version"
	@echo "  mpi        - Build only the MPI version"
	@echo "  hybrid     - Build only the Hybrid MPI+OpenMP version"
	@echo "  clean      - Remove all built executables"
	@echo "  help       - Show this help message"

.PHONY: all sequential openmp mpi hybrid clean help