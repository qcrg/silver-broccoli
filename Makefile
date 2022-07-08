VERSION := 0.0.0

SRC_PATH := src
SRC_LIST := $(wildcard ${SRC_PATH}/*.c)
INCLUDE_PATH := include
INCLUDE_LIST := $(wildcard ${INCLUDE_PATH}/*.h)

COMMONFLAGS += -I./${INCLUDE_PATH} -Wall -Wextra -Wpedantic
ifndef NDEBUG
    COMMONFLAGS += -g -O0
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
	${CC} ${CFLAGS} -c -fPIC $^

%-share.o: %.cc
	${CXX} ${CXXFLAGS} -c -fPIC $^

%.o: %.c
	${CC} ${CFLAGS} -c $^

%.o: %.cc
	${CXX} ${CXXFLAGS} -c $^

tags: ${INCLUDE_LIST}
	ctags $^

clean:
	${RM} *.o tags *.a* *.so*
