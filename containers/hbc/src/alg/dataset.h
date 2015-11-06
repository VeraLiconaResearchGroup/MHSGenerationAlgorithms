using namespace std;

#ifndef DATASET
#define DATASET

#include <set>

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

class Dataset {

public:
  // ensemble des attributs
  set<ATT> attributs;

  Dataset();
  Dataset(char*);
  ~Dataset(void);
  void show();

  bool getValue(OBJ i, ATT j);

  ATT nbAtt();
  OBJ nbObj();

private:
  void read(char *nomFich);
  void load(char *nomFich);

  ATT m_nbAtt;
  OBJ m_nbObj;
  bool** m_dataset;

};

#endif
