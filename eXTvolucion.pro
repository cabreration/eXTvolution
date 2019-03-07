TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    Parser.c \
    Respuestas.c \
    Cadenas.c \
    FileSystem.c \
    Reports.c

HEADERS += \
    parser.h \
    respuestas.h \
    cadenas.h \
    structures.h \
    filesystem.h \
    reports.h
