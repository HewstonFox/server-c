CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic -Wno-error=unused-command-line-argument -g
CLIBS=-pthread
NAME=serverc

.OBJECTS = $(wildcard src/*.c)
OBJECTS = $(.OBJECTS:src%.c=obj%.o)

DEPS_PATHS=$(wildcard ./deps/*)
foreachdep=$(foreach dep, $(DEPS_PATHS), $(wildcard $(dep)$1))

DEPS_INC=$(addprefix "-I" ,$(call foreachdep,/inc*/))
DEPS_BIN=$(call foreachdep,/*.a)

all: prepare $(DEPS_PATHS) $(NAME)

prepare:
	mkdir -p obj/

$(NAME): $(OBJECTS)
	ar -rcsT $(NAME).a $(DEPS_BIN) $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(DEPS_INC) -Iinc -o $@ -c $< $(DEPS_BIN)

$(DEPS_PATHS):
	$(MAKE) -C $@

clean:
	rm -rf obj

uninstall: clean
	rm -f $(NAME)

reinstall: uninstall all

.PHONY: $(DEPS_PATHS)
