# Makefile to compile main.cpp and create the executable 'main'

# The compiler and flags to use
CXX = c++
CXXFLAGS = 

# Name of source file
SRC = main.cpp

# The name of output binary
OUTPUT = main

# Default target 
all: $(OUTPUT)

# Rule to build the output
$(OUTPUT): $(SRC)
	$(CXX) $(SRC) -o $(OUTPUT)


clean:
	rm -f $(OUTPUT)