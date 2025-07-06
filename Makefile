CC := gcc
CFLAGS :=  -Wall -Wextra -g 

TARGET := ipp_interpret
BUILD_DIR := build
INCLUDE_DIR := headders
SOURCE_DIR := src

SRCS := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
HEADDERS := $(wildcard $(INCLUDE_DIR)/*.h)

all: $(TARGET) bld_dir

$(TARGET): $(OBJS) bld_dir
	$(CC) $(CFLAGS) $(OBJS) -I. -o $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADDERS) bld_dir
	$(CC) $(CFLAGS) -I. -c $< -o $@

bld_dir:
	@mkdir -p $(BUILD_DIR)

.PHONY: all clean

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
#Fandím ti kocourku :3