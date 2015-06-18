CC=g++
CFLAGS=-Wall -g
BIN=test
OBJS= test.o EventLoop.o Thread.o Logging.o LogStream.o Timestamp.o Channel.o Poller.o Timer.o TimerQueue.o EventLoopThread.o Acceptor.o InetAddress.o Socket.o SocketsOps.o TcpServer.o TcpConnection.o Buffer.o EPoller.o
LIB=-lcrypt -pthread
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN) a.out
