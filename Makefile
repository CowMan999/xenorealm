CXX= ccache g++
CC= gcc
CXXFLAGS= -std=c++20 -Wall -Wextra -Werror -O3 -DSFML_STATIC -Wpedantic -DTAGLIB_STATIC -s -mwindows
CFLAGS= -std=c99 -Wall -Wextra -Werror -s -Ofast -mwindows
CXXLIBS= -lsfcollider -limgui-SFML -limgui -lsfml-graphics-s -lsfml-window-s -lopengl32 -ljpeg -lfreetype -lgdi32 -lsfml-audio-s -lopenal32 -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg -lsfml-network-s -lws2_32 -lsfml-system-s -lwinmm 
CXXLIBPATHS= -L"C:\Your\Path\Here\SFML-2.6.x\extlibs\libs-mingw\x64" -L"CYour\Path\Here\My_Programs\SFML\SFML-2.6.x\staticbuild\lib" -L"C:\Your\Path\Here\My_Programs\Dear_ImGUI\lib"

SRC = $(wildcard *.cpp)
SRH = $(wildcard *.hpp)
OBJ = $(patsubst %.cpp, make/%.o, $(SRC))

FINAL= xrealm.exe

$(FINAL): $(OBJ)
	@windres icon.rc -o make/icon.o
	@echo "Linking $@"
	@$(CXX) $(CXXFLAGS) $(CXXLIBPATHS) $(OBJ) make/icon.o $(CXXLIBS) -o $(FINAL)

make/%.o: %.cpp $(SRC) Makefile $(SRH)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	@del .\make\*.o

