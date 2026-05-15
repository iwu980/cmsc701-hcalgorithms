CFLAGS = -std=c++17 -I/"/Users/iwu/Documents/School/Spring 2026/CMSC 701" -I. -O3
all: greedyls
greedyls: greedyls.o LSTree.o
	g++ $(CFLAGS) -o greedyls greedyls.o LSTree.o
greedyls.o: greedyls.cpp
	g++ $(CFLAGS) -c greedyls.cpp -o greedyls.o
LSTree.o: LSTree.cpp
	g++ $(CFLAGS) -c LSTree.cpp -o LSTree.o
clean:
	rm -f greedyls greedyls.o LSTree.o