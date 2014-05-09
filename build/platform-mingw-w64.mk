# Configuration for MinGW-w64.
# Don't be fooled by the name: it can compile to both 32-bit and 64-bit Windows.
#  http://mingw-w64.sourceforge.net/

include build/platform-mingw32.mk

ifeq ($(OPENMSX_TARGET_CPU),x86)
MINGW_CPU:=i686
else
MINGW_CPU:=$(OPENMSX_TARGET_CPU)
endif

CXX:=$(MINGW_CPU)-w64-mingw32-g++
WINDRES:=$(MINGW_CPU)-w64-mingw32-windres
STRIP:=$(MINGW_CPU)-w64-mingw32-strip