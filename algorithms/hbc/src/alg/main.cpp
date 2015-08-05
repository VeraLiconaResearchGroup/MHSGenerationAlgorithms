using namespace std;

#include <iostream>
#include "constantes.h"
#include "dataset.h"
#include "support.h"
#include "ptreelibre.h"
#include "algo.h"
#include <stdlib.h>

OBJ _gamma = 0;
OBJ _delta = 0;

void out_of_memory(void) {
    cerr << "memory exhausted" << endl;
    exit(1);
}

//--------------------------------- MAIN ------------------------------

int main(int argc, char * const argv[]) {
    set_new_handler(out_of_memory);
    cout << argv[1] << endl;
    if (argc != 2){
        cerr << "error usage : " << argv[0] << " <input file>" << endl;
        return 1;
    }
    Algo algo(argv[1]);
    algo.run();
    return 0;
}
