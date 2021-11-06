PP = g++
objs = $(srcs:.cpp=.o)
version=c++14

all:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
	$(PP) -std=$(version) -o serverA.out serverA.cpp
	$(PP) -std=$(version) -o serverB.out serverB.cpp
servermain:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
serverA:
	$(PP) -std=$(version) -o serverA.out serverA.cpp
serverB:
	$(PP) -std=$(version) -o serverB.out serverB.cpp

clear:
	rm *.out
