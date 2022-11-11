EXE = test
SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc, build/%.o, ${SRCS})
HEADERS = $(wildcard *.h)

CXXFLAGS = -Wall -g -I. -O3
LDFLAGS = 

LDFLAGS += $(shell root-config --libs)
CXXFLAGS += $(shell root-config --cflags)

CXX = g++

.PHONY: clean

all: ${EXE}

${EXE}: ${OBJS}
	${CXX} $^ -o $@ ${LDFLAGS}

${OBJS}: ${SRCS}
	${CXX} $^ -c  -o ${OBJS} ${CXXFLAGS}

clean:
	@rm -v ${EXE} ${OBJS}
