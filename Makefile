PKGCONFIG:= $(shell which $(PLATFORM)-pkg-config 2>/dev/null || echo pkg-config)
PLATFORM:= $(shell ./config.guess)
LUA:= $(shell for i in {jit,"",5.3,53,5.2,52,5.1,51}; do $(PKGCONFIG) --exists lua$$i && echo $$i && exit; done; echo error)
ifeq (error,$(LUAVERSION))
$(error Cannot determine LUAVERSION, please provide on command line)
endif
$(info Selected LUAVERSION=$(LUAVERSION))
LUACFLAGS:=$(shell $(PKGCONFIG) --cflags lua$(LUAVERSION))
LUALDFLAGS:=$(shell $(PKGCONFIG) --libs lua$(LUAVERSION))

CC:=g++
CFLAGS:=-c -g -std=c++11 -ILuaBridge
LDFLAGS:=-lpthread -L/usr/lib -llua -ldl -lcurl
SOURCE_FILES:=$(wildcard src/*.cpp)
OBJECTS:=$(SOURCE_FILES:.cpp=.o)
BUILD_DIR:=bin

all: $(SOURCE_FILES) ircclient
	
ircclient: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*.o ircclient

src/client.o: src/sock.h src/client.h
src/main.o: src/client.h src/tracker.h
src/sock.o: src/sock.h
