rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
OS := $(shell uname)
CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic -g

CLIBS=-pthread -lcurl

ifneq ($(OS),Darwin)
CLIBS+=`pkg-config --libs --cflags openssl` 
endif

NAME=serverc

.OBJECTS=$(call rwildcard,src,*.c)

FW_PATHS=$(wildcard ./deps/*)
foreachfw=$(foreach fw, $(FW_PATHS), $(wildcard $(fw)$1))

FW_INC=$(addprefix "-I" ,$(call foreachfw,/inc*/))
FW_BIN=$(call foreachfw,/*.a)

all: $(FW_PATHS) $(NAME)

$(NAME): $(FW_BIN) $(.OBJECTS)
	$(CC) $(CFLAGS) $(CLIBS) $(FW_INC) -Iinc -o $(NAME) $(.OBJECTS) $(FW_BIN) $(CDEFINES)

$(FW_PATHS):
	$(MAKE) -C $@

clean:
	rm -rf obj

uninstall: clean
	rm -f $(NAME)

reinstall:
	rm -f $(NAME)
	$(MAKE) $(NAME)

.PHONY: $(FW_PATHS)
