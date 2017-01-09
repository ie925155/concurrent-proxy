#
# Makefile for http-proxy
# Based on Makefiles previously written by Julie Zelenski.
#

CXX = g++

# The CFLAGS variable sets compile flags for gcc:
#  -g                         compile with debug information
#  -Wall                      give all diagnostic warnings
#  -pedantic                  require compliance with ANSI standard
#  -O0                        do not optimize generated code
#  -std=c++0x                 go with the c++0x extensions for thread support, unordered maps, etc
#  -D_GLIBCXX_USE_NANOSLEEP   included for this_thread::sleep_for and this_thread::sleep_until support
#  -D_GLIBCXX_USE_SCHED_YIELD included for this_thread::yield support
CXXFLAGS = -g -Wall -pedantic -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I/usr/class/cs110/include -I/usr/class/cs110/local/include

# The LDFLAGS variable sets flags for linker
#  -lpthread  link in libpthread (thread library) to back C++11 extensions
#  -lthread   link to course-specific concurrency functions and classes
#  -lsocket++ link to open source socket++ library, which layers iostream objects over sockets
#LDFLAGS = -lpthread -L/usr/class/cs110/local/lib -lthreads -L/usr/class/cs110/lib/socket++ -lsocket++ -Wl,-rpath=/usr/class/cs110/lib/socket++
LDFLAGS = -lpthread -L/usr/class/cs110/local/lib -L/usr/class/cs110/lib/socket++ -lsocket++ -Wl,-rpath=/usr/class/cs110/lib/socket++

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
SOURCES = \
	main.cc \
	proxy.cc \
	scheduler.cc \
	thread-pool.cc \
	request-handler.cc \
	request.cc \
	response.cc \
	header.cc \
	payload.cc \
	cache.cc \
	blacklist.cc \
	ostreamlock.cc \
	string-utils.cc \
	semaphore.cc \

HEADERS = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
TARGETS = http-proxy

default: $(TARGETS)

http-proxy: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# In make's default rules, a .o automatically depends on its .cc file
# (so editing the .cc will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .cc to rocompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compiled object and backup files.
.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies

spartan: clean
	@rm -f *~
