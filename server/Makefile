ROOTDIR := ..
TARGET := server

CFLAGS += -DLOGGER_ENABLE_STDERR

# comment/uncomment to set default debug level
CFLAGS += -DDEBUG_LEVEL_DEFAULT=10

SOURCES += connect_handler.cpp
SOURCES += control_server.cpp
SOURCES += controller.cpp
SOURCES += listener.cpp
SOURCES += parser.cpp
SOURCES += main_server.cpp



SOURCES_DIRS += $(ROOTDIR)/utils
SOURCES += event.cpp
SOURCES += cxx/printf.cpp
SOURCES += debug.c
SOURCES += gettid.c
SOURCES += global.cpp
SOURCES += logger.c
SOURCES += signal_handlers.cpp
SOURCES += time.c

LIBS += pthread
LIBS += rt

include $(ROOTDIR)/executable.Makefile