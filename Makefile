CC=g++
CFLAGS=-Wall
BIN=test3
OBJS= test3.o EventLoop.o Thread.o Logging.o LogStream.o Timestamp.o Channel.o Poller.o
LIB=-lcrypt -pthread
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN)
