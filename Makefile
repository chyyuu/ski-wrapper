CXXFLAGS=-Wall -std=c++0x -g

.PHONY: clean all

all: dump-ana pretty env

dump-ana: sympp.o dump-ana.o
	$(CXX) $(CXXFLAGS) -o dump-ana dump-ana.o sympp.o

pretty: sympp.o main.o
	$(CXX) $(CXXFLAGS) -o pretty main.o sympp.o

sympp.o: sympp.h

dump-ana.o: sympp.h

main.o: sympp.h

clean:
	rm -rf *.o dump-ana pretty main env