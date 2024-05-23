CXX = g++
#CXX = clang++
CXXFLAGS= -Wall -Wextra -ggdb -Iinclude -O3
LIBS= -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
#-lavformat -lavcodec -lswresample


SRCDIR = src
INCDIR = include
BUILDDIR = build
TARGET = musvis

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR) $(TARGET)
