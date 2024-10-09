CC = gcc
CFLAGS = -Wall -Werror

MAIN_TARGET = main.exe
MAIN_SRCS = main.c dram.c cpu.c

TEST_TARGET = test.exe
TEST_SRCS = test.c

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRCS)
	$(CC) $(CFLAGS) -o $(MAIN_TARGET) $(MAIN_SRCS)

$(TEST_TARGET): $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS)

test: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	del /F $(MAIN_TARGET) $(TEST_TARGET)
