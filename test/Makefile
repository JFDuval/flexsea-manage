CC=gcc
SRC_DIR = ../src


CFLAGS = -I../inc \
				 -I./\
				 -I../flexsea-system/inc

TEST_OBJ = unity.o \
					 test_fm_block_allocator.o\
					 fm_block_allocator.o \
					 test_main.o

HEADERS = ../inc/fm_block_allocator.h

test: $(TEST_OBJ) $(HEADERS)
	gcc -g -o test $(TEST_OBJ) $(CFLAGS)
	./test


%.o: $(SRC_DIR)/%.c
	gcc -Wall --std=c99 -g $(CFLAGS) -c -o $@ $<

%.o: ../flexsea-shared/unity/%.c
	gcc -Wall --std=c99 -g $(CFLAGS) -c -o $@ $<

%.o: %.c
	gcc -g $(CFLAGS) --std=c99 -c -o $@ $<

%.o: %.cxx
	g++ -g $(CPPFLAGS) --std=c++11 -c -o $@ $<

clean:
	rm -f *.o test

cleanAndRun: clean test
