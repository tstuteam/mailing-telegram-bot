CXX = g++
PKGS = spdlog openssl
LIBS = -lboost_system -lTgBot $(shell pkg-config --libs $(PKGS))
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra $(shell pkg-config --cflags $(PKGS))

DEBUG = false
ifeq ($(DEBUG), true)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -Os
endif

SRC = bot.cpp lib.cpp
OBJ = ${SRC:.cpp=.o}

all: options bot

options:
	@echo bot build options:
	@echo "DEBUG		= ${DEBUG}"
	@echo "CXXFLAGS   	= ${CXXFLAGS}"
	@echo "LIBS  		= ${LIBS}"
	@echo "CXX       	= ${CXX}"

.cpp.o:
	${CXX} -c ${CXXFLAGS} $<

bot: $(OBJ)
	${CXX} -o $@ ${OBJ} ${LIBS}

clean:
	rm -f bot ${OBJ}

.PHONY: all options clean
