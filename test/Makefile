VERSION := 0.0.0
COMMONFLAGS := -Wall -Wextra -Wpedantic -O0 -ggdb -I../include 
CFLAGS := ${COMMONFLAGS}

STATIC_LIB := ../libsilver-broccoli.a.${VERSION}

all: main

${STATIC_LIB}:
	make -C ../

%: %.c ${STATIC_LIB}
	${CC} ${CFLAGS} -o $@ $< -L. -lsilver-broccoli

clean:
	${RM} main
