# Automatic OS Detection
UNAME_S := $(shell uname -s)

# Common Variables
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCLUDES = -Iinclude
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
EXECUTABLE = $(BINDIR)/sparse_matrix_mult

# OS-Specific Flags
ifeq ($(UNAME_S), Linux)
    CC = mpicc
    CFLAGS = -Wall -Wextra -O3 -fopenmp -march=native -ffast-math
    LDFLAGS = -fopenmp
else ifeq ($(UNAME_S), Darwin)
    CC = mpicc
    CFLAGS = -Wall -Wextra -O3 -Xpreprocessor -fopenmp -I/opt/homebrew/Cellar/libomp/18.1.8/include
    LDFLAGS = -L/opt/homebrew/Cellar/libomp/18.1.8/lib -lomp
endif

# Add MPI-specific flags (optional, if necessary)
MPIFLAGS = -D_MPI

.PHONY: all clean

# Default target to build everything
all: $(EXECUTABLE)

# Linking the executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(MPIFLAGS) $^ -o $@ $(LDFLAGS)

# Compiling each object
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(MPIFLAGS) -c $< -o $@

# Create directories if they do not exist
$(OBJDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

# Clean up build files
clear:
	rm -rf $(OBJDIR) $(BINDIR)

# Dependency rules
$(OBJDIR)/main.o: $(SRCDIR)/main.c include/matrix_generation.h include/matrix_operations.h include/utils.h
$(OBJDIR)/matrix_generation.o: $(SRCDIR)/matrix_generation.c include/matrix_generation.h include/utils.h
$(OBJDIR)/matrix_operations.o: $(SRCDIR)/matrix_operations.c include/matrix_operations.h include/utils.h
$(OBJDIR)/utils.o: $(SRCDIR)/utils.c include/utils.h