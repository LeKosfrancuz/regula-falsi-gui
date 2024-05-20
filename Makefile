SHELL		= /bin/sh # from "http://www.gnu.org/prep/standards/", do not change
CC		= gcc # C compiler to use; this statement doesn't work usually, instead using cc.
# M4		?= m4 # Change this to gm4 in Unix or a non-GNU system.
# INSTALL		?= install # Installer utility to use; change to ginstall under Unix.
# INSTALL_PROGRAM	?= $(INSTALL) # Command to install executable program files.
# INSTALL_DATA	?= $(INSTALL) -m 0644 # Command to install data files.

CC_OPTIMIZE	?= -O3 # Default C compiler optimization flags that are safe (but please remove for LLVM/Clang, see "misc/known_bugs.txt").
# Be sure and run tests to see if Mathomatic works and runs faster, if you uncomment the following line:
#CC_OPTIMIZE	+= -fno-signaling-nans -fno-trapping-math -fomit-frame-pointer # Possible additional optimizations, not tested.

# VERSION		= `cat VERSION`
mathdir	?= ./mathomatic
OPTFLAGS	?= $(CC_OPTIMIZE) -Wall -Wshadow -Wno-char-subscripts -ggdb3 # optional gcc specific flags
CFLAGS		?= $(OPTFLAGS)
CFLAGS		+= -fexceptions -DUNIX -DVERSION=\"$(VERSION)\"
LDLIBS		+= -lm # libraries to link with to create the Mathomatic executable
LIB             = $(mathdir)/lib/libmathomatic.a
HEADERS		= $(mathdir)/lib/mathomatic.h

# Run "make READLINE=1" to include the optional readline editing and history support:
CFLAGS		+= $(READLINE:1=-DREADLINE)
LDLIBS		+= $(READLINE:1=-lreadline) # Add -lncurses if needed for readline, might be called "curses" on some systems.
# Run "make EDITLINE=1" to include the optional editline editing and history support (smaller than readline):
CFLAGS		+= $(EDITLINE:1=-DEDITLINE)
LDLIBS		+= $(EDITLINE:1=-leditline)

# Uncomment the following line to force generation of x86-64-bit code:
#CFLAGS		+= -m64

# Install directories follow; installs everything in $(DESTDIR)/usr/local by default.
# Note that support for the DESTDIR variable was added in version 15.2.1.
# DESTDIR is used by package maintainers, who should remove all DESTDIR patches,
# because DESTDIR is handled properly (according to GNU standards) by this makefile now.
# prefix		?= /usr/local
# bindir		?= $(prefix)/bin
# datadir		?= $(prefix)/share
# mandir		?= $(prefix)/share/man
# docdir		?= $(prefix)/share/doc
# mathdatadir	?= $(datadir)/mathomatic

# Mathomatic program names:
AOUT		?= regula_falsi.out
# M4SCRIPTPATH	= $(DESTDIR)$(bindir)/matho

INCLUDES	= $(mathdir)/includes.h $(mathdir)/license.h $(mathdir)/standard.h $(mathdir)/am.h $(mathdir)/externs.h $(mathdir)/blt.h $(mathdir)/complex.h $(mathdir)/proto.h $(mathdir)/altproto.h
MATHOMATIC_OBJECTS += $(mathdir)/globals.o $(mathdir)/am.o $(mathdir)/solve.o $(mathdir)/help.o $(mathdir)/parse.o $(mathdir)/cmds.o $(mathdir)/simplify.o \
		  $(mathdir)/factor.o $(mathdir)/super.o $(mathdir)/unfactor.o $(mathdir)/poly.o $(mathdir)/diff.o $(mathdir)/integrate.o \
		  $(mathdir)/complex.o $(mathdir)/complex_lib.o $(mathdir)/list.o $(mathdir)/gcd.o $(mathdir)/factor_int.o $(mathdir)/lib/lib.o

# PRIMES_MANHTML	= doc/matho-primes.1.html doc/matho-pascal.1.html doc/matho-sumsq.1.html \
# 		  doc/primorial.1.html doc/matho-mult.1.html doc/matho-sum.1.html
# HTML man pages to make:
# MANHTML		= doc/mathomatic.1.html doc/rmath.1.html $(PRIMES_MANHTML)

# Flags to make HTML man pages with rman:
# RMANOPTS	= -f HTML -r '%s.%s.html'
# RMAN		= rman
# Flags to make HTML man pages with groff instead (uncomment below and comment above to use):
#RMANOPTS       = -man -Thtml -P -l 
#RMAN		= groff

# man pages to install:
# MAN1		= mathomatic.1 rmath.1
# Text files to install:
# TEXT_DOCS	?= VERSION AUTHORS COPYING README.txt NEWS

# PDFDOC		= manual.pdf	# Name of PDF file containing the Mathomatic User Guide and Command Reference.
# PDFMAN		= bookman.pdf	# Name of PDF file containing all the Mathomatic man pages.

.PHONY: all $(LIB) $(AOUT) clean

all static: $(AOUT) $(LIB)# make these by default

$(LIB): $(INCLUDES) $(MATHOMATIC_OBJECTS)
	$(AR) cr $(LIB) $+
	-ranlib $(LIB)
	@echo
	@echo Built library ./$(LIB)
	@echo

$(MATHOMATIC_OBJECTS): %.o: ./%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# To compile Mathomatic as a stand-alone executable
# that has no shared library dependencies, type "make clean static".
static: LDFLAGS += -static

# Create the mathomatic executable:
$(AOUT): main.c $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $+ $(LDLIBS) -o $(AOUT)
	@echo
	@echo ./$(AOUT) successfully created.
	@echo


clean:
	cd $(mathdir)
	rm -f *.a
	rm -f *.o
