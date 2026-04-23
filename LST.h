#ifndef LST_H_INCLUDED
#define LST_H_INCLUDED

#include "ELTARTICLE.h"


typedef struct structNoeud {
    ELEMENT info;
    struct structNoeud *suivant;
} structNoeud, *NOEUD;

typedef struct {
    NOEUD tete;
    int   lg;
} laStruct, *LISTE;


#endif // LST_H_INCLUDED
