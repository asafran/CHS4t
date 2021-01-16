TEMPLATE = lib

QT -= gui
QT += xml

TARGET = chs4t

DESTDIR = $$(RRS_ROOT)/modules/$$join(TARGET,,,)

# Библиотеки симулятора, с которыми компонуется DLL локомотива
LIBS += -L$$(RRS_ROOT)/bin -lCfgReader
LIBS += -L$$(RRS_ROOT)/bin -lphysics
LIBS += -L$$(RRS_ROOT)/bin -lvehicle
LIBS += -L$$(RRS_ROOT)/bin -ldevice
LIBS += -L$$(RRS_ROOT)/bin -lfilesystem

INCLUDEPATH += ./include
INCLUDEPATH += ./shared-memory-tools
INCLUDEPATH += $$(RRS_ROOT)/sdk/include

HEADERS += $$files(./include/*.h) \
    $$files(./shared-memory-tools/*.h) \



SOURCES += $$files(./src/*.cpp) \
    $$files(./shared-memory-tools/*.cpp) \


CONFIG += force_debug_info
#QMAKE_CXXFLAGS += -Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wmaybe-uninitialized -Wreturn-type -Warray-bounds=1
