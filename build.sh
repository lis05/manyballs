gcc -O3 -mavx2 `find . -type f -name '*.c'` -o main `sdl2-config --libs` -lSDL2_ttf -lm &&
./main