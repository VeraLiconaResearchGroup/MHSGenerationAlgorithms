# File names
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(SRC:.cpp=.P)
BIN = dual

# C++ compiler flags
CXXFLAGS += -fPIC
CXXFLAGS += -Wall
CXXFLAGS += -g
CXXFLAGS += -O3

# Commands
all: $(OBJ) $(BIN)

-include $(DEP)

debug: CXXFLAGS += -O0
debug: all

dual: $(OBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) -MD $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)
