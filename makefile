all: Dissector

Dissector: ProtocolDissector.o main.o
	g++ ProtocolDissector.o main.o -o Dissector

main.o: main.cpp
	g++ -c main.cpp

ProtocolDissector.o: ProtocolDissector.hpp ProtocolDissector.cpp
	g++ -c ProtocolDissector.cpp

clean:
	rm -rf *.o