mkdir dest obj
emcc -c src/main.cpp -o obj/main.o -I include -s USE_SDL=2
emcc -c src/Shaders.cpp -o obj/Shaders.o -I include -s USE_SDL=2
emcc -c src/Matrix.cpp -o obj/Matrix.o -I include -s USE_SDL=2
emcc -c src/SDLInstance.cpp -o obj/SDLInstance.o -I include -s USE_SDL=2
emcc -c src/Scene3D.cpp -o obj/Scene3D.o -I include -s USE_SDL=2
emcc -c src/Vec3.cpp -o obj/Vec3.o -I include -s USE_SDL=2
emcc -c src/Camera.cpp -o obj/Camera.o -I include -s USE_SDL=2
emcc obj/main.o obj/Shaders.o obj/SDLInstance.o obj/Scene3D.o obj/Matrix.o obj/Vec3.o obj/Camera.o -o dest/index.html --shell-file index.html -s USE_SDL=2 -s FORCE_FILESYSTEM=1 --preload-file base_scene.obj
