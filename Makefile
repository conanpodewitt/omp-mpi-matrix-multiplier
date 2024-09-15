CC = gcc
CFLAGS = -Wall -Wextra -O3 -fopenmp
INCLUDES = -Iinclude
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
EXECUTABLE = $(BINDIR)/sparse_matrix_mult

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Dependency rules
$(OBJDIR)/main.o: $(SRCDIR)/main.c include/matrix_generation.h include/matrix_operations.h include/utils.h
$(OBJDIR)/matrix_generation.o: $(SRCDIR)/matrix_generation.c include/matrix_generation.h include/utils.h
$(OBJDIR)/matrix_operations.o: $(SRCDIR)/matrix_operations.c include/matrix_operations.h include/utils.h
$(OBJDIR)/utils.o: $(SRCDIR)/utils.c include/utils.h