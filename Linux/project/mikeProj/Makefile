###############################################################
#
# Purpose: Makefile for "game"
# Authors: robotis, mike
# Version: 0.1
# License: GPL
#
###############################################################

TARGET = game

INCLUDE_DIRS = -I../../include -I../../../Framework/include

SPEECH_DIRS = -I/usr/share/festival/festival/src/include -I/usr/include/speech_tools/
SPEECH_FLAGS = -lFestival -lestools -leststring
SPEECH_INCLUDES = $(SPEECH_DIRS) $(SPEECH_FLAGS)

CXX = g++
CXXFLAGS += -O2 -DLINUX -Wall $(INCLUDE_DIRS) $(SPEECH_DIRS)
LFLAGS += -ljpeg 

OBJECTS = StatusCheck.o Body.o Eyes.o Player.o main.o ../../lib/darwin.a

all: $(TARGET)

Eyes.o: Eyes.h Eyes.cpp darwin.a
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c Eyes.cpp

Body.o: Body.h Body.cpp darwin.a
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) -c Body.cpp

Player.o: Player.h Player.cpp darwin.a
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) $(SPEECH_DIRS) -c Player.cpp

darwin.a:
	make -C ../../build

$(TARGET): darwin.a $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(INCLUDE_DIRS) $(SPEECH_INCLUDES) ../../lib/darwin.a -o $(TARGET) /usr/lib/libestbase.a $(LFLAGS) 
	chmod 755 $(TARGET)

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo

libclean:
	make -C ../../build clean

distclean: clean libclean

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups/DARwIn_demo_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *

