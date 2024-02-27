mkdir -p bin

# if [ $OSTYPE == "msys" ]; then
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if clang++ src/*.cc src/*.c -Iinc -Llib -lSDL2 -obin/3dpiv -Wno-deprecated; then 
		echo "Build succeeded";
		exit 0;
	else 
		echo "Build failed"; 
		exit 1;
	fi
else
	if clang++ src/*.cc src/backend/*.cc src/cv/*.cc src/imgui/*.cpp src/backend/*.c src/imgui/backends/imgui_impl_opengl3.cpp src/imgui/backends/imgui_impl_sdl.cpp -Iinc -Isrc/imgui -Llib -lSDL2 -obin/3dpiv.exe -Wno-deprecated; then 
		echo "Build succeeded";
		exit 0;
	else 
		echo "Build failed"; 
		exit 1;
	fi
fi




