
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/local/include/farixEngine `sdl2-config --cflags`
LDFLAGS = -L/usr/local/lib -lfarixEngine -lSDL2_ttf `sdl2-config --libs`

# Source files and output binary
SRC = main.cpp game/game.cpp
BIN = ~/Builds/CppProjectBuild/game

# Default target
all: $(BIN)

# Link the executable
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
	@echo "✅ Built game: $@"

# Clean build artifacts
clean:
	rm -f $(BIN)

.PHONY: all clean

