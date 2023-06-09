# Define required macros here

#c99 -I /opt/homebrew/include/ -lsodium -L /opt/homebrew/lib/ eth.c keccak-tiny.c ecdh.c

SHELL=/bin/sh

OBJS= main.cpp									\
			eth_keys.cpp							\
			btc_keys.cpp							\
			keccak-tiny.cpp						\

CFLAGS=-Wall -Ofast -fomit-frame-pointer -pipe -std=c++14 -march=native -Wno-deprecated-declarations -Wno-unused-result 
# -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage -pg
CC=c++
INCLUDES= -I/opt/homebrew/include/ -I/usr/include -I/opt/homebrew/opt/openssl@3/include/
LIBS_PATH= -L/opt/homebrew/lib/  -L/usr/lib/x86_64-linux-gnu/ -L/opt/homebrew/opt/openssl@3/lib
LIBS= -lm -lsodium -lsecp256k1 -lcrypto 
#-lprofiler

# all: 
# 	${CC} -o ${CFLAGS} ${INCLUDES} ${LIBS} ${LIBS_PATH} -o $@ ${OBJS} 


# %.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

compute: clean  ${OBJ}
	${CC} ${CFLAGS} ${INCLUDES} ${LIBS} ${LIBS_PATH} -o $@ ${OBJS} 

clean:
	-rm -f *.o compute *.core

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} ${LIBS} ${LIBS_PATH} -c $<

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} ${LIBS} ${LIBS_PATH} -c $<

# .PHONY : clean