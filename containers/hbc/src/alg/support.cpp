#include <iostream>
#include <fstream>
#include "constantes.h"
#include "dataset.h"
#include "support.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------

static OBJ* tmpObj = new OBJ[MAX_TRANS];

// -------------------------------------------------------------------
//---------------------constructeurs----------------------------------

Support::Support() {
  m_objects=NULL;
  m_frequency=0;
}

// -------------------------------------------------------------------

Support::Support(ATT j, Dataset* data) {
  m_frequency = 0;
  
  for (OBJ i = 0; i<data->nbObj(); i++) {
    if (!data->getValue(i, j)) {
      tmpObj[m_frequency] = i+1;
      m_frequency++;
    }
  }
  
  m_objects = new OBJ[m_frequency];
  
  for (OBJ i = 0; i<m_frequency; i++)  m_objects[i] = tmpObj[i];
  
}

// -------------------------------------------------------------------
//---------------------destructeur------------------------------------

Support::~Support() {
  if (m_objects) {
    delete m_objects;
    m_objects=NULL;
  }
}

// -------------------------------------------------------------------
//---------------------fr�quence--------------------------------------

OBJ Support::frequency(){return m_frequency;}

// -------------------------------------------------------------------
//---------------------affichage--------------------------------------

void Support::show(){
  //OBJ* temp = m_objects;
  cout << "support: ";

  for (OBJ i = 0; i<m_frequency; i++) {
    cout << *(m_objects + i) << ' ';
  }

  cout << "fr�quence: " << int(m_frequency) << endl;
}

Support* Support::intersection(Support* s) {
	OBJ index=0, i1=0, i2=0;
	while ((i1<m_frequency) && (i2<s->m_frequency)) {
		if (m_objects[i1]==s->m_objects[i2]) {
			tmpObj[index++]=m_objects[i1];
			i1++;
			i2++;
		}
		else
			if (m_objects[i1]>s->m_objects[i2])
				i2++;
			else
				i1++;
	}
	Support* tmp= new Support();
	tmp->m_frequency=index;
	tmp->m_objects=new OBJ [index];
	for (OBJ i=0; i<index; i++)
		tmp->m_objects[i]=tmpObj[i];
	return tmp;
}

void Support::clean() {
  delete m_objects;
  m_objects=NULL;
}



