CXX = g++-4.2
LD = $(CXX)
CXXFLAGS=-g -Wall -arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
LDFLAGS=-framework GLUT -framework OpenGL
RM = /bin/rm -f

.PHONY: clean

all: glut_test

glut_test: main.o image_loader.h
	$(LD) $(LDFLAGS) -o glut_test main.o
main.o: main.cc image_loader.h
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o
clean:
	$(RM) *~ main.o glut_test


#mkdir /Users/radu/workspace/glut_test/build/Debug/glut_test.app/Contents
#cd /Users/radu/workspace/glut_test
#<com.apple.tools.info-plist-utility> glut_test-Info.plist -genpkginfo build/Debug/glut_test.app/Contents/PkgInfo -expandbuildsettings -o build/Debug/glut_test.app/Contents/Info.plist

