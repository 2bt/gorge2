CF = `$(PKG)` -std=c++17 -Wall -Og -g
LF = -Wall `$(PKG)`
PKG = pkg-config --cflags --libs sdl2 SDL2_image glew gl

CXX = g++
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=obj/%.o)
TRG = gorge

all: $(TRG)


obj/%.o: src/%.cpp Makefile
	@mkdir -p obj/
	$(CXX) $(CF) $< -c -o $@


$(TRG): $(OBJ) Makefile
	$(CXX) $(OBJ) -o $@ $(LF)


clean:
	rm -rf obj/ $(TRG)
