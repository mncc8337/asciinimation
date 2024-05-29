SRC = main.cpp
SRC += $(wildcard ./test/*.cpp)

BIN = $(patsubst %.cpp,%,$(SRC))
DEPS = $(addsuffix .d, $(BIN))

LDLIBS = -lncurses

CXX = g++
CXXFLAGS = -I. -MMD

all: $(BIN)

clean:
	rm -f $(BIN) $(DEPS)

-include *.d
