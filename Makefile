CXX = x86_64-w64-mingw32-g++
CXX ?= g++
C++FLAGS ?= -Wall -Werror -pedantic -O3 -static -DSFML_STATIC
LIBS = -L/usr/local/SFML/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32
LIBS ?= -L/usr/SFML-2.6.1/lib -lsfml-window -lsfml-graphics -lsfml-system
INC = -I/usr/local/SFML/include

momentum: momentum.exe
	

obj/%.o: %.cpp
	mkdir -p obj
	$(CXX) $(C++FLAGS)  -c $< -o $@ $(INC)

momentum.exe: obj/momentum.o obj/gameObjects.o obj/level.o
	$(CXX) $(C++FLAGS) $^ -o momentum $(LIBS)

test: obj/test.o
	$(CXX) $(C++FLAGS) test.o -o test $(LIBS)

clean:
	rm -rf obj *.exe
