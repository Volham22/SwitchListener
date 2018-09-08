CURRENT_DIR = $(shell pwd)
PROJECT_SOURCE_DIR = $(CURRENT_DIR)/src
COMPILER = g++
EXECUTABLE_NAME = SwitchListener
LINKER_FLAG = -lhidapi-hidraw
ALL_WARNINGS = -Wall

all : SwitchListener Tools
	@echo "Done."

SwitchListener :
	@echo "Building SwitchListener"
	@$(COMPILER) -o $(EXECUTABLE_NAME) $(PROJECT_SOURCE_DIR)/*.cpp $(LINKER_FLAG) $(ALL_WARNINGS)

SwitchListener-Debug :
	@echo "Building SwitchListener"
	@$(COMPILER) -o $(EXECUTABLE_NAME) $(PROJECT_SOURCE_DIR)/*.cpp $(LINKER_FLAG) -g $(ALL_WARNINGS)

Tools :
	@echo "Building Tools"
	@gcc -o ListHIDDevice $(CURRENT_DIR)/tools/ListHIDDevice.c $(LINKER_FLAG) $(ALL_WARNINGS)

clean :
	@rm $(EXECUTABLE_NAME) ListHIDDevice
	@echo "Cleaned"