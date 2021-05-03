LIBS=dwrite d2d1 ole32 windowscodecs
SRC=Game.cpp Map.cpp main.cpp Renderer.cpp Sprite.cpp
DEBUG:=y
NAME:=out
IDIR=include
LDIR=lib
64BIT:=y
OPTIMIZELEVEL:=2
CFLAGS:=
LFLAGS:=-mwindows
SHARED=n
CC=g++
TESTFILENAME=test

ifeq ($(DEBUG),y)
	CFLAGS+=-g
	LFLAGS+=-g
else
	CFLAGS+=-O$(OPTIMIZELEVEL)
	LFLAGS+=-O$(OPTIMIZELEVEL)
endif

ifeq ($(64BIT),n) 
	32BITFLAG=-m32
	NAME:=$(NAME)32
endif

ifeq ($(SHARED),y) 
	LFLAGS+=-shared
	OUTFILE=$(NAME).dll
else
	OUTFILE=$(NAME).exe
endif

LDIR?=.
IDIR?=.
ODIR=build
SRCDIR=src

.PHONY: clean fresh fresh-test test all status

$(OUTFILE): $(patsubst %.cpp,$(ODIR)/%.o,$(SRC))
	@echo linking $(OUTFILE)...
	@$(CC) -o $(OUTFILE) $^ $(32BITFLAG) $(LFLAGS) -L$(LDIR) $(patsubst %,-l%,$(LIBS))

 $(ODIR):
	@mkdir $@

$(TESTFILENAME).exe: $(patsubst %.cpp,$(ODIR)/%.o,test.cpp)
	@echo building $(TESTFILENAME).exe...
	@$(CC) -o $(TESTFILENAME).exe $(32BITFLAG) $^ -L. -l$(NAME)

-include $(ODIR)/.depend

$(ODIR)/.depend: $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(SRC))
	@echo building dependencies
	@-del /s /q "$@"
	@$(foreach X,$^,$(CC) $(CFLAGS) -I$(IDIR) -I$(LDIR) -MT $(patsubst $(SRCDIR)/%.cpp,$(ODIR)/%.o,$(X)) -MM $(X) >> "$@" && ) echo done

$(ODIR)/%.o: $(SRCDIR)/%.cpp | $(ODIR)
	@echo $(patsubst $(ODIR)/%,%,$@):
	@$(CC) -c -o $@ $< $(32BITFLAG) $(CFLAGS) -I$(IDIR) -I$(LDIR)

status:
	@echo -------------------------------------------------
	@echo Sources: $(SRC)
	@echo Debug: $(if $(subst y,,$(DEBUG)),no,yes)
	@echo 64-Bit: $(if $(subst y,,$(64BIT)),no,yes)
ifneq ($(DEBUG),y) 
	@echo Optimization: O$(OPTIMIZELEVEL)
endif
	@echo Shared Library: $(if $(subst y,,$(SHARED)),no,yes)
	@echo Output File: $(OUTFILE)
	@echo Libraries: $(LIBS)
	@echo CFLAGS: $(CFLAGS)
	@echo -------------------------------------------------

all: status $(OUTFILE)
test: all $(TESTFILENAME).exe
fresh: clrscr clean all
fresh-test: clrscr clean test

clrscr:
	@cls

clean:
	@echo cleaning up...
	@-rmdir /s /q $(ODIR)
	@-del /s /q $(OUTFILE)