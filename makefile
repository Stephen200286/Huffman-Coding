huf: main.o node.o
	g++ main.o node.o -fsanitize=address -o huf 

main: main.cpp
	g++ -c -std=c++11 main.cpp

node: node.cpp
	g++ -c -std=c++11 node.cpp

clean:
	rm *.o

