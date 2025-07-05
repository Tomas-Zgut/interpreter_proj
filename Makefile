CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -g

TARGET := ipp_interpret
BUILD_DIR := build
INCLUDE_DIR := headders
SOURCE_DIR := src
SRCS := $(wildcard $(SOURCE_DIR)/ *.c)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
HEADDERS := $(wildcard $(INCLUDE_DIR)/*.h)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

.PHONY: all clean 

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
