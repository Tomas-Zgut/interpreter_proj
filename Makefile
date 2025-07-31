CC := gcc
BUILD ?= debug

ifeq ($(BUILD), debug)
    CFLAGS := -Wall -Wextra -g
else ifeq ($(BUILD), release)
    CFLAGS := -Wall -Wextra -Werror -O3 -DNDEBUG
else
    $(error Unknown build type: $(BUILD))
endif


TARGET := ipp_interpret
TEST_TARGET := run_tests
BUILD_DIR := build
INCLUDE_DIR := headders
TEST_DIR := tests
SOURCE_DIR := src

SRCS := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
HEADDERS := $(wildcard $(INCLUDE_DIR)/*.h)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))
TEST_DEP_OBJS := $(filter $(BUILD_DIR)/DynBuf.o, $(OBJS))
GENERATOR_FILE := $(TEST_DIR)/gen_test_runner.py
GENERATED_FILE := $(TEST_DIR)/test_runner.h


VALGRIND_LOG_FILE := $(TEST_DIR)/valgrind_out.txt
VALGRIND_OPTIONS := -s --leak-check=full --track-origins=yes --show-leak-kinds=all --log-file=$(VALGRIND_LOG_FILE) --error-exitcode=1

all: $(TARGET) 

test: $(GENERATED_FILE) $(BUILD_DIR)/$(TEST_TARGET)
	@valgrind  $(VALGRIND_OPTIONS)  $(BUILD_DIR)/$(TEST_TARGET) || {\
		echo "🔴 Valgrind reported memory issues! See $(VALGRIND_LOG_FILE) for full details."; \
		tail -n 20 $(VALGRIND_LOG_FILE); \
		exit 1;\
	}

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I. -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADDERS) bld_dir
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HEADDERS) bld_dir
	$(CC) -I. -c $< -o $@

$(BUILD_DIR)/$(TEST_TARGET): $(TEST_OBJS) $(TEST_DEP_OBJS) $(HEADDERS)
	$(CC) -I.  $(TEST_DEP_OBJS) $(TEST_OBJS) -o $@


$(GENERATED_FILE): $(GENERATOR_FILE) $(TEST_SRCS)
	./$(GENERATOR_FILE)

bld_dir:
	@mkdir -p $(BUILD_DIR)

.PHONY: all clean test

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(GENERATED_FILE) $(VALGRIND_LOG_FILE)
#Fandím ti kocourku :3