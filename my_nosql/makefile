CC = g++
CXXFLAGS = -std=c++0x
CFLAGS = -I
skiplist:main.o
	$(CC) -o main main.o --std=c++11 -pthread 
	rm -f ./*.o
%.o: %.cpp
	$(CC) -c $@ $^
clean: 
	rm -f ./*.o