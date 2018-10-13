CURRENT_DIR = $(shell pwd)
PROJECT_SOURCE_DIR = $(CURRENT_DIR)/src/
CFLAGS += -Wall
LDFLAGS += -lhidapi-hidraw
TARGET = SwitchListener
OBJS = AnswerReader.o Controller.o HidComm.o HidScanner.o main.o
SRC = $(wildcard src/*.cpp)

all: $(TARGET)

$(OBJS):
	g++ -c $(SRC) $(CFLAGS)

$(TARGET): $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)