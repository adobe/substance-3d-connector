# Makefile
# Contains the compiling information for the connector framework Tclmodule
# Galen Helfter - Allegorithmic
# Copyright Allegorithmic. All rights reserved.

SHELL = /bin/sh

PROJ = substance_connector_tcl

VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
VERSION_SMALL = $(VERSION_MAJOR).$(VERSION_MINOR)

L_CC ?= clang
CC = $(L_CC)
CXX ?= clang++

INC = -I./include -I../../include -I/usr/include/
LIB =
DEFS = -DCONNECTOR_TCL_VERSION=\"$(VERSION_SMALL)\"

NOERR = -Wno-error=unused-variable -Wno-error=unused-parameter

CFLAGS = -std=c89 -Wall -fPIC -Wextra -Wpedantic -Werror $(NOERR) $(INC) $(DEFS)
LDFLAGS = -std=c89 -Wall -fPIC -shared -Wl,-rpath='$$ORIGIN'

TARFILE = $(PROJ).tar.gz
ZIPFILE = $(PROJ).zip

OBJECTS = src/connector_tcl.o

LIBS = -ltcl8.6 substance_connector.so

PROGS = connector.so

CLEANFILES = *.out core.* $(TARFILE) $(ZIPFILE) $(OBJECTS) $(PROGS)

all:	$(PROGS)

connector.so: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

CDB = compile_commands.json

.PHONY: clean tar zip database

database:
	rm -f $(CDB)
	CC=$(CC) CXX=$(CXX) bear --verbose --cdb $(CDB) --use-cc $(CC) --use-c++ $(CXX) make
	# Use sed to replace the references to cc and c++ with the actual compilers
	sed -i 's/''"cc"/"$(CC)"/g' compile_commands.json
	sed -i 's/''"c++"/"$(CXX)"/g' compile_commands.json


clean:
	rm -f $(CLEANFILES) $(CDB)

tar:
	rm -f $(TARFILE)
	tar -czvf $(TARFILE) ./*

zip:
	rm -f $(ZIPFILE)
	zip $(ZIPFILE) ./*
