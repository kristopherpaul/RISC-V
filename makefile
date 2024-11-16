CC = gcc
CFLAGS = -Wall -Werror

MAIN_TARGET = main.exe
MAIN_SRCS = src/main.c src/cpu.c src/trap.c src/clint.c src/dram.c src/plic.c src/uart.c src/bus.c src/virtio.c

TEST_TARGET = test.exe
TEST_SRCS = test.c

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRCS)
	$(CC) $(CFLAGS) -o $(MAIN_TARGET) $(MAIN_SRCS)

$(TEST_TARGET): $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS)

test: $(TEST_TARGET)
	MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL="*" ./$(TEST_TARGET)

test_rv64u: $(TEST_TARGET)
	MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL="*" ./$(TEST_TARGET) "rv64u"

clean:
	MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL="*" rm -f $(MAIN_TARGET) $(TEST_TARGET)
