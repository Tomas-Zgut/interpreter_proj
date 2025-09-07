CC := gcc
CFLAGS := -Wall -Wextra
BIN_DIR = build
BUILD ?= debug

TARGET := ipp_interpret
TEST_TARGET := run_tests

INCLUDE_DIR := headders
TEST_DIR := tests
SOURCE_DIR := src
COVERAGE_DIR := coverage

ifeq ($(BUILD), debug)
	CFLAGS += -g
	BUILD_DIR := $(BIN_DIR)/debug
else ifeq ($(BUILD), release)
    CFLAGS += -Werror -O3 -DNDEBUG
	BUILD_DIR := $(BIN_DIR)/release
else ifeq ($(BUILD),coverage)
	CFLAGS += -g --coverage
	BUILD_DIR := $(BIN_DIR)/coverage
else
    $(error Unknown build type: $(BUILD))
endif


SRCS := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
HEADDERS := $(wildcard $(INCLUDE_DIR)/*.h)

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))
TEST_DEP_OBJS := $(filter-out $(BUILD_DIR)/main.o, $(OBJS))

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

check_coverage_build:
ifeq ($(BUILD),coverage)
	@echo "BUILD is coverage continuing ..."
else 
	$(error "coverage target requires BUILD=cov. Current BUILD=$(BUILD)")
endif

coverage:
	$(MAKE) target_coverage BUILD=coverage

target_coverage: test | check_coverage_build dir_coverage
	lcov --capture --directory $(BUILD_DIR) --output-file $(COVERAGE_DIR)/coverage.info
	lcov --remove $(COVERAGE_DIR)/coverage.info '*/tests/*' -o $(COVERAGE_DIR)/coverage.filtered.info
	genhtml $(COVERAGE_DIR)/coverage.filtered.info --output-directory $(COVERAGE_DIR)/coverage-html

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I. -o $@ 

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADDERS) | dir_build
	$(CC) $(CFLAGS) -I. -c $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HEADDERS) | dir_build
	$(CC) $(CFLAGS) -I. -c $< -o $@ 

$(BUILD_DIR)/$(TEST_TARGET): $(TEST_OBJS) $(TEST_DEP_OBJS) $(HEADDERS)
	$(CC) $(CFLAGS) -I. $(TEST_DEP_OBJS) $(TEST_OBJS) -o $@


$(GENERATED_FILE): $(GENERATOR_FILE) $(TEST_SRCS)
	./$(GENERATOR_FILE)

$(BUILD_DIR)/test_runner.o: $(GENERATED_FILE)

release:
	$(MAKE) BUILD=release

dir_bin:
	@mkdir -p $(BIN_DIR)

dir_build: | dir_bin
	@mkdir -p $(BUILD_DIR)

dir_coverage:
	@mkdir -p $(COVERAGE_DIR)

.PHONY: all clean test release coverage dir_bin dir_build dir_coverage

clean:
	rm -rf $(BIN_DIR) $(TARGET) $(GENERATED_FILE) $(VALGRIND_LOG_FILE) $(COV_DIR)
#Fandím ti kocourku :3