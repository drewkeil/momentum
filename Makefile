CXX = x86_64-w64-mingw32-g++
CXX ?= g++
C++FLAGS ?= -Wall -Werror -pedantic -O2 -static -DSFML_STATIC
LIBS = -L/usr/local/SFML/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lfreetype
INC = -I/usr/local/SFML/include

momentum: momentum.exe
	

momentum_s: momentum_s.exe
	

lvlBuild: lvlBuild.exe
	

lvlBuild_s: lvlBuild_s.exe
	

build: lvlBuild.exe momentum.exe
	./momentum.exe --building &
	./lvlBuild.exe &

obj/%.o: %.cpp
	mkdir -p obj
	$(CXX) $(C++FLAGS) $(INC) -c $< -o $@

momentum_s.exe: obj/momentum.o obj/gameObjects.o obj/level.o obj/gameMenu.o
	$(CXX) -O2 -static $^ -o momentum_s $(LIBS)

momentum.exe: obj/momentum.o obj/gameObjects.o obj/level.o obj/gameMenu.o
	$(CXX) -O2 $^ -o momentum $(LIBS)

lvlBuild.exe: obj/gameObjects.o obj/level.o obj/lvlBuild.o
	$(CXX) -O2 $^ -o lvlBuild $(LIBS)

lvlBuild_s.exe: obj/gameObjects.o obj/level.o obj/lvlBuild.o
	$(CXX) -O2 -static $^ -o lvlBuild_s $(LIBS)
 	
test: obj/test.o
	$(CXX) $(C++FLAGS) $^ -o test $(LIBS)

clean:
	rm -rf obj *.exe levels/lvltest
