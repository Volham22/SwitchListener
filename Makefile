CURRENT_DIR = $(shell pwd)
PROJECT_SOURCE_DIR = $(CURRENT_DIR)/src/
COMPILER = g++
EXECUTABLE_NAME = SwitchListener
LINKER_FLAG = -lhidapi-hidraw
ALL_WARNINGS = -Wall
OBJECTS_FILES = AnswerReader.o Controller.o HidComm.o HidScanner.o main.o

all : SwitchListener Tools
	@echo "Done."

SwitchListener :
	@echo "Building SwitchListener"
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)AnswerReader.cpp $(LINKER_FLAG) $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)Controller.cpp $(LINKER_FLAG) $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)HidComm.cpp $(LINKER_FLAG) $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)HidScanner.cpp $(LINKER_FLAG) $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)main.cpp $(LINKER_FLAG) $(ALL_WARNINGS)
	$(COMPILER) -o $(EXECUTABLE_NAME) $(OBJECTS_FILES) $(LINKER_FLAG) $(ALL_WARNINGS)

SwitchListener-Debug :
	@echo "Building SwitchListener"
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)AnswerReader.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)Controller.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)HidComm.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)HidScanner.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)
	$(COMPILER) -c $(PROJECT_SOURCE_DIR)main.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)
	$(COMPILER) -o $(EXECUTABLE_NAME) $(OBJECTS_FILES) $(LINKER_FLAG) -g $(ALL_WARNINGS)

Tools :
	@echo "Building Tools"
	@gcc -o ListHIDDevice $(CURRENT_DIR)/tools/ListHIDDevice.c $(LINKER_FLAG) $(ALL_WARNINGS)

clean :
	@rm $(EXECUTABLE_NAME) ListHIDDevice
	@rm $(OBJECTS_FILES)
	@echo "Cleaned"