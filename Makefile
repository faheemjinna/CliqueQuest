# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -O2

# Source and object files
SRCS = dict.c
OBJS = dict.o

# Target executable
TARGET = dict

# Default rule to build the target
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Rule to compile the source files into object files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJS) $(TARGET) output.out

# PHONY targets (not associated with actual files)
.PHONY: all clean