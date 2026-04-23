#include <stdio.h>
#include <stdlib.h>
#include "LSTPRIM.H"

NOEUD noeudCreer(ELEMENT e){
    NOEUD n;
    n =(NOEUD)malloc(sizeof(structNoeud));
    if(!n)
    printf ("\nPlus d'espace");
    else
        {
        elementAffecter(&n->info, e);
        n->suivant = NULL;
        }
return n;
}

void noeudDetruire(NOEUD n){
    free(n);
}


LISTE listeCreer(void) {
    LISTE L = (LISTE) malloc(sizeof(laStruct));
    if (!L)
        printf("\nPlus d'espace");
    else {
        L->lg   = 0;
        L->tete = NULL;
    }
    return L;
}

void listeDetruire(LISTE L) {
    NOEUD p, q = L->tete;
    int i;
    for (i = 1; i <= L->lg; i++) {
        p = q;
        q = q->suivant;
        noeudDetruire(p);
    }
    free(L);
}

int estVide(LISTE L) {
    return (L->lg == 0);
}

int estSaturee(LISTE L) {
    NOEUD temp = (NOEUD) malloc(sizeof(structNoeud));
    int sature =1;
    if (temp != NULL){
        sature =0;
        free(temp);
    }
    return sature;
}

int listeTaille(LISTE L) {
    return L->lg;
}

int inserer(LISTE L, ELEMENT e, int pos) {
    int succee = 1, i;
    NOEUD n, p, q;
    if (estSaturee(L)) {
        printf("\nListe saturee");
        succee = 0;
    }
    else if (pos < 1 || pos > L->lg + 1) {
        printf("\nPosition invalide");
        succee = 0;
    }
    else {
        n = noeudCreer(e);
        if (pos == 1) {
            n->suivant = L->tete;
            L->tete = n;
        }
        else {
            q = L->tete;
            for (i = 1; i < pos; i++) {
                p = q;
                q = q->suivant;
                }
            p->suivant = n;
            n->suivant = q;
        }
        (L->lg)++;
    }
    return succee;
}

int supprimer(LISTE L, int pos) {
    int succee = 1, i;
    NOEUD p = NULL, q = NULL;

    if (estVide(L)) {
        printf("\n[Erreur] Liste vide");
        succee = 0;
    }
    else if (pos < 1 || pos > L->lg) {
        printf("\n[Erreur] Position %d invalide (Taille: %d)", pos, L->lg);
        succee = 0;
    }
    else {
        q = L->tete;
        if (pos == 1) {
            L->tete = L->tete->suivant;
        }
        else {
            i = 1;
            while (i < pos) {
                p = q;
                q = q->suivant;
                i++;
            }
            p->suivant = q->suivant;
        }
        noeudDetruire(q);
        L->lg--;
    }
    return succee;
}

ELEMENT recuperer(LISTE L, int pos) {
    ELEMENT elt = elementCreer();
    int i;
    NOEUD p;
    if (estVide(L))
        printf("\nListe vide");
    else if (pos < 1 || pos > L->lg)
        printf("\nPosition invalide");
    else {
        p = L->tete;
        for (i = 1; i < pos; i++) p = p->suivant;
        elementAffecter(&elt, p->info);
    }
    return elt;
}

void listeAfficher(LISTE L) {
    int i;
    NOEUD p = L->tete;
    for (i = 1; i <= L->lg; i++) {
        elementAfficher(p->info);
        p = p->suivant;
    }
}

LISTE listeCopier(LISTE L) {
    LISTE LR = listeCreer();
    int i;
    ELEMENT elt;
    for (i = 1; i <= L->lg; i++) {
        elt = elementCreer();
        elementCopier(&elt, recuperer(L, i));
        inserer(LR, elt, i);
    }
    return LR;
}

int listeComparer(LISTE L1, LISTE L2) {
    int test = 1, i = 1;
    if (listeTaille(L1) != listeTaille(L2))
        test = 0;
    while (i <= listeTaille(L1) && test) {
        if (elementComparer(recuperer(L1,i), recuperer(L2,i)) != 0)
            test = 0;
        i++;
    }
    return test;
}
