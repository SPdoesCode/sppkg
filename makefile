CC      ?= cc
CFLAGS  ?= -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith \
           -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
           -Werror -O2 -pipe -fstack-protector-strong \
           -D_FORTIFY_SOURCE=2 -fPIE -static
DCFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -g3 -O0 -static

DESTDIR ?=
PREFIX  ?= /usr/bin
TARGET  = sppkg
SRC     = $(wildcard src/*.c)

.PHONY: all debug clean install

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $@

debug:
	$(CC) $(DCFLAGS) $(SRC) -o $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/$(TARGET)

clean:
	rm -f $(TARGET)
