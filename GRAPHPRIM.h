#ifndef GRAPHPRIM_H_INCLUDED
#define GRAPHPRIM_H_INCLUDED

#include "GRAPHSDD.h"

extern int prochainId;

void err_exist(char* msg);
void trierTab(ELEMENT tab[], int compteur);

void sauvegarderGraphe(grapheReseau g,const char *filename);
int cheminExists(grapheReseau g, int idSrc, int idDest);

grapheReseau creerGraphe(int V);
grapheReseau chargerGraphe(const char *filename);
void detruireGraphe(grapheReseau g);

int ajouterArticle(grapheReseau g, ELEMENT art);
int supprimerArticle(grapheReseau g, int idArt);
int ajouterCitation(grapheReseau g, int idSrc, int idDest);
int supprimerCitation(grapheReseau g, int idSrc, int idDest);

void afficherGraphe(grapheReseau g);

void articlesCites(grapheReseau g, int idSrc);
void articlesCitants(grapheReseau g, int idDest);
void sourcesOriginales(grapheReseau g);
void articlesIsoles(grapheReseau g);
ELEMENT articlePlusCite(grapheReseau g);

int comparerDates(ELEMENT art1, ELEMENT art2);
void trierParDate(grapheReseau g);
void premierCitant(grapheReseau g, int idDest);
void chainePropagation(grapheReseau g, int idSrc);

void simulerPropagation(grapheReseau g, int idSrc);
void articlesAccessibles(grapheReseau g, int idSrc);

int  analyserArticle(ELEMENT art);
void analyserReseau(grapheReseau g);
void articlesSuspectsCites(grapheReseau g);

void simulerSuppression(grapheReseau g, int idArt);
int neutraliserPropagation(grapheReseau g, int idSrc, int idDest);

#endif // GRAPHPRIM_H_INCLUDED
