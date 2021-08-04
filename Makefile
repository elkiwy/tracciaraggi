#Folders
SRC := src
OBJ := obj
BUILD := build
EXECUTABLE := $(BUILD)/tracciaraggi

#Compiler
CC = g++-10

#Source files and objects
SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst %.cpp, $(OBJ)/%.o, $(notdir $(SOURCES)))

#Libs
LIBSDL = -l SDL2-2.0.0 -l SDL2_ttf -l SDL2_image -l SDL2_gfx
LIBS = -L /usr/local/lib -fopenmp $(LIBSDL) -I/usr/local/include

#Flags
WARNS = -Wall
FLAGS = -g -std=c++17 $(WARNS) #-fsanitize=address -fsanitize=undefined



#############################

#Main target
$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ $(FLAGS) $(LIBS) -o $@

#Objects
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(FLAGS) $(LIBS) $(INCL) -c $< -o $@



#############################

#Clean target
clean:
	rm -f $(EXECUTABLE) && rm -f $(OBJ)/*.o

#Run build
run: $(EXECUTABLE)
	./$(EXECUTABLE)
