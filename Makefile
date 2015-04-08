#
# Makefile
#
# LednickyEqn
#

CXX = $(shell root-config --cxx)
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

GSL_FLAGS = $(shell pkg-config gsl --cflags)
GSL_LIBS = $(shell pkg-config gsl --libs)

CFLAGS = -Wall -g ${ROOTCFLAGS} ${GSL_FLAGS} -std=c++11

#LEDNICKY_LIBS = lednicky.o lednickyplot.o faddeeva.o

LEDNICKY_LIBS = $(addprefix build/, lednicky.o faddeeva.o)

all: build lednicky

build:
	mkdir build
	touch build/.keep

build/%.o: src/%.cxx src/%.h
	${CXX} ${CFLAGS} -c $< -o $@

lednicky: src/main.cc ${LEDNICKY_LIBS} # src/lednicky.cxx src/lednicky.h
	${CXX} ${CFLAGS} $< -o $@ ${ROOTLIBS} ${LEDNICKY_LIBS} ${GSL_LIBS}

clean:
	rm build/*
