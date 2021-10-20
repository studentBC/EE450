PP = g++
objs = $(srcs:.cpp=.o)
version=c++14

all:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
	$(PP) -std=$(version) -o serverA.out serverA.cpp
	$(PP) -std=$(version) -o serverB.out serverB.cpp
servermain:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
client:
	$(PP) -std=$(version) -o client.out client.cpp

clear:
	rm *.out
