CXX ?= g++
CXXFLAGS ?= -g -O2
RM ?= rm -f

all:
	@echo "Compiling CSV generator..."
	${CXX} ${CXXFLAGS} makecolumn.cpp -o makecolumn
	@echo "Compiling BBC compressor..."
	${CXX} ${CXXFLAGS} superBBC_c.cpp -o superBBC_c
	@echo "Compiling BBC decompressor..."
	${CXX} ${CXXFLAGS} superBBC_d.cpp -o superBBC_d

clean:
	@echo "Cleaning up..."
	${RM} -r -v makecolumn superBBC_c superBBC_d
