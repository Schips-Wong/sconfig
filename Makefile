#############################################################
# Generic Makefile for C/C++ Program
#
# License: GPL (General Public License)
# Author:  	whyglinux <whyglinux AT gmail DOT com>
#	  	   	schips    <schips AT dingtalk DOT com>
#
# Date:    	2006/03/04 (version 0.1)@whyglinux
#	 	   	2007/03/24 (version 0.2)@whyglinux
#	 	   	2007/04/09 (version 0.3)@whyglinux
#	 	   	2007/06/26 (version 0.4)@whyglinux
#	 	   	2008/04/05 (version 0.5)@whyglinux
#	 	   	..
#	 	   	2020/02/25 (version 1.0)@schips : More powerful search, add cross complie
#	 	   	2020/03/24 (version 1.1)@schips : Disable making dependencies
#	 	   	2020/08/14 (version 1.2)@schips : Enable dependencies and hide them
#	 	   	2020/08/22 (version 1.3)@schips : Enable make lib for .so, .a
#	 	   	2020/08/26 (version 1.4)@schips : Add color for UI
#
# Description:
# ------------
# This is an easily customizable makefile template. The purpose is to
# provide an instant building environment for C/C++ programs.
#
# It searches all the C/C++ source files in the specified directories,
# makes dependencies, compiles and links to form an executable.
#
# For GNU make
#
# Usage:
# ------
# 1. Copy the Makefile to your program directory.
# 2. Customize in the "Customizable Section" only if necessary:
# 		* TYPE 'DIY' TO LOCATE THE POSITION WHERE YOU CAN CUSTOMIZE
#       * to search sources/header in more directories, set to <SRCDIRS>, <INCDIRS>
#       * to specify your program name, set to <PROGRAM>
# 3. Type `make` to build your program.
# 		* `make help` for more details.

##========================================================================##
## Customizable Section: adapt those variables to suit your program.      ##
##========================================================================##

##==========================DIY=START=====================================##

# The type of the result, can be "so" for .so, "a" for .a .
# or empty(Default for executable file)
TYPE=

# The output file name.
# If not specified, for difference TYPE, result will be 
#   (Default): current directory name(or 'a.out'), eg : xx or a.out
#         so : current directory name,             eg : libxx.so
#          a : current directory name,             eg : libxx.a
PROGRAM   = test

# The pre-processor and compiler options.
# For c/c++ (All)
ALLFLAGS  = -Wall -g -O2
# Just for c   compiler
CFLAGS    = 
# Just for cpp compiler(man cpp for more).
CXXFLAGS  = 

# The options used in linking as well as in any direct use of ld.
LDFLAGS   =

# For cross complie, as same as gcc if not specified.
CROSS_COMPLIE=
#CROSS_COMPLIE=arm-linux-

# The directories in which source/include/library files reside.
# If not specified, Makefile will:
#   for SRCDIRS, INCDIRS : search current directory recursively
SRCDIRS   = .
INCDIRS   = . ../../
#   for LIBDIR : only the current directory will be serached.
LIBDIR    =

# The list of the libs' name used in project.
#LIBS = pthread m

##==========================DIY=END=======================================##

##========================================================================##
## Implicit Section: change the following only when necessary.            ##
##========================================================================##

# The source file types (headers excluded).
# .c indicates C source files, and others C++ ones.
SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp

# The header file types.
HDREXTS = .h .H .hh .hpp .HPP .h++ .hxx .hp

# Compatible cross complie  
CC     = $(CROSS_COMPLIE)gcc
CXX    = $(CROSS_COMPLIE)g++
AR     = $(CROSS_COMPLIE)ar
LD     = $(CROSS_COMPLIE)ld
AS     = $(CROSS_COMPLIE)as
OBJCOPY= $(CROSS_COMPLIE)objcopy

# The command used to delete file.
#RM     = rm -f

ETAGS = etags
ETAGSFLAGS =

CTAGS = ctags
CTAGSFLAGS =


##========================================================================##
## Stable Section: usually no need to be changed. But you can add more.   ##
##========================================================================##
SHELL   = /bin/sh
EMPTY   =
SPACE   = $(EMPTY) $(EMPTY)
ifeq ($(PROGRAM),)
  CUR_PATH_NAMES = $(subst /,$(SPACE),$(subst $(SPACE),_,$(CURDIR)))
  PROGRAM = $(word $(words $(CUR_PATH_NAMES)),$(CUR_PATH_NAMES))
  ifeq ($(PROGRAM),)
	PROGRAM = a.out
  endif
endif

ifneq ($(TYPE),)
PROGRAM  = lib$(word $(words $(CUR_PATH_NAMES)),$(CUR_PATH_NAMES)).$(TYPE)
endif

ifeq ($(SRCDIRS),)
	SRCDIRS:=$(shell find . -type d)
endif

ifeq ($(INCDIRS),)
	INCDIRS:=$(shell find . -type d)
endif

ifeq ($(LIBDIR),)
	LIBDIR=.
endif

INC_P    = -I
LIBD_P   = -L
LIB_P    = -l

CFLAGS   += $(addprefix $(INC_P), $(INCDIRS))
ifeq ("$(TYPE)","so")
ALLFLAGS   += -fPIC
endif

CXXFLAGS += $(addprefix $(INC_P), $(INCDIRS))

