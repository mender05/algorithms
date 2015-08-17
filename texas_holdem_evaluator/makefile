CC = gcc
CPP = g++
CFLAGS = -DDEBUG -Wall -O -g
TARGET = game
INSTALLPATH=~/game/works/target/
EXECPATH=/home/game/
BUILDPATH=./build/
%.o:%.cpp
    $(CPP) $(CFLAGS) -c $< -o $(BUILDPATH)$@
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(patsubst %.cpp,%.o, $(SOURCES)))

$(TARGET):$(OBJS)
    cd $(BUILDPATH); $(CPP) $(OBJS) -o $(TARGET); chmod a+x $(TARGET)
clean:
    rm -rf $(BUILDPATH)*.o $(BUILDPATH)$(TARGET)
    rm -rf $(INSTALLPATH)$(TARGET)
install:
    cp $(BUILDPATH)$(TARGET) $(INSTALLPATH)
exec:
    cd $(EXECPATH);./dist_check_and_run.sh
