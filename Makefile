st_SRCS = st.cc render.cc
squarelogo_SRCS = squarelogo.cc render.cc

st_OBJS = $(patsubst %.cc,%.o,$(st_SRCS))
squarelogo_OBJS = $(patsubst %.cc,%.o,$(squarelogo_SRCS))

CXXFLAGS = -O3 -g
CXX = g++
LDFLAGS = -lm

TARGETS = st squarelogo

all:: $(TARGETS)

clean::
	rm -f $(TARGETS) *.o

st: $(st_OBJS)
	$(CXX) -o $@ $(st_OBJS) $(CXXFLAGS) $(LDFLAGS)

squarelogo: $(squarelogo_OBJS)
	$(CXX) -o $@ $(squarelogo_OBJS) $(CXXFLAGS) $(LDFLAGS)


