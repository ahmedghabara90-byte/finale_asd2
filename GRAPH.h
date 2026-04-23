#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "ELTARTICLE.h"
#include "LSTPRIM.h"

typedef struct {
    int      V;
    ELEMENT *articles;
    LISTE   *adjList;
    int     *degre_in;
} grapheStruct, *grapheReseau;

#endif // GRAPH_H_INCLUDED
