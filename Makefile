VERSION := 0.0.0

SRC_PATH := src
SRC_LIST := $(wildcard ${SRC_PATH}/*.c)
INCLUDE_PATH := include
INCLUDE_LIST := $(wildcard ${INCLUDE_PATH}/*.h)

COMMONFLAGS += -I./${INCLUDE_PATH} -Wall -Wextra -Wpedantic
ifndef NDEBUG
    COMMONFLAGS += -ggdb -O0
    CFLAGS += ${COMMONFLAGS}
    CXXFLAGS += ${COMMONFLAGS}
else
    COMMONFLAGS += -O3
    CFLAGS += ${COMMONFLAGS}
    CXXFLAGS += ${COMMONFLAGS}
endif

OBJ_LIST := $(patsubst %.c, %.o, ${SRC_LIST})
SHARE_OBJ_LIST := $(patsubst %.c, %-share.o, ${SRC_LIST})

STATIC_LIB_NAME := libsilver-broccoli.a.${VERSION} 
SHARED_LIB_NAME := libsilver-broccoli.so.${VERSION}



all: ${STATIC_LIB_NAME} ${SHARED_LIB_NAME}

${STATIC_LIB_NAME}: ${OBJ_LIST}
	${AR} -cvq $@ $^

${SHARED_LIB_NAME}: ${SHARE_OBJ_LIST}
	${CC} -shared -o $@ $^

%-share.o: %.c
	${CC} ${CFLAGS} -c -fPIC -o $@ $^

%-share.o: %.cc
	${CXX} ${CXXFLAGS} -c -fPIC -o $@ $^

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $^

%.o: %.cc
	${CXX} ${CXXFLAGS} -c -o $@ $^

tags: ${INCLUDE_LIST}
	ctags -o $@ $^

unit-test: ${TARGET}
	${CC} ${CFLAGS} -DUSE_TESTS -o $@ ${TARGET}

clean:
	${RM} ${SRC_PATH}/*.o tags *.a* *.so* unit-test
