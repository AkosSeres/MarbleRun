mkdir dest obj
emcc -c src/main.cpp -o obj/main.o -I include -s USE_SDL=2
emcc -O3 obj/main.o -o dest/index.html --shell-file index.html -s USE_SDL=2 -s FORCE_FILESYSTEM=1 --preload-file base_scene.obj
