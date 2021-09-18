PP = g++
srcs = crc_rx.cpp crc_tx.cpp crc_vs_checksum.cpp
objs = $(srcs:.cpp=.o)


all: $(objs)
	$(PP) -c $(srcs)
	$(PP) -o crc_rx.out crc_rx.o
	$(PP) -o crc_tx.out crc_tx.o
	$(PP) -o crc_vs_checksum.out crc_vs_checksum.o
	./crc_rx.out
	./crc_tx.out
	./crc_vs_checksum.out

clear:
	rm *.o
