using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "constantes.h"
#include "dataset.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------

// -------------------------------------------------------------------
//---------------------constructeur-----------------------------------

Dataset::Dataset(char *nomFich) {
    m_dataset = NULL;
    read(nomFich);
    load(nomFich);
    //show();
}

// -------------------------------------------------------------------
//---------------------destructeur------------------------------------

Dataset::~Dataset(void) {
    if (m_dataset) {
        for (OBJ i = 0; i < m_nbObj; i++) {
            delete m_dataset[i];
            m_dataset[i] = NULL;
        }
        delete m_dataset;
        m_dataset = NULL;
    }
}

// -------------------------------------------------------------------
//---------------------read-------------------------------------------

// Initialise le format du jeu de donn�es

void Dataset::read(char *nomFich) {
    char *buffer = new char [MAX_TRANS];
    m_nbObj = 0;
    m_nbAtt = 0;
    ifstream *file = new ifstream(nomFich);
    if (*file == NULL) {
        cerr << "the input file " << nomFich << " could not be found." << endl;
        exit(EXIT_FAILURE);
    }
    while (!file->eof()) {
        file->getline(buffer, MAX_TRANS, '\n');
        if ((strlen(buffer) > 0) && (buffer[0] != '#')) {
            m_nbObj++;
            unsigned long i = 0, j = 0;
            while ((buffer[i] != '\0') && (buffer[i] != '\n')) {
                if (buffer[i] == ' ') {
                    buffer[i] = '\0';
                    ATT tmp = atoi(&buffer[j]);
                    if (tmp > m_nbAtt)
                        m_nbAtt = tmp;
                    j = i + 1;
                }
                i++;
            }
            buffer[i] = '\0';
            ATT tmp = atoi(&buffer[j]);
            if (tmp > m_nbAtt)
                m_nbAtt = tmp;
        }
    }
    file->close();
    m_dataset = new bool* [m_nbObj];
    for (OBJ i = 0; i < m_nbObj; i++)
        m_dataset[i] = new bool [m_nbAtt + 1];
    for (OBJ i = 0; i < m_nbObj; i++)
        for (ATT j = 0; j < m_nbAtt + 1; j++)
            //m_dataset[i][j] = true;
            m_dataset[i][j] = false;
    delete buffer;
}

// -------------------------------------------------------------------
//---------------------load-------------------------------------------

// Charge le jeu de donn�es

void Dataset::load(char *nomFich) {
    OBJ obj = 0;
    char *buffer = new char [MAX_TRANS];
    fstream file(nomFich);
    while (!file.eof()) {
        file.getline(buffer, MAX_TRANS, '\n');
        if ((strlen(buffer) > 0) && (buffer[0] != '#')) {
            unsigned long i = 0, j = 0;
            while ((buffer[i] != '\0') && (buffer[i] != '\n')) {
                if (buffer[i] == ' ') {
                    buffer[i] = '\0';
                    int tmp = atoi(&buffer[j]);
                    if (tmp > 0) {
                        //m_dataset[obj][tmp] = false;
                        m_dataset[obj][static_cast<ATT>(tmp)] = true;
                        attributs.insert(static_cast<ATT>(tmp));
                    }
                    j = i + 1;
                }
                i++;
            }
            buffer[i] = '\0';
            ATT tmp = atoi(&buffer[j]);
            if (tmp > 0){
                //m_dataset[obj][tmp] = false;
                m_dataset[obj][tmp] = true;
                attributs.insert(static_cast<ATT>(tmp));
            }
            obj++;
        }
    }
    file.close();
    delete buffer;
}

void Dataset::show() {
    for (OBJ i = 0; i < m_nbObj; i++) {
        for (ATT j = 1; j < m_nbAtt + 1; j++)
            if (!m_dataset[i][j])
                cout << j << " ";
        cout << endl;
    }
    cout << "attributs" << endl;
    for (set<ATT>::iterator i = attributs.begin(); i != attributs.end(); i++)
        cout << *i << ' ';
    cout << endl;
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------

bool Dataset::getValue(OBJ i, ATT j) {
    return m_dataset[i][j];
}

// -------------------------------------------------------------------
//---------------------nbAtt------------------------------------------

// Retourne le nb d'attributs

ATT Dataset::nbAtt() {
    return m_nbAtt;
}

// -------------------------------------------------------------------
//---------------------nbObj------------------------------------------

// Retourne le nb d'objets

OBJ Dataset::nbObj() {
    return m_nbObj;
}

