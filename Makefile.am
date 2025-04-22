lib_LTLIBRARIES = libmec.la
bin_PROGRAMS = test

libcps2010_la_SOURCES = \
			../mec_app.cpp 
			
test_SOURCES = cps2010/src/samples/test_shapedemo.c
test_LDADD = libcps2010.la -lpthread -lncurses -lstdc++ -ldl
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -fPIC
AM_CXXFLAGS = -fPIC


INCLUDES+= -Icps2010/src/includes

