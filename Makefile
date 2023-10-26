# Makefile for Your Project
# Compiler
CC = gcc
OPTIMIZE = -O3
PROFILER = -pg

# Compiler flags
ifeq ($(OS),Windows_NT)
	CFLAGS = -g3 -ggdb -Wall -Wextra  -pedantic $(OPTIMIZE)
	EXECUTABLE = gofl.exe
	LIBS = -lm -lWs2_32
else
	CFLAGS = -ggdb -Wall -Wextra $(OPTIMIZE) -pedantic -fsanitize=address
	EXECUTABLE = gofl
	LIBS = -lm 
endif

#Test File
TEST_FILE = ./src/test.c
TEST_FILE_OUT = ./test.out

# Libraries


#filters
FILTERS=main

# Source files

SRC_FILES:=$(shell find  -L -type f -iname \*.c | grep -v $(FILTERS))


OBJ_FILES = $(SRC_FILES:.c=.o)



all: $(EXECUTABLE)

# Rule to compile C source files into object files
%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@ $(LIBS) $(PROFILER)

$(EXECUTABLE): $(OBJ_FILES)
		$(CC) $(CFLAGS)  $^ -o $@ $(LIBS)  $(PROFILER)

debug: compile
	gdb ./$(EXECUTABLE)

clean:
ifeq ($(OS),Windows_NT)
	rm .\src\*.o
	rm  .\src\*\*.o
	rm .\*.exe
else
	rm $(OBJ_FILES) $(EXECUTABLE)
endif

run: $(EXECUTABLE)
	./$(EXECUTABLE)

test:
ifneq ("$(wildcard $(TEST_FILE))","")
	$(CC) $(TEST_FILE) -o $(TEST_FILE_OUT)
	$(TEST_FILE_OUT)
	rm $(TEST_FILE_OUT)
endif
	echo -e "Done✅"

.PHONY: all compile debug clean run
