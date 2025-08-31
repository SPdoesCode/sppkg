ZIG       	?= zig
ZIGFLAGS  	?= -OReleaseSafe

SRC       	:= src/main.zig
TARGET    	:= sppkg

DESTDIR		?=
PREFIX		?= /usr/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(ZIG) build-exe $(ZIGFLAGS) -femit-bin=$(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/$(TARGET)
