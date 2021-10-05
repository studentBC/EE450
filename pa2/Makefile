PP = g++
srcs = crc_rx.cpp crc_tx.cpp crc_vs_checksum.cpp
objs = $(srcs:.cpp=.o)
version=c++14

all:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
	$(PP) -std=$(version) -o client.out client.cpp
servermain:
	$(PP) -std=$(version) -o servermain.out servermain.cpp 
client:
	$(PP) -std=$(version) -o client.out client.cpp

clear:
	rm *.out
