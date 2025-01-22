ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS=-Wall -g
AM_CFLAGS+=-D DEBUG
AM_CFLAGS+=-D WARNING
AM_CFLAGS+=-D BIG_ENDIAN_SIMULATOR
# Uncomment if performance when running with -DDEBUG is an issue
# Warning, if uncommented, issuing calls to debug functions during options
# parsing might result in debug flag incorrectly set to 0 for some files
#AM_CFLAGS+=-D CACHE_DEBUG_FLAG

LDADD=-lpthread


SUBDIRS=
SUBDIRS+=. Examples_loader

bin_PROGRAMS=
bin_PROGRAMS+=ARM_runner
bin_PROGRAMS+=custom_readelf
bin_PROGRAMS+=reimplantation


COMMON=debug.h debug.c util.h util.c status.h structure.h structure.c display.h display.c io_fun.h io_fun.c

EXTRA_DIST=filtre.pl
SIMULATOR_INTERFACE=csapp.h csapp.c scanner.h scanner.l gdb_protocol.h gdb_protocol.c arm_simulator_interface.h arm_simulator_interface.c

ARM_runner_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) ARM_runner.c

custom_readelf_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) custom_readelf.c

reimplantation_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) reimplantation.c

EXTRA_DIST+=sample_run.sh

test: all
	@./run_tests.sh
