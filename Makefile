LIBS := sdl2 SDL2_image glew gl
CF   := -std=c++17 -Wall -Og -g $(shell pkg-config --cflags $(LIBS))
LF   := -Wall $(shell pkg-config --libs $(LIBS))

CXX  := g++
SRC  := $(wildcard src/*.cpp)
OBJ  := $(SRC:src/%.cpp=obj/%.o)
TRG  := gorge


all: $(TRG)


obj/%.o: src/%.cpp Makefile
	@mkdir -p obj/
	$(CXX) $(CF) $< -c -o $@


$(TRG): $(OBJ) Makefile
	$(CXX) $(OBJ) -o $@ $(LF)


clean:
	rm -rf obj/ $(TRG)
