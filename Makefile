SRC_LIST := 
SRC_PATH := src
INCLUDE_LIST := pay.h depay.h list.h common.h packet.h
INCLUDE_PATH := include


COMMONFLAGS += -I./${INCLUDE_PATH}
ifndef NDEBUG
    COMMONFLAGS += -Wall -g -O0
    CFLAGS += ${COMMONFLAGS}
    CXXFLAGS += ${COMMONFLAGS}
else
    COMMONFLAGS += -O3
    CFLAGS += ${COMMONFLAGS}
    CXXFLAGS += ${COMMONFLAGS}
endif

OBJ_LIST := $(patsubst %.c, %.o, ${SRC_LIST})
SHARE_OBJ_LIST := $(patsubst %.c, %-share.o, ${SRC_LIST})

all: libsilver-broccoli.a libsilver-broccoli.so

libsilver-broccoli.a: ${OBJ_LIST}
	${AR} -cvq $@ $^

libsilver-broccoli.so: ${SHARE_OBJ_LIST}
	${CC} -shared -o $@ $^

%-share.o: %.c
	${CC} ${CFLAGS} -c -fPIC $^

%-share.o: %.cc
	${CXX} ${CXXFLAGS} -c -fPIC $^

%.o: %.c
	${CC} ${CFLAGS} -c $^

%.o: %.cc
	${CXX} ${CXXFLAGS} -c $^

tags: ./include/*
	ctags ./include/*
