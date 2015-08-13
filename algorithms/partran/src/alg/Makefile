LIBS += -lcilkrts -lboost_program_options
CXXFLAGS += -O0 -g -fcilkplus -Wall -fpic
TARGET = partran

all: $(TARGET)

$(TARGET): main.o sort.o trvgen.o gen.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
sort.o: sort.cpp sort.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
trvgen.o: trvgen.cpp trvgen.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
gen.o: gen.cpp gen.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
clean:
	rm -f *~ *.o $(TARGET) *.out* *.log *.dat
run:
	./$(TARGET)
