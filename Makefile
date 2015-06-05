CC=g++
CFLAGS=-Wall
BIN=test2
OBJS= test2.o EventLoop.o Thread.o Logging.o LogStream.o Timestamp.o
LIB=-lcrypt -pthread
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN)
