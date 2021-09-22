PP = g++
srcs = crc_rx.cpp crc_tx.cpp crc_vs_checksum.cpp
objs = $(srcs:.cpp=.o)
version=c++14

all: $(objs)
	$(PP) -c $(srcs)
	$(PP) -std=$(version) -o crc_rx.out crc_rx.o
	$(PP) -std=$(version) -o crc_tx.out crc_tx.o
	$(PP) -std=$(version) -o crc_vs_checksum.out crc_vs_checksum.o
	./crc_rx.out
	./crc_tx.out
	./crc_vs_checksum.out

clear:
	rm *.o
