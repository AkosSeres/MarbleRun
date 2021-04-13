mkdir dest obj
emcc -c src/main.cpp -o obj/main.o -I include -s USE_SDL=2
emcc -c src/Shaders.cpp -o obj/Shaders.o -I include -s USE_SDL=2
emcc -c src/SDLInstance.cpp -o obj/SDLInstance.o -I include -s USE_SDL=2
emcc obj/main.o obj/Shaders.o obj/SDLInstance.o -o dest/index.html --shell-file index.html -s USE_SDL=2 -s FORCE_FILESYSTEM=1 --preload-file base_scene.obj
