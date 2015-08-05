class PTreeLibre {

public:
  PTreeLibre(ATT j=0, bool motif=false, Support* support = NULL, PTreeLibre* aine=NULL, PTreeLibre* suivant=NULL);
  PTreeLibre(const PTreeLibre & p);
  ~PTreeLibre();
  
  void initialize(Dataset* data);
  //void candidates(unsigned short profondeur);
  PTreeLibre* candidates(unsigned short profondeur);
  void show();
  void list();

private:
  bool subPattern(int);
  PTreeLibre* candidates(unsigned short, unsigned short );

  void print(ATT l, ATT i);
  void list(ATT);
  
  ATT m_value;
  bool m_motif;
  PTreeLibre* m_suivant;
  PTreeLibre* m_aine;
  Support* m_support;

};
