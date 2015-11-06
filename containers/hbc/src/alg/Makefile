SOURCES_CPP = algo.cpp dataset.cpp main.cpp ptreelibre.cpp support.cpp
SOURCES_H = algo.h constantes.h dataset.h ptreelibre.h support.h
OBJECT = algo.o dataset.o main.o ptreelibre.o support.o

DEST = mtminer

${DEST}: ${OBJECT}
	g++ -O3 ${OBJECT} -o mtminer

%.o : %.cpp %.h
	g++ -c -O3 $<

clean:
	rm -f ${OBJECT} ${DEST}