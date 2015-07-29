using namespace std;

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "constantes.h"
#include "dataset.h"
#include "support.h"
#include "ptreelibre.h"
#include "algo.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------

extern OBJ _gamma;
extern OBJ _delta;
bool _next;

// -------------------------------------------------------------------
//---------------------constructeur-----------------------------------

Algo::Algo(char *nomFich) {
    m_dataset = new Dataset(nomFich);
    //m_dataset->show();puts("toto");
    m_libres = NULL;
}

// -------------------------------------------------------------------
//---------------------destructeur------------------------------------

Algo::~Algo(void) {
    if (m_dataset) {
        delete m_dataset;
        m_dataset = NULL;
    }
}

// -------------------------------------------------------------------

void Algo::run() {
    for (ATT j = m_dataset->nbAtt(); j > 0; j--) {
        if (m_dataset->attributs.find(j) != m_dataset->attributs.end()){
            Support* tmp = new Support(j, m_dataset);
            if (tmp->frequency() >= 1 && tmp->frequency() < m_dataset->nbObj()) {
                m_libres = new PTreeLibre(j, true, tmp, NULL, m_libres);
            } else {
                cout << j << ' ' << endl;
                delete tmp;
            }
        }
    }
    delete m_dataset;
    m_dataset = NULL;

    if (m_libres) {
        _next = true;
    } else
        _next = false;

    ATT k = 1;
    while (_next) {
        _next = false;
        cerr << "# depth " << k + 1 << endl;
        if (m_libres) m_libres = m_libres->candidates(k);
        k++;
    }
}
