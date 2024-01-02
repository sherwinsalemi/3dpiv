if g++ src/*.cpp -I/usr/include/opencv4 -luvc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -obin/3dpiv; then 
echo "Build succeeded";
sudo bin/3dpiv
else 
echo "Build failed"; 
fi
