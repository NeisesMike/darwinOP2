###############################################################
#
# Purpose: Makefile for "game"
# Author.: robotis
# Version: 0.1
# License: GPL
#
###############################################################

TARGET = game

INCLUDE_DIRS = -I../../include -I../../../Framework/include

CXX = g++ 
CXXFLAGS += -O2 -DLINUX -Wall $(INCLUDE_DIRS)
#CXXFLAGS += -O2 -DDEBUG -DLINUX -Wall $(INCLUDE_DIRS)
LFLAGS += -lpthread -lrt -ljpeg

OBJECTS = StatusCheck.o Player.o main.o

all: $(TARGET)

Player.o: Player.h Player.cpp darwin.a
	$(CXX) $(CFLAGS) $(LFLAGS) $(INCLUDE_DIRS) -g -c Player.cpp ../../lib/darwin.a $(LFLAGS)

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo

libclean:
	make -C ../../build clean

distclean: clean libclean

darwin.a:
	make -C ../../build

$(TARGET): darwin.a $(OBJECTS)
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJECTS) ../../lib/darwin.a -o $(TARGET) $(LFLAGS)
	chmod 755 $(TARGET)

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups/DARwIn_demo_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *
