all:
	g++ -std=c++11 -c gltbam.cpp -o gltbam.o
	g++ -std=c++11 -c shader.cpp -o shader.o
	g++ -std=c++11 -c image_figure.cpp -o image_figure.o
	g++ -std=c++11 -c test.cpp -o test.o
	g++ -std=c++11 gltbam.o shader.o image_figure.o test.o -lGL -lGLEW -lglfw -o test

clean:
	rm test *.o
