#include <iostream>
#include <fstream>
#include "constantes.h"
#include "support.h"
#include "dataset.h"
#include "ptreelibre.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------

static ATT tmpAtt[MAX_TRANS];
extern OBJ _gamma;
extern OBJ _delta;
extern bool _next;

// -------------------------------------------------------------------
//---------------------constructeurs----------------------------------

PTreeLibre::PTreeLibre(ATT j, bool motif, Support* support, PTreeLibre* aine, PTreeLibre* suivant) {
  m_value = j;
  m_motif = motif;
  m_suivant = suivant;
  m_aine = aine;
  m_support = support;
}

// -------------------------------------------------------------------

PTreeLibre::PTreeLibre(const PTreeLibre & p){
  m_value = p.m_value;
  m_motif = p.m_motif;
  m_suivant = p.m_suivant;
  m_aine = p.m_aine;
  m_support = p.m_support;
}

// -------------------------------------------------------------------
//---------------------destructeur------------------------------------

PTreeLibre::~PTreeLibre() {
  if (m_suivant) {
    delete m_suivant;
    m_suivant=NULL;
  }
  if (m_aine) {
    delete m_aine;
    m_aine=NULL;
  }
  if (m_support) {
    delete m_support;
    m_support=NULL;
  }
}


// -------------------------------------------------------------------
//---------------------initialize-------------------------------------

// Initialisation des singletons

void PTreeLibre::initialize(Dataset* data){
  for (ATT j = data->nbAtt(); j>0; j--) {
    m_aine = new PTreeLibre(j, true, new Support(j, data), NULL, m_aine);
  }
}

// -------------------------------------------------------------------
//---------------------candidates-------------------------------------

// G�n�ration de candidats
static PTreeLibre* root;
static OBJ m_freq;

PTreeLibre* PTreeLibre::candidates(unsigned short profondeur) {
	root=this; // root sert a avoir le debut de l'arbre
	return candidates(profondeur-1, 0); // et c'est parti !
}

void PTreeLibre::print(ATT l, ATT i) {
	for (ATT j=0; j<l+1; j++)
		if (j!=i)
			cout<<tmpAtt[j]<<" ";
	cout<<endl;
}

// v�rifie que tous les sous-motifs sont pr�sents
bool PTreeLibre::subPattern(int k) {
  ATT j=0;
  while (j<k) { // test des sous-motifs du candidat
    PTreeLibre* current=root;
    ATT l=0;
    while (l<=k+1) { // passer tous les attributs du sous-motif
      if (l==j)
	l++;
      while ((current)&&(current->m_value<tmpAtt[l])) { //se d�placer dans l'arbre
	current=current->m_suivant;
      }
      if ((!current)||(current->m_value>tmpAtt[l]))
	return false;
      if ((l==k+1)&&(current->m_support->frequency()<=m_freq+_delta)) 	// test de libert� !!!
	return false;
      current=current->m_aine;
      l++;
    }
    j++;
  }
  return true;
}

PTreeLibre* PTreeLibre::candidates(unsigned short profondeur, unsigned short index){
	if ((index<profondeur)&&(m_support)) {
		delete m_support;
		m_support=NULL;
	}
	// ins�rons dans notre tableau courant la valeur de ce noeud
	tmpAtt[index]=m_value;
	if (profondeur==index) {
	  //if (m_support->frequency()>= _gamma) {
	  //if (m_support->frequency()>(_gamma+_delta)) {
		// on est a la bonne profondeur !
		// Tentons de generer de nouveaux candidats...
		PTreeLibre* attribute=m_suivant; // attribute servira a aller chercher de potentiel attributs
		PTreeLibre* insertion=NULL; // endroit d'insertion...
		// on �num�reles candidats
		while (attribute) {
			tmpAtt[index+1]=attribute->m_value; // le motif courant dans tmpAtt est notre candidat			
			//calculons son support
			// intersection des 2 supports
			Support* supp=m_support->intersection(attribute->m_support);
			
			// si il exc�de notre seuil de fr�quence il faut enumerer les sous-motifs
			//if (supp->frequency()>0) {
			if (((supp->frequency()) < m_support->frequency()) && ((supp->frequency()) < attribute->m_support->frequency())){//&&(attribute->m_support->frequency()>(_gamma+_delta))) { //libert�
			  // insertion du motif si tout est ok
			  m_freq=supp->frequency();
			  if (subPattern(index)) {
			    if(supp->frequency()){
			      if (insertion) {
				if (m_aine)
				  insertion->m_suivant=new PTreeLibre(tmpAtt[index+1], true, supp, NULL, NULL);
				insertion=insertion->m_suivant;
			      }
			      else { // cas ou le motif a inserer est juste l'aine
				_next=true;
				PTreeLibre* tmp=new PTreeLibre(tmpAtt[index+1], true, supp, NULL, NULL);
				m_aine=tmp;
				insertion=m_aine;
			      }
			    }
			    else{
			      for (ATT i=0; i<index+2; i++)
				cout<<tmpAtt[i]<<" ";
			      cout<<endl;
			      delete supp;
			    }
			  }
			  else
			    delete supp;
			}
			else delete supp;
			attribute=attribute->m_suivant;
                }
		//}
		// le motif courant n'est plus vraiment interessant
		m_motif=false;
		m_support->clean();
	}
	else
	  // sinon allons plus bas... si possible.
  if (m_aine)
    m_aine=m_aine->candidates(profondeur, index+1);
	// on a aussi des copains a suivre... si possible.
	if (m_suivant)
	  m_suivant=m_suivant->candidates(profondeur, index);
	if (!m_aine) {
	  PTreeLibre *suivant=m_suivant;
	  m_suivant=NULL;
  delete this;
  return suivant;
	}
	return this;
	
//tmpAtt[] = m_value;
//cout << "profondeur: " << profondeur << endl;
	//cout << m_value << endl;
	
	// Morceau C�line
	/*  if (m_value) tmpAtt[2-] = m_value;
	    
  if (!profondeur) cout << "fin" << endl;
  else {
  if (m_aine) m_aine->candidates(profondeur-1);
  if (m_suivant) m_suivant->candidates(profondeur);
  }*/
	//cout << "profondeur2: " << profondeur << endl;
}

// -------------------------------------------------------------------
// --------------------affichage--------------------------------------

void PTreeLibre::list() {
	list(0);
}

void PTreeLibre::list(ATT index) {
	tmpAtt[index++]=m_value;
	if (m_motif) {
		for (ATT i=0; i<index; i++)
			cout<<tmpAtt[i]<<" ";
//		if (m_support)
//			m_support->show();
		cout<<endl;
	}
	if (m_aine)
		m_aine->list(index);
	if (m_suivant)
		m_suivant->list(--index);
}

void PTreeLibre::show(){
  PTreeLibre* temp = m_aine;

  while (temp) {
    cout << (int)temp->m_value << ": ";
    temp->m_support->show();
    temp = temp->m_suivant;
  }

}
