CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

TARGET = server
LOG_DIR = ../code/log/
POOL_DIR = ../code/pool/
TIMER_DIR = ../code/timer/
HTTP_DIR = ../code/http/
SERVER_DIR = ../code/server/
BUFFER_DIR = ../code/buffer/
SUFFIX = *.cpp

OBJS = $(LOG_DIR)$(SUFFIX) $(POOL_DIR)$(SUFFIX) $(TIMER_DIR)$(SUFFIX) \
       $(HTTP_DIR)$(SUFFIX) $(SERVER_DIR)$(SUFFIX) \
       $(BUFFER_DIR)$(SUFFIX) ../code/main.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET)  -pthread -lmysqlclient

clean:
	rm -rf ../bin/$(OBJS) $(TARGET)




