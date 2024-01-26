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
	if clang++ src/*.cc src/*.c -Iinc -Llib -lSDL2 -obin/3dpiv.exe -Wno-deprecated; then 
		echo "Build succeeded";
		exit 0;
	else 
		echo "Build failed"; 
		exit 1;
	fi
fi




