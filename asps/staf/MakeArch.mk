#  $Log: MakeArch.mk,v $
#  Revision 1.13  1998/06/18 19:32:45  ward
#  fix crash in sockstream, improve TABLE/DUMP
#
#  Revision 1.10  1998/05/19 16:36:38  perev
#  Makefiles
#
#  Revision 1.4  1998/02/13 14:18:20  fisyak
#  Simplify Makefile, reduce SLibrary
#
#  Revision 1.3  1998/02/12 13:35:09  fisyak
#  Add versioning, new Makefile with domain/package libraries
#
#  Revision 1.2  1998/02/10 00:06:10  fisyak
#  SL98a second version
#
#  Revision 1.1  1998/01/31 23:32:52  fisyak
#  New Environment variables
#
#  Revision 1.2  1998/01/30 12:42:16  fisyak
#  Save changes before moving to SL97b
#
#  Revision 1.1.1.1  1997/12/31 14:35:23  fisyak
#  Revision ?.?.?.?  1998/02/07           perev
#
#             Last modification $Date: 1998/06/18 19:32:45 $ 
#. default setings


GCC      :=  gcc
CC       :=  $(GCC)
CFLAGS   := -g -fpic -w
CXX      :=  g++
CXXFLAGS := -g -fpic -w
FC 	 := f77
AR       := ar
ARFLAGS  := rvu
LD 	 := $(CXX)
SO	 := $(CC)
SOFLAGS	 := 
SOEXT    := so
CLIBS    :=
FLIBS    :=
CPPFLAGS := $(UNAMES) $(STAF_ARCH) $(TULL_ARCH)
OSFID    :=
YACC     := yacc
YACCLIB  := -ly
LEX      := lex
LEXLIB   := -ll
ifneq (,$(findstring $(STAF_ARCH),rs_aix31 rs_aix32 rs_aix41))
#  case IBMRT
#  ====================

  OSFID :=aix AIX CERNLIB_IBMRT CERNLIB_UNIX CERNLIB_QMIBM
  ifdef GCC.
    CXXFLAGS := -g  -fsigned-char -w  
    CFLAGS  := -g  -fsigned-char -w 
    LDFLAGS  := 
    SOFLAGS  :=  -shared 
    CLIBS  :=  -lXm -lXt -lX11 -lg++ -lm -lld
  else
    CXX    := xlC
    CC     := xlC
    LD     := $(CXX)
    SO     := ???
    SOFLAGS := ???
    CXXFLAGS := -g -w -qchars=signed -qnoro -qnoroconst 
    CFLAGS  := -g -w -qchars=signed -qnoro -qnoroconst 
    LDFLAGS  := 
    CLIBS  :=  -lXm -lXt -lX11  -lld  -lm -lc -lcurses
  endif

  FC         = xlf
  FLIBS   := -lxlf90 -lxlf
  FFLAGS  := -g -qextname  -qrndsngl -qcharlen=6000 
  FEXTEND := -e
endif 

ifneq (,$(findstring $(STAF_ARCH),i386_linux2))
#    case linux
#  ====================
  OSFID    := lnx Linux linux LINUX CERNLIB_LINUX CERNLIB_UNIX CERNLIB_LNX CERNLIB_QMLNX
  FC       := pgf77
  LD       := $(CXX)
  SO	   := $(CXX)
  CXXFLAGS := -g -fPIC
  CFLAGS   := -g -fPIC
  CPPFLAGS += f2cFortran
  LDFLAGS  := -Wl,-Bstatic
  SOFLAGS  := -shared 
  CLIBS    := -L/usr/X11R6/lib -Wl,-Bdynamic -lXpm -lXt -lXext -lX11 -lg++ -lm -ldl -rdynamic
  FLIBS    := -L/usr/pgi/linux86/lib -lpgftnrtl -lf2c
  FFLAGS   := -DPGI -w 
  FEXTEND  := -Mextend
  YACC     := bison -y
  YACCLIB  := 
  LEX      := flex
  LEXLIB   := -lfl
endif

ifneq (,$(findstring $(STAF_ARCH),alpha_osf1 alpha_osf32c alpha_dux40))
#    case "alpha":
#  ====================
  OSFID := osf ALPHA alpha CERNLIB_QMVAOS CERNLIB_DECS CERNLIB_UNIX
  ifdef GCC.
    CXXFLAGS := -w  -D__osf__ -D__alpha 
    CFLAGS  := -w  -D__osf__ -D__alpha 
    LDFLAGS  :=
    SOFLAGS  := -shared 
    CLIBS  := -lXm -lXt -lX11 -lg++ -lm -lPW -lm -lm_4sqrt -lots -lc
 
 else
    CXX    :=cxx
    CC     :=cc
    LD     :=$(CXX)
    SO     :=$(CXX)
    CXXFLAGS := -w -D__osf__ -D__alpha -Dcxxbug -DALPHACXX 
    CFLAGS  := -g -w  
    LDFLAGS  := 
    SOFLAGS  :=  -call_shared -expect_unresolved '*'
    CLIBS  :=  -lXm -lXt -lX11 -lm -lPW -lm -lm_4sqrt -lots -lc
 
  endif

  FLIBS    :=  -lUfor -lfor -lFutil 
  FFLAGS   :=  -pic  -static -fpe2 
  FEXTEND  :=  -extend_source 
endif

ifneq (,$(findstring $(STAF_ARCH),hp_ux102 hp700_ux90))

