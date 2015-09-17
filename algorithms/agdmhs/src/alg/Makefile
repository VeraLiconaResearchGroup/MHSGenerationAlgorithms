SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(SRC:.cpp=.P)
BIN = agdmhs

# C++ compiler flags
CXXFLAGS += --std=c++11
CXXFLAGS += -fPIC
CXXFLAGS += -fopenmp
CXXFLAGS += -Wall
CXXFLAGS += -g
CXXFLAGS += -O3

# Libraries
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_log

# Includes
INCLUDES += -Iinclude

# Commands
all: $(OBJ) $(BIN)

-include $(DEP)

debug: CXXFLAGS += -O0
debug: all

agdmhs: $(OBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) -MD $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

clean:
	-rm -vf $(EXEC) $(OBJ) $(DEP) $(BIN)
