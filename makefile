CXX        ?= g++
CXXFLAGS   ?= -O3 -flto -Wall -Wextra -Wpedantic -std=c++20 -s
DESTDIR    ?=
PREFIX     ?= /usr/bin

all: sppkg

sppkg:
	$(CXX) main.cc lib/pkg.cc lib/tar.cc lib/gbl.cc $(CXXFLAGS) -o sppkg -larchive

testpkg: sppkg
	./sppkg test

install:
	mkdir -p /sppkg/build
	mkdir -p /sppkg/temp
	mkdir -p /sppkg/var
	mkdir -p /sppkg/tars
	cp ./config/global /sppkg/var/global
	install -Dm755 sppkg $(DESTDIR)$(PREFIX)/sppkg

clean:
	rm -f sppkg