ALLLIBS  += $(addprefix $(LIB_P), $(LIBS))
ALLLIBS  += $(addprefix $(LIBD_P), $(LIBDIR))


SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
HEADERS = $(foreach d,$(INCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))
SRC_CXX = $(filter-out %.c,$(SOURCES))
OBJS    = $(addsuffix .o, $(basename $(SOURCES)))
#DEPS    = $(foreach f, $(OBJS), $(addprefix $(dir $(f))., $(patsubst %.o, %.d, $(notdir $(f)))))

## Define some useful variables.
#DEP_OPT = $(shell if `$(CC) --version | grep -i "GCC" >/dev/null`; then \ 
# DEP_OPT = $(shell if `gcc --version | grep -i "GCC" >/dev/null`; then \
#                   echo "-MM"; else echo "-M"; fi )
# #DEPEND      = $(CC)  $(DEP_OPT)   $(CFLAGS) $(ALLFLAGS)
# DEPEND      = gcc  $(DEP_OPT)   $(CFLAGS) $(ALLFLAGS)
# DEPEND.d    = $(subst -g ,,$(DEPEND))
COMPILE.c   = $(CC)   $(CFLAGS)   $(ALLFLAGS) -c
COMPILE.cxx = $(CXX)  $(CXXFLAGS) $(ALLFLAGS) -c
LINK.c      = $(CC)   $(CFLAGS)   $(ALLFLAGS) $(LDFLAGS)
LINK.cxx    = $(CXX)  $(CXXFLAGS) $(ALLFLAGS) $(LDFLAGS)

.PHONY: all objs tags ctags clean distclean help show

# Delete the default suffixes
.SUFFIXES:

all: $(PROGRAM)


# Rules for generating object files (.o).
#----------------------------------------
objs:$(OBJS)
	@echo -e "\u276f \c "
%.o:%.c
	@echo -e "\u273f \c "
	$(COMPILE.c) $< -o $@

%.o:%.C
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.cc
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.cpp
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.CPP
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.c++
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.cp
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

%.o:%.cxx
	@echo -e "\u273f \c "
	$(COMPILE.cxx) $< -o $@

# Rules for generating the tags.
#-------------------------------------
tags: $(HEADERS) $(SOURCES)
	$(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SOURCES)

ctags: $(HEADERS) $(SOURCES)
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)

# Rules for generating the executable.
#-------------------------------------
$(PROGRAM):$(OBJS) 
	@echo -e "\e[36m\u2794 \e[0m\c"
ifeq ($(SRC_CXX),)	      # C program
ifeq ("$(TYPE)","so")     ## C and so
	$(LINK.c) -shared  $(OBJS) $(ALLLIBS) -o $@
else
ifeq ("$(TYPE)","a")      ## C and a
	$(AR) rcs $@ $^
else
	$(LINK.c)   $(OBJS) $(ALLLIBS) -o $@

endif
endif
else			          # C++ program
ifeq ("$(TYPE)","so")     ## C++ and so
	$(LINK.cxx) -shared  $(OBJS) $(ALLLIBS) -o $@ 
else
ifeq ("$(TYPE)","a")      ## C++ and a
	$(AR) rcs $@ $^
else
	$(LINK.cxx) $(OBJS) $(ALLLIBS) -o $@
endif
endif
endif
	@echo -e "\e[32m\u2713 \e[0m\c"
	@echo "Made [$@]"

#ifndef NODEP
#ifneq ($(DEPS),)
#-include $(DEPS)
#endif
#endif

clean:
	@echo  -e "\e[31m\u2717 \e[0m\c"
	@$(RM) $(OBJS) $(PROGRAM) $(PROGRAM).exe
	@echo  -e "$(RM) $(OBJS) $(PROGRAM)"

distclean: clean
	@echo  -e "\e[31m\u2717 \e[0m\c"
	$(RM) $(DEPS) TAGS

# Show help.
help:
	@echo 'Generic Makefile for C/C++ Programs (gcmakefile) version 1.2'
	@echo 'Copyright (C) schips <schips@dingtalk.com>'
	@echo
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link.'
	@echo '  NODEP=yes make without generating dependencies.'
	@echo '  objs      compile only (no linking).'
	@echo '  tags      create tags for Emacs editor.'
	@echo '  ctags     create ctags for VI editor.'
	@echo '  clean     clean objects and the executable file.'
	@echo '  distclean clean objects, the executable and dependencies.'
	@echo '  show      show variables (for debug use only).'
	@echo '  help      print this message.'

# Show variables (for debug use only.)
show:
	@echo 'PROGRAM     :' $(PROGRAM)
	@echo 'SRCDIRS     :' $(SRCDIRS)
	@echo 'INCDIRS     :' $(INCDIRS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'HEADERS     :' $(HEADERS)
	@echo 'SRC_CXX     :' $(SRC_CXX)
	@echo 'OBJS        :' $(OBJS)
	@echo 'DEPS        :' $(DEPS)
	@echo 'DEPEND      :' $(DEPEND)
	@echo 'COMPILE.c   :' $(COMPILE.c)
	@echo 'COMPILE.cxx :' $(COMPILE.cxx)
	@echo 'link.c      :' $(LINK.c)
	@echo 'link.cxx    :' $(LINK.cxx)

## End of the Makefile ##
##############################################################