#    case "hp":
#  ====================
  OSFID :=hpx hp HP-UX HPUX CERNLIB_HPUX CERNLIB_UNIX
  ifdef GCC
    CXXFLAGS  := -fPIC   -I/usr/include/X11R5 
    CFLAGS   := -fPIC   -I/usr/include/X11R5 
    LDFLAGS   := -b 
    SOFLAGS   := -shared 
    CLIBS   := -L/usr/lib/Motif1.2 -L/usr/lib/X11R5 -L/usr/lib -lXm -lXt -lX11 -lg++ -lm -lPW -ldld -L/opt/CC/lib -lC.ansi -lcxx -lcl -lc
    FLIBS   :=  /opt/fortran/lib/libU77.a 

  endif

  ifdef ACC.
    CXX     := aCC
    CC      := cc
    LD      := $(CXX)
    SO      := $(CXX)
    CXXFLAGS  := -z +Z -w 
    CFLAGS   := -Ae -z +Z 
    LDFLAGS   :=  -z -Wl,+s -Wl,-E 
    SOFLAGS   :=  -b -z -Wl,+vnocompatwarnings 
    CLIBS   :=   -lXm -lXt -lX11 -lm -lPW -ldld

  else
    CXX     :=  CC
    CC      := cc
    LD      := $(CXX)
    SO      := $(CXX)
    CXXFLAGS  := +a1 -z +Z -w 
    CFLAGS   :=  -Ae -z +Z 
    LDFLAGS   := +a1 -z -Wl,+s -Wl,-E 
    SOFLAGS   := -b +a1 -z 
    CLIBS   :=   -L/usr/lib/X11R5 -lXm -lXt -lX11 -lm -lPW -ldld
  endif
  SOEXT := sl
  FC        :=fort77
  FLIBS     := /opt/fortran/lib/libU77.a 
  FFLAGS    := +DA1.0 +ppu +Z  +U77
  FEXTEND   := +es
endif



ifneq (,$(findstring $(STAF_ARCH),sgi_52 sgi_53))
#  ====================
  OSFID := sgi SGI irix IRIX CERNLIB_SGI CERNLIB_UNIX
  FFLAGS    :=  -Nn20000 -static -trapuv  
  FEXTEND   :=  -extend_source
  CC        :=    cc
  CFLAGS    :=   -signed -fullwarn
  CXX       :=   CC
  CXXFLAGS  :=   -signed  -fullwarn
  ARFLAGS   :=   slrvu
  LD        :=   $(CXX)
  LDFLAGS   :=
  SO        :=   $(CXX)
  SOFLAGS   :=  -shared
  CLIBS     := -lsun  -lm -lc -lPW -lXext
  FLIBS     :=   -lftn 

endif

ifneq (,$(findstring $(STAF_ARCH),sgi_64 ))

  OSFID :=  irix64 sgi64 SGI64 IRIX64 CERNLIB_QMIRIX64 CERNLIB_SGI CERNLIB_UNIX
  FFLAGS    :=  -n32  -static -trapuv 
  FEXTEND   :=  -extend_source
  CC        :=    cc
  CFLAGS    :=  -n32  -fullwarn	
  CXX       :=    CC
  CXXFLAGS  :=  -n32 -fullwarn
  LD        :=   $(CXX)
  LDFLAGS   :=  -n32 
  SO        :=   $(CXX)
  SOFLAGS   :=  -n32 -shared
  CLIBS     :=  -lsun  -lm -lc -lPW -lXext
  FLIBS     :=  -lftn 

endif


ifneq (,$(findstring $(STAF_ARCH),sun4x_55 sun4x_56))

  OSFID := sun SUN SOLARIS solaris Solaris
  CC :=  insure
  CXX := insure
  LD  := $(CXX)
  SO  := $(CXX)
  FC  := /opt/SUNWspro/bin/f77

  FFLAGS   :=  -g  -KPIC -w 
  FEXTEND  :=  -e
  CFLAGS   :=  -g  -KPIC 
  CXXFLAGS :=  -g  -KPIC 
  LDFLAGS  :=  -g  -Bstatic
  SOFLAGS  :=   -G
  CLIBS    := -L/opt/SUNWspro/lib -L/opt/SUNWspro/SC4.2/lib -lM77 -lF77 -lm -lc -L/usr/ucblib -R/usr/ucblib -lucb
  FLIBS    := -lM77 -lF77 
endif


ifneq (,$(findstring $(STAF_ARCH),sunx86_55))
  OSFID := sun SUN SOLARIS SOLARISPC
  CC       :=  insure
  CXX      := insure
  SO       := $(CXX)
  FC       := /opt/SUNWspro/bin/f77
  LD       := $(CXX)
  FFLAGS   :=  -g -KPIC  
  FEXTEND  := -e
  CFLAGS   :=  -g -KPIC +w2
  CXXFLAGS :=  -g -KPIC +w2
  LDFLAGS  :=  -g -t -z muldefs
  SOFLAGS  :=  -g  -G
  CLIBS    := -L/opt/SUNWspro/lib -L/opt/SUNWspro/SC4.2/lib  -lm -lc -L/usr/ucblib -R/usr/ucblib -lucb
  FLIBS    := -lM77 -lF77 

endif

CPPFLAGS := $(CPPFLAGS) $(OSFID)
CPPFLAGS := $(sort $(addprefix -D,$(CPPFLAGS)))

FOR72 := $(FC)
FC  := $(FC) $(FEXTEND)

LD_LIBS := $(CLIBS) $(FLIBS)




