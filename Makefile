SRC = main.cpp
SRC += $(wildcard ./test/*.cpp)

BIN = $(patsubst %.cpp,%,$(SRC))

LDLIBS = -lncurses

CXX = g++
CXXFLAGS = -I. -MMD

all: $(BIN)

clean:
	rm -f $(BIN)

-include *.d
