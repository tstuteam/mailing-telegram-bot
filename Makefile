LIBS = -lTgBot -lspdlog -lfmt -lboost_system -lssl -lcrypto -lpthread
LDFLAGS  = ${LIBS}
# CFLAGS = -std=c++20 -pedantic -Wall -Wextra -Os # release
CFLAGS = -g -std=c++11 -pedantic -Wall -Wextra -O0 -DSPDLOG_SHARED_LIB -DSPDLOG_COMPILED_LIB -DSPDLOG_FMT_EXTERNAL # debug
CC = g++

SRC = bot.cpp lib.cpp
OBJ = ${SRC:.cpp=.o}

all: options bot

options:
	@echo bot build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.cpp.o:
	${CC} -c ${CFLAGS} $<

bot: $(OBJ)
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f bot ${OBJ}

.PHONY: all options clean