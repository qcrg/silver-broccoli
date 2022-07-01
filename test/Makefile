CFLAGS := -Wall -Wextra -Wpedantic -g -I../include -DUSE_TESTS

all: list_test

test: main.cc ../src/packet.c ../src/allocator.c
	${CXX} -I../include -Wall -Wextra -Wpedantic -g -o $@ $^

clean:
	${RM} test list_test


list_test: ../src/list.c ../src/allocator.c
	${CC} ${CFLAGS} -o $@ $^


