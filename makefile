cxx 		?= g++
cxxflags 	?=
destdir     ?=
prefix	    ?= /usr/bin

make:
	$(cxx) main.cc $(cxxflags) -o sppkg

testpkg:
	\./sppkg test

install:
	cp sppkg $(destdir)$(prefix)/sppkg
