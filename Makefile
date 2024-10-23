all:
	gcc -I src/include -I H -L src/lib -o m C/main.c C/array.c C/display.c C/matrix.c C/mesh.c C/triangle.c C/vector.c -lmingw32 -lSDL2main -lSDL2