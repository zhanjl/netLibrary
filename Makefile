CC=g++
CFLAGS=-Wall -g
BIN=test6
OBJS= test6.o EventLoop.o Thread.o Logging.o LogStream.o Timestamp.o Channel.o Poller.o Timer.o TimerQueue.o EventLoopThread.o
LIB=-lcrypt -pthread
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN)
