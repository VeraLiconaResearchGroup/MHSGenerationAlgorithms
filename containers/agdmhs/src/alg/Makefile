SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

BINSRC := $(wildcard src/bin-*.cpp)
BINOBJ := $(BINSRC:.cpp=.o)

TESTSRC := $(wildcard test/*.cpp)
TESTOBJ := $(TESTSRC:.cpp=.o)

LIBOBJ := $(OBJ)
LIBOBJ := $(filter-out $(BINOBJ),$(LIBOBJ))
LIBOBJ := $(filter-out $(TESTOBJ),$(LIBOBJ))

DEP := $(SRC:.cpp=.P)
HDR := $(wildcard src/*.h)

# C++ compiler flags
CXXFLAGS += --std=c++14
CXXFLAGS += -fPIC
CXXFLAGS += -fopenmp
CXXFLAGS += -Wall
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

profile: CXXFLAGS += -g3 -pg
profile: all

debug: CXXFLAGS += -O0 -g3
debug: all

BIN = agdmhs
TEST = agdmhs-test
all: $(OBJ) $(BIN) $(TEST)

agdmhs: $(LIBOBJ) $(BINOBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

agdmhs-test: $(LIBOBJ) $(TESTOBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) -MD $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

clean:
	-rm -vf $(EXEC) $(OBJ) $(TESTOBJ) $(DEP) $(BIN) $(TEST)

