#
#
#
SRC=main.c loop.c
LIBS=-lpvm

ifndef CC
   CC=g++
endif
ifndef CC_OPTS
   CC_OPTS=-Wall -Wshadow -O3
endif
ifndef LIB2_DIR
   LIB2_DIR=$(HOME)/.lib2
endif
ifndef A_OUT
   A_OUT=a.out
endif

all:
	@$(CC) $(CC_OPTS) -I$(LIB2_DIR) $(SRC) -L$(LIB2_DIR) $(LIBS) -o $(A_OUT)
