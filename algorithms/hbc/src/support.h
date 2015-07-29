#include "dataset.h"

class Support {
public:
    Support();
    Support(ATT, Dataset* data);
    ~Support();
    OBJ frequency();
    void show();
    Support* intersection(Support*);
    void clean();
private:
    OBJ* m_objects;
    OBJ m_frequency;
};
