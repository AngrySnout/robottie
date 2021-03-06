PLATFORM:= $(shell ./config.guess)

CC:=g++
CFLAGS:=-c -g -std=c++11 -ILuaBridge
LDFLAGS:=-lpthread -L/usr/lib -llua -ldl -lcurl
SOURCE_DIR:=src
SOURCE_FILES:=$(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS:=$(SOURCE_FILES:.cpp=.o)
BUILD_DIR:=bin
EXECUTABLE:=ircclient

all: $(SOURCE_FILES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(SOURCE_DIR)/*.o $(EXECUTABLE)

src/client.o: src/sock.h src/client.h
src/main.o: src/client.h src/tracker.h
src/sock.o: src/sock.h
