PP = g++
srcs = crc_rx.cpp crc_tx.cpp crc_vs_checksum.cpp
objs = $(srcs:.cpp=.o)
version=c++14

all:
	$(PP) -std=$(version) -o crc_rx.out crc_rx.cpp 
	$(PP) -std=$(version) -o crc_tx.out crc_tx.cpp 
	$(PP) -std=$(version) -o crc_vs_checksum.out crc_vs_checksum.cpp
	./crc_rx.out
	./crc_tx.out
	./crc_vs_checksum.out

clear:
	rm *.out
