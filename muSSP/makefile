# change these to suit your system
CCOMP = g++ -std=c++14
#CCOMP = gcc-4
#CFLAGS = -O4 -DNDEBUG
#CFLAGS = -g -DCHECK_SOLUTION -Wall
CFLAGS = -g -Wall
#CFLAGS = -O4 -DNDEBUG -DNO_ZERO_CYCLES
BIN=muSSP

pesudo_sst: main.o Graph.o Node.o Sink.o
	g++ main.o Graph.o Node.o Sink.o -DPRINT_ANS -DCOMP_DUALS -DCOST_RESTART -o $(BIN)

main.o: main.cpp
	$(CCOMP) -c main.cpp

Graph.o: Graph.cpp
	$(CCOMP) -c Graph.cpp

Node.o: Node.cpp
	$(CCOMP) -c Node.cpp

Sink.o: Sink.cpp
	$(CCOMP) -c Sink.cpp


clean:
	rm -f $(BIN) *~
