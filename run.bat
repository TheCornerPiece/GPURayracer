Rem gcc -Iinclude main.c src/glad.c -o main.exe -lglfw3 -lopengl32 -lgdi32 -lmingw32 -lchipmunk

gcc -Iinclude -c src/glad.c -o glad.o
gcc -Iinclude -c main.c -o main.o
gcc main.o glad.o chipmunk.lib -o main.exe -lopengl32 -lglfw3 -lgdi32 -lmingw32 -lchipmunk -L.

PAUSE