#ifndef ELTARTICLE_H_INCLUDED
#define ELTARTICLE_H_INCLUDED

#include "ARTICLESDD.h"

void err_num(char *msg);
void err_char(char *msg);
int verif_no_espace(char *msg);
int verif_num(char *msg);

void    elementLire(ELEMENT *);
void    elementAfficher(ELEMENT);
void    elementAffecter(ELEMENT *, ELEMENT);
void    elementCopier(ELEMENT *, ELEMENT);
int     elementComparer(ELEMENT, ELEMENT);
ELEMENT elementCreer(void);
void    elementDetruire(ELEMENT);

#endif // ELTARTICLE_H_INCLUDED
