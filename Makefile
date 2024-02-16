CC = g++
INCLUDES = -I/usr/include/GL
LIBS = -Wl,-rpath=/usr/local/cs/lib64 -lglut -lGL -lGLU
STD = -std=c++17
G32 = /usr/local/cs/bin/g32

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

PRODUCT = MarbleMadness

all: $(PRODUCT)

%.o: %.cpp $(HEADERS)
	cmd=$(CC); [ -x $(G32) ] && cmd=$(G32); $$cmd -c $(STD) $(INCLUDES) $< -o $@

$(PRODUCT): $(OBJECTS) 
	cmd=$(CC); [ -x $(G32) ] && cmd=$(G32); $$cmd $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PRODUCT)
