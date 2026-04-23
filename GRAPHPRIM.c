#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "GRAPHPRIM.h"
#include "fakesdb.h"

int prochainId = 0;


void err_exist(char *msg)
 {
    system("cls");
    system("color 4F");
    printf("\n\n\n\n\n\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                               ERREUR                                               ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    printf("                                       TYPE D'ERREUR : Aucun Resultat trouve.\n");
    printf("                                       DETAILS       : %s\n\n", msg);
    printf("                             ========================================================================================================\n");
    printf("                                                   Appuyez sur une touche pour retourner au menu principal...                         \n");
    printf("                             ========================================================================================================\n\n");
    system("pause > nul");
    system("color 07");
}

void trierTab(ELEMENT tab[], int compteur) {
    int k, l;
    ELEMENT temp;
    k = 1;
    while (k < compteur) {
        temp = tab[k];
        l = k - 1;
        while (l >= 0 && elementComparer(temp, tab[l]) == -1) {
            tab[l + 1] = tab[l];
            l--;
        }
        tab[l+1] = temp;
        k++;
    }
}

void sauvegarderGraphe(grapheReseau g, const char *filename) {
    int  i, j;
    FILE *f = fopen(filename, "w");

    if (!f)
        err_exist("Impossible d'ouvrir le fichier pour sauvegarde.");
    else {
        fprintf(f, "# Format : A id titre source score jour mois annee heure minute\n");
        fprintf(f, "# C id_src id_dest\n\n");

        for (i = 0; i < g->V; i++) {
            if (g->articles[i] != NULL)
                fprintf(f, "A %d \"%s\" %s %d %d %d %d %d %d\n",
                    g->articles[i]->id, g->articles[i]->titre,
                    g->articles[i]->source, g->articles[i]->score_fiabilite,
                    g->articles[i]->jour, g->articles[i]->mois, g->articles[i]->annee,
                    g->articles[i]->heure, g->articles[i]->minute);
        }

        fprintf(f, "\n");

        for (i = 0; i < g->V; i++) {
            if (g->articles[i] != NULL) {
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    fprintf(f, "C %d %d\n", i, recuperer(g->adjList[i], j)->id);
                }
            }
        }
        fclose(f);
        prochainId = g->V;
    }
}

grapheReseau creerGraphe(int V) {
    int i;
    grapheReseau g = (grapheReseau) malloc(sizeof(grapheStruct));
    if (!g) {
        printf("\nPlus d'espace memoire.");
        g = NULL;
    }
    else {
        g->V = V;
        g->articles = (ELEMENT *) malloc(V * sizeof(ELEMENT));
        g->adjList  = (LISTE *) malloc(V * sizeof(LISTE));
        g->degre_in = (int *)malloc(V * sizeof(int));

        if (!g->articles || !g->adjList || !g->degre_in) {
            printf("\nPlus d'espace memoire.");
            g = NULL;
        }
        else {
            for (i = 0; i < V; i++) {
                g->articles[i] = NULL;
                g->adjList[i]  = listeCreer();
                g->degre_in[i] = 0;
            }
        }
    }
    return g;
}

grapheReseau chargerGraphe(const char *filename) {
    FILE *f;
    char ligne[300];
    int V = 0;
    grapheReseau g = NULL;
    ELEMENT art;
    int src, dest;

    f = fopen(filename, "r");
    if (!f) {
        char message[150];
        strcpy(message, "Impossible de trouver le fichier : ");
        strcat(message, filename);
        err_exist(message);
    }
    else {
        while (fgets(ligne, sizeof(ligne), f)){
            if (ligne[0] == 'A')
                V++;
        }
        fclose(f);

        if (V == 0) {
            err_exist("Pas d'article dans le fichier.");
        }
        else {
            g = creerGraphe(V);
            if (g) {
                f = fopen(filename, "r");
                while (fgets(ligne, sizeof(ligne), f)) {
                    if (ligne[0] == 'A') {
                        art = elementCreer();
                        sscanf(ligne,"A %d \"%99[^\"]\" %49s %d %d %d %d %d %d",
                            &art->id, art->titre, art->source, &art->score_fiabilite,
                            &art->jour, &art->mois, &art->annee, &art->heure, &art->minute);
                        ajouterArticle(g, art);
                    }
                    else if (ligne[0] == 'C') {
                        sscanf(ligne, "C %d %d", &src, &dest);
                        ajouterCitation(g, src, dest);
                    }
                }
                fclose(f);
                prochainId = g->V;
            }
        }
    }
    return g;
}

void detruireGraphe(grapheReseau g) {
    int i;
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL)
            elementDetruire(g->articles[i]);
        listeDetruire(g->adjList[i]);
    }
    free(g->articles);
    free(g->adjList);
    free(g->degre_in);
    free(g);
}


int ajouterArticle(grapheReseau g, ELEMENT art) {
    int succee = 1;
    int i;

    if (art->id < 0) {
        err_num("L'ID doit etre positif.");
        succee = 0;
    }
    else {
        if (art->id >= g->V) {
            int oldV = g->V;
            int newSize = art->id + 1;
            ELEMENT *tempArticles = (ELEMENT *) realloc(g->articles, newSize * sizeof(ELEMENT));
            LISTE *tempAdj = (LISTE *) realloc(g->adjList,  newSize * sizeof(LISTE));
            int *tempDeg = (int *) realloc(g->degre_in, newSize * sizeof(int));

            if (!tempArticles || !tempAdj || !tempDeg) {
                printf("    [!] Erreur Memoire : Impossible d'agrandir le reseau.");
                succee = 0;
            }
            else {
                g->articles = tempArticles;
                g->adjList = tempAdj;
                g->degre_in = tempDeg;
                for (i = oldV; i < newSize; i++) {
                    g->articles[i] = NULL;
                    g->adjList[i] = listeCreer();
                    g->degre_in[i] = 0;
                }
                g->V = newSize;
            }
        }

        if (succee && g->articles[art->id] != NULL) {
            err_num("Essayez un ID unique dans le reseau.");
            succee = 0;
        }

        if (succee && art->id > 0 && g->articles[art->id - 1] == NULL) {
            err_num("L'ID precedent n'existe pas encore, respectez l'ordre.");
            succee = 0;
        }

        if (succee) {
            g->articles[art->id] = art;
            g->degre_in[art->id] = 0;
        }
    }
    return succee;
}

int supprimerArticle(grapheReseau g, int idArt) {
    int i, j;
    int trouve, succee;

    succee = idArt >= 0 && idArt < g->V && g->articles[idArt] != NULL;
    if (succee) {
        i = 0;
        while (i < g->V) {
            if (g->articles[i] != NULL && i != idArt) {
                j = 1;
                trouve = 0;
                while (j <= listeTaille(g->adjList[i]) && !trouve) {
                    ELEMENT e = recuperer(g->adjList[i], j);
                    if (e != NULL && e->id == idArt) {
                        supprimer(g->adjList[i], j);
                        trouve = 1;
                    }
                    else {
                        j++;
                    }
                }
            }
            i++;
        }

        listeDetruire(g->adjList[idArt]);
        elementDetruire(g->articles[idArt]);

        i = idArt;
        while (i < g->V - 1) {
            g->articles[i] = g->articles[i + 1];
            g->adjList[i]  = g->adjList[i + 1];
            g->degre_in[i] = g->degre_in[i + 1];

            if (g->articles[i] != NULL) {
                g->articles[i]->id = i;
            }
            i++;
        }

        g->articles[g->V - 1] = NULL;
        g->adjList[g->V - 1]  = NULL;
        g->degre_in[g->V - 1] = 0;

        g->V--;

        i = 0;
        while (i < g->V) {
            if (g->articles[i] != NULL) {
                j = 1;
                while (j <= listeTaille(g->adjList[i])) {
                    ELEMENT e = recuperer(g->adjList[i], j);
                    if (e != NULL && e->id > idArt) {
                        e->id = e->id - 1;
                    }
                    j++;
                }
            }
            i++;
        }

        prochainId = g->V;
    }
    else {
        err_exist("Cet article est introuvable dans le graphe.");
    }

    return succee;
}

int ajouterCitation(grapheReseau g, int idSrc, int idDest) {
    int j = 1;
    int existe = 0;
    int succee = 1;

    if (idSrc  < 0 || idSrc  >= g->V || g->articles[idSrc]  == NULL)
        succee = 0;
    else if (idDest < 0 || idDest >= g->V || g->articles[idDest] == NULL)
        succee = 0;
    else if (idSrc == idDest)
        succee = 0;
    else if (elementComparer(g->articles[idSrc], g->articles[idDest]) != 1)
        succee = 0;
    else {
        while (j <= listeTaille(g->adjList[idSrc]) && !existe) {
            if (recuperer(g->adjList[idSrc], j)->id == idDest)
                existe = 1;
            j++;
        }
        if (existe)
            succee = 0;
    }

    if (succee) {
        inserer(g->adjList[idSrc], g->articles[idDest],listeTaille(g->adjList[idSrc]) + 1);
        g->degre_in[idDest]++;
    }
    else {
        char msg[100];
        sprintf(msg,"Citation (%d --> %d) deja existante, ou source publiee avant destination.", idSrc, idDest);
        err_num(msg);
    }
    return succee;
}

int supprimerCitation(grapheReseau g, int idSrc, int idDest) {
    int j = 1;
    int succee = 0;

    if (idSrc  >= 0 && idSrc  < g->V && g->articles[idSrc]  != NULL && idDest >= 0 && idDest < g->V && g->articles[idDest] != NULL) {
        while (j <= listeTaille(g->adjList[idSrc]) && !succee) {
            if (recuperer(g->adjList[idSrc], j)->id == idDest) {
                supprimer(g->adjList[idSrc], j);
                g->degre_in[idDest]--;
                succee = 1;
            }
            j++;
        }
    }

    if (!succee)
        err_exist("Citation ou articles introuvables dans le graphe.");

    return succee;
}

void afficherGraphe(grapheReseau g) {
    int i, j;
    ELEMENT dest;

    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                           RESEAU D'INFORMATION  ||  GRAPHE DE CITATIONS                            ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    Sleep(200);

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL) {
            Sleep(50);
            printf("                             ========================================================================================================\n");
            printf("                             ||                                                                                                    ||\n");
            printf("                             ||   [ID %02d]  %-88s||\n", i, g->articles[i]->titre);
            printf("                             ||   Source : %-88s||\n", g->articles[i]->source);
            printf("                             ||   Score  : %02d /100 %-80s||\n",g->articles[i]->score_fiabilite,"");
            printf("                             ||   Date   : %02d/%02d/%04d ; %02dh%02d%-70s||\n",
              g->articles[i]->jour, g->articles[i]->mois, g->articles[i]->annee,g->articles[i]->heure, g->articles[i]->minute,"");
            printf("                             ||                                                                                                    ||\n");
            printf("                             ========================================================================================================\n\n");

            if (listeTaille(g->adjList[i]) == 0) {
                printf("                                       --> [!] Ne cite aucun article\n");
            }
            else
                {
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    dest = recuperer(g->adjList[i], j);
                    printf("                                       \xB3\n");

                    printf("                                       \xC3\xC4\x10 [%d] %s\n", dest->id, dest->titre);
                }
            }
            printf("\n\n\n");
        }
    }
}

void articlesCites(grapheReseau g, int idSrc) {
    int j;
    ELEMENT dest;

    if (idSrc < 0 || idSrc >= g->V || g->articles[idSrc] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                         ARTICLES CITES                                             ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n");
        printf("                                       Par : [ID %02d] %s\n\n", idSrc, g->articles[idSrc]->titre);

        if (listeTaille(g->adjList[idSrc]) == 0)
            printf("                                       [!] Aucun article cite\n");
        else {
            for (j = 1; j <= listeTaille(g->adjList[idSrc]); j++) {
                Sleep(50);
                dest = recuperer(g->adjList[idSrc], j);
                printf("                                       \xB3\n");
                printf("                                       \xC3\xC4\x10 [ID %02d] %s\n", dest->id, dest->titre);
            }
        }
        printf("\n");
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}

void articlesCitants(grapheReseau g, int idDest) {
    int i, j, trouve;
    trouve = 0;

    if (idDest < 0 || idDest >= g->V || g->articles[idDest] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                        ARTICLES CITANTS                                            ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n");
        printf("\n                                       Article : [ID %02d] %s\n\n\n", idDest, g->articles[idDest]->titre);

        for (i = 0; i < g->V; i++) {
            if (g->articles[i] != NULL) {
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    if (recuperer(g->adjList[i], j)->id == idDest) {
                        Sleep(50);
                        printf("                                       --> [ID %02d] %s\n", g->articles[i]->id, g->articles[i]->titre);
                        trouve = 1;
                    }
                }
            }
        }
        if (!trouve)
            printf("                                        [!] Aucun article ne cite cet article\n");

        printf("\n");
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}

void sourcesOriginales(grapheReseau g) {
    int i, trouve = 0;
    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                         SOURCES ORIGINALES                                         ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL && listeTaille(g->adjList[i]) == 0) {
                Sleep(50);
            printf("                                       --> [ID %02d] %s\n", i, g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve)
        printf("                                        [!] Aucune source originale\n");

    printf("\n");
    printf("                             ========================================================================================================\n\n");
}

void articlesIsoles(grapheReseau g) {
    int i, trouve = 0;

    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                           ARTICLES ISOLES                                          ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL && g->degre_in[i] == 0) {
                Sleep(50);
            printf("                                       --> [ID %02d] %s\n", i, g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve)
        printf("                                        [!] Aucun article isole\n");

    printf("\n");
    printf("                             ========================================================================================================\n\n");
}

ELEMENT articlePlusCite(grapheReseau g) {
    int i, max = -1;
    ELEMENT res = NULL;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL && g->degre_in[i] > max) {
            max = g->degre_in[i];
            res = g->articles[i];
        }
    }
    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                    ARTICLE(S) LE(S) PLUS CITE(S)                                   ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");

    if (max <= 0) {
        printf("                                       [!] Aucun article n'est cite dans le graphe. \n");
    } else {
        for (i = 0; i < g->V; i++) {
            if (g->articles[i] != NULL && g->degre_in[i] == max) {
                    Sleep(50);
                printf("                                       --> [ID %02d] %-55s  cite par %d article(s)\n", i, g->articles[i]->titre, max);
            }
        }
    }

    printf("\n");
    printf("                             ========================================================================================================\n\n");
    system("pause");

    return res;
}



void trierParDate(grapheReseau g) {
    int i, compteur = 0;
    ELEMENT tab[100];

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL) {
            tab[compteur] = g->articles[i];
            compteur++;
        }
    }

    trierTab(tab, compteur);
    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                 ARTICLES TRIES PAR DATE DE PUBLICATION                             ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");

    for (i = 0; i < compteur; i++) {
            Sleep(50);
        printf("                                  %2d >>  [ID %02d] %-55s  {%02d/%02d/%04d ; %02dh%02d}\n",
            i + 1, tab[i]->id, tab[i]->titre, tab[i]->jour, tab[i]->mois, tab[i]->annee, tab[i]->heure, tab[i]->minute);
    }

    printf("\n");
    printf("                             ========================================================================================================\n\n");
}

void premierCitant(grapheReseau g, int idDest) {
    int i, j;
    ELEMENT premier = NULL;

    if (idDest < 0 || idDest >= g->V || g->articles[idDest] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        i = 0;
        while (i < g->V) {
            if (g->articles[i] != NULL && i != idDest) {
                j = 1;
                while (j <= listeTaille(g->adjList[i])) {
                    ELEMENT v = recuperer(g->adjList[i], j);
                    if (v != NULL && v->id == idDest) {
                        if (premier == NULL || elementComparer(g->articles[i], premier) == -1)
                            premier = g->articles[i];
                    }
                    j++;
                }
            }
            i++;
        }

        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                      PREMIER ARTICLE CITANT                                        ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n\n");

        printf("                                      Article cible : [ID %02d] %-45s (%02d/%02d/%04d  %02dh%02d)\n\n",
                idDest, g->articles[idDest]->titre, g->articles[idDest]->jour, g->articles[idDest]->mois, g->articles[idDest]->annee, g->articles[idDest]->heure, g->articles[idDest]->minute);

        if (premier == NULL)
            printf("                                        [!] Aucun article ne cite cet article.\n");

        else
            printf("                                           --> [ID %02d] %-55s  %02d/%02d/%04d  %02dh%02d\n",
                    premier->id, premier->titre, premier->jour, premier->mois, premier->annee, premier->heure, premier->minute);


        printf("\n");
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}

void chainePropagation(grapheReseau g, int idSrc) {
    int visited[100];
    int i, j, k, change, compteur;
    ELEMENT tab[100];

    if (idSrc < 0 || idSrc >= g->V || g->articles[idSrc] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        for (i = 0; i < g->V; i++) visited[i] = 0;

        visited[idSrc] = 1;
        tab[0] = g->articles[idSrc];
        compteur = 1;
        do {
            change = 0;
            for (i = 0; i < g->V; i++) {
                if (g->articles[i] != NULL && !visited[i]) {
                    j = 1;
                    while (j <= listeTaille(g->adjList[i]) && !visited[i]) {
                        if (visited[recuperer(g->adjList[i], j)->id]) {
                            visited[i] = 1;
                            tab[compteur] = g->articles[i];
                            compteur++;
                            change = 1;
                        }
                        j++;
                    }
                }
            }
        } while (change);

        trierTab(tab, compteur);
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                       CHAINE DE PROPAGATION                                        ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n\n");
        printf("                                    Depuis : [ID %02d] %s\n\n\n ", idSrc, g->articles[idSrc]->titre);

        if (compteur == 1) {
            printf("                                      [ID %02d] %-60s  %02d/%02d/%d  %02dh%02d\n",
                tab[0]->id, tab[0]->titre, tab[0]->jour, tab[0]->mois, tab[0]->annee, tab[0]->heure, tab[0]->minute);
            printf("\n                                          [!] Aucun article ne propage cet article.\n");
        }
        else {
            printf("                                      [ID %02d] %-60s  %02d/%02d/%d  %02dh%02d\n",
                tab[0]->id, tab[0]->titre, tab[0]->jour, tab[0]->mois, tab[0]->annee, tab[0]->heure, tab[0]->minute);
            for (k = 1; k < compteur; k++) {
                Sleep(50);
                printf("                                          |\n");
                printf("                                          v\n");
                printf("                                       [ID %02d] %-60s  %02d/%02d/%d  %02dh%02d\n",
                    tab[k]->id, tab[k]->titre, tab[k]->jour, tab[k]->mois, tab[k]->annee, tab[k]->heure, tab[k]->minute);
            }
        }

        printf("\n\n");
        printf("                                    Total : %d article(s) dans la chaine.\n", compteur);
        printf("\n");
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}



int analyserArticle(ELEMENT art) {
    int score_suspicion = 0;
    int i, j, m;
    int trouve, test;
    char titre_minuscule[100];

    m =0;
    while (art->titre[m] != '\0') {
        if (art->titre[m] >= 'A' && art->titre[m] <= 'Z')
            titre_minuscule[m] = art->titre[m] + 32;
        else if (art->titre[m]=='_')
            titre_minuscule[m] = ' ';
        else
            titre_minuscule[m] = art->titre[m];
        m++;
    }
    titre_minuscule[m] = '\0';
    test = 0;
    for (i = 0; i < NB_FAKES; i++) {
        trouve = 0;
        j = 0;
        while (titre_minuscule[j] != '\0' && trouve == 0) {
            m = 0;
            while (BASE_FAKES[i][m] != '\0' && titre_minuscule[j+m] == BASE_FAKES[i][m])
                m++;

            if (BASE_FAKES[i][m] == '\0'){
                trouve = 1;
                test = 1;
            }
            j++;
        }
    }
    if (test == 1)
        score_suspicion += 40;

    for (i = 0; i < NB_SUSPECTS; i++) {
        trouve = 0;
        j = 0;
        while (titre_minuscule[j] != '\0' && trouve == 0) {
            m = 0;
            while (MOTS_SUSPECTS[i][m] != '\0' && titre_minuscule[j+m] == MOTS_SUSPECTS[i][m])
                m++;
            if (MOTS_SUSPECTS[i][m] == '\0') {
                trouve = 1;
                score_suspicion += 10;
            }
            j++;
        }
    }

    art->score_fiabilite = 100 - score_suspicion;
    if (art->score_fiabilite < 0)
        art->score_fiabilite = 0;


    return score_suspicion;
}

void analyserReseau(grapheReseau g) {
    int i, j, k, m;
    int trouve;
    char titre_minuscule[100];
    ELEMENT temp;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL)
            analyserArticle(g->articles[i]);
    }

    for (k = 1; k < g->V; k++) {
        if (g->articles[k] != NULL) {
            temp = g->articles[k];
            j = k - 1;
            while (j >= 0 && g->articles[j] != NULL && g->articles[j]->score_fiabilite > temp->score_fiabilite) {
                g->articles[j + 1] = g->articles[j];
                j--;
            }
            g->articles[j + 1] = temp;
        }
    }
    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                    ANALYSE DES FAKE NEWS                                           ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n\n");

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL) {
            Sleep(50);

            m = 0;
            while (g->articles[i]->titre[m] != '\0') {
                if (g->articles[i]->titre[m] >= 'A' && g->articles[i]->titre[m] <= 'Z')
                    titre_minuscule[m] = g->articles[i]->titre[m] + 32;
                else if (g->articles[i]->titre[m] == '_')
                    titre_minuscule[m] = ' ';
                else
                    titre_minuscule[m] = g->articles[i]->titre[m];
                m++;
            }
            titre_minuscule[m] = '\0';

            if (g->articles[i]->score_fiabilite < 40)
                printf("                                       [SUSPECT] ");
            else if (g->articles[i]->score_fiabilite < 70)
                printf("                                       [DOUTEUX] ");
            else
                printf("                                       [FIABLE ] ");

            printf("%-60s | Score : %d\n", g->articles[i]->titre, g->articles[i]->score_fiabilite);

            for (j = 0; j < NB_FAKES; j++) {
                trouve = 0;
                k = 0;
                while (titre_minuscule[k] != '\0' && !trouve) {
                    m = 0;
                    while (BASE_FAKES[j][m] != '\0' && titre_minuscule[k + m] != '\0' && titre_minuscule[k + m] == BASE_FAKES[j][m])
                        m++;
                    if (BASE_FAKES[j][m] == '\0')
                        trouve = 1;
                    k++;
                }
                if (trouve == 1)
                    printf("                                                  --> base fake     : \"%s\"\n", BASE_FAKES[j]);
            }

            for (j = 0; j < NB_SUSPECTS; j++) {
                trouve = 0;
                k  = 0;
                while (titre_minuscule[k] != '\0' && !trouve) {
                    m = 0;
                    while (MOTS_SUSPECTS[j][m] != '\0' && titre_minuscule[k + m] == MOTS_SUSPECTS[j][m])
                        m++;
                    if (MOTS_SUSPECTS[j][m] == '\0')
                        trouve = 1;
                    k++;
                }
                if (trouve)
                    printf("                                                  --> mot suspect   : \"%s\"\n", MOTS_SUSPECTS[j]);
            }
            printf("\n\n");
        }
    }

    printf("                             ========================================================================================================\n\n");
    for (k = 1; k < g->V; k++) {
        if (g->articles[k] != NULL) {
            temp = g->articles[k];
            j = k - 1;
            while (j >= 0 && g->articles[j] != NULL && g->articles[j]->id > temp->id) {
                g->articles[j + 1] = g->articles[j];
                j--;
            }
            g->articles[j + 1] = temp;
        }
    }
    system("pause");
}

void articlesSuspectsCites(grapheReseau g) {
    int i, j, k, compteur;
    ELEMENT tab[100];
    ELEMENT temp;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL)
            analyserArticle(g->articles[i]);
    }

    compteur = 0;
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != NULL && g->articles[i]->score_fiabilite < 40 && g->degre_in[i] > 0) {
            tab[compteur] = g->articles[i];
            compteur++;
        }
    }

    k = 1;
    while (k < compteur) {
        temp = tab[k];
        j = k - 1;
        while (j >= 0 && g->degre_in[tab[j]->id] < g->degre_in[temp->id]) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = temp;
        k++;
    }
    system("cls");
    printf("\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                ARTICLES SUSPECTS LES PLUS CITES                                    ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");

    if (compteur == 0)
        printf("                                        [!] Aucun article suspect cite\n");
    else {
        for (i = 0; i < compteur; i++) {
            printf("                                       --> [ID %02d] %-55s  \n",tab[i]->id, tab[i]->titre);
            printf("                                              score : %d   ;   cite par %d article(s)\n\n", tab[i]->score_fiabilite, g->degre_in[tab[i]->id]);
        }
    }

    printf("\n");
    printf("                             ========================================================================================================\n\n");
    system("pause");
}

void simulerPropagation(grapheReseau g, int idSrc) {
    int visited[100];
    int niveau[100];
    int i, j, niveauMax, k;
    LISTE file;
    ELEMENT courant;

    if (idSrc < 0 || idSrc >= g->V || g->articles[idSrc] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        for (i = 0; i < g->V; i++) {
            visited[i] = 0;
            niveau[i] = -1;
        }

        file = listeCreer();
        visited[idSrc] = 1;
        niveau[idSrc] = 0;
        inserer(file, g->articles[idSrc], listeTaille(file) + 1);

        while (!estVide(file)) {
            courant = recuperer(file, 1);
            supprimer(file, 1);
            j = 1;
            while (j <= listeTaille(g->adjList[courant->id])) {

                ELEMENT v = recuperer(g->adjList[courant->id], j);

                if (visited[v->id] == 0) {
                    visited[v->id] = 1;
                    niveau[v->id] = niveau[courant->id] + 1;
                    inserer(file, v, listeTaille(file) + 1);
                }

                j++;
            }
        }

        listeDetruire(file);

        niveauMax = 0;
        for (i = 0; i < g->V; i++) {
            if (niveau[i] > niveauMax)
                niveauMax = niveau[i];
        }
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                     SIMULATION DE PROPAGATION (BFS)                                ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n");
        printf("                                       Depuis : [ID %02d] %s\n\n", idSrc, g->articles[idSrc]->titre);

        for (k = 0; k <= niveauMax; k++) {
            printf("                                       Niveau %d :\n", k);
            for (i = 0; i < g->V; i++) {
                if (niveau[i] == k)
                    printf("                                           --> [ID %02d] %s\n", g->articles[i]->id, g->articles[i]->titre);
            }
            printf("\n");
        }

        j = 0;
        for (i = 0; i < g->V; i++) {
            if (visited[i])
                j++;
        }

        printf("                             ========================================================================================================\n\n");
        printf("                                       %d niveau(x), %d article(s) atteint(s).\n\n", niveauMax + 1, j);
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}

void articlesAccessibles(grapheReseau g, int idSrc) {
    int visited[100];
    int i, j, compteur;
    LISTE file;
    ELEMENT courant;

    if (idSrc < 0 || idSrc >= g->V || g->articles[idSrc] == NULL)
        err_num("Article introuvable dans le graphe.");
    else {
        for (i = 0; i < g->V; i++) visited[i] = 0;

        file = listeCreer();
        visited[idSrc] = 1;
        inserer(file, g->articles[idSrc], listeTaille(file) + 1);

        while (!estVide(file)) {
            courant = recuperer(file, 1);
            supprimer(file, 1);

            for (j = 1; j <= listeTaille(g->adjList[courant->id]); j++) {
                ELEMENT voisin = recuperer(g->adjList[courant->id], j);
                if (!visited[voisin->id]) {
                    visited[voisin->id] = 1;
                    inserer(file, voisin, listeTaille(file) + 1);
                }
            }
        }

        listeDetruire(file);
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                      ARTICLES ACCESSIBLES                                          ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n");
        printf("                                       Depuis : [ID %02d] %s\n\n", idSrc, g->articles[idSrc]->titre);

        compteur = 0;
        for (i = 0; i < g->V; i++) {
            if (visited[i]) {
                    Sleep(50);
                printf("                                       --> [ID %02d] %s\n", g->articles[i]->id, g->articles[i]->titre);
                compteur++;
            }
        }

        printf("\n");
        printf("                                       Total : %d article(s) accessible(s).\n", compteur);
        printf("\n");
        printf("                             ========================================================================================================\n\n");
    }
    system("pause");
}





int cheminExists(grapheReseau g, int idSrc, int idDest) {
    int visited[100];
    int i, j, result;
    LISTE file;
    ELEMENT courant;

    for (i = 0; i < g->V; i++) visited[i] = 0;

    file = listeCreer();
    visited[idSrc] = 1;
    inserer(file, g->articles[idSrc], listeTaille(file) + 1);
    result = 0;

    while (!estVide(file) && !result) {
        courant = recuperer(file, 1);
        supprimer(file, 1);
        j = 1;
        while (j <= listeTaille(g->adjList[courant->id])) {
            ELEMENT v = recuperer(g->adjList[courant->id], j);
            if (v->id == idDest) {
                result = 1;
            }
            else if (!visited[v->id]) {
                visited[v->id] = 1;
                inserer(file, v, listeTaille(file) + 1);
            }
            j++;
        }
    }
    listeDetruire(file);
    return result;
}

void simulerSuppression(grapheReseau g, int idArt) {
    int i, j, nbCitants, nbCites;
    int idsCitants[100];
    int nbC = 0;
    int trouve, aucun;

    if (idArt < 0 || idArt >= g->V || g->articles[idArt] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {

        nbCitants = g->degre_in[idArt];
        nbCites = listeTaille(g->adjList[idArt]);

        char titreSupprime[100];
        strcpy(titreSupprime, g->articles[idArt]->titre);


        for (i = 0; i < g->V; i++) {
            if (g->articles[i] != NULL && i != idArt) {
                trouve = 0;
                j = 1;
                while (j <= listeTaille(g->adjList[i]) && !trouve) {
                    if (recuperer(g->adjList[i], j)->id == idArt) {
                        idsCitants[nbC++] = i;
                        trouve = 1;
                    }
                    else
                        j++;
                }
            }
        }

        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                   SIMULATION DE SUPPRESSION                                        ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n\n\n");
        printf("                                       Article supprime : [ID %02d] %s\n\n", idArt,  titreSupprime);
        Sleep(50);



        printf("                                       Etait cite par          : ");
        if (nbC == 0)
            printf("Aucun");
        for (i = 0; i < nbC; i++) {
            printf("\n                                           --> [ID %02d] %s", g->articles[idsCitants[i]]->id, g->articles[idsCitants[i]]->titre);
        }
        printf("\n                                           (%d) article(s).\n\n", nbCitants);


        printf("                                       Citait                  : ");
        if (nbCites == 0)
            printf("Aucun");
        for (j = 1; j <= nbCites; j++) {
            ELEMENT dest = recuperer(g->adjList[idArt], j);
            if (dest != NULL)
                printf("\n                                           --> [ID %02d] %s", dest->id, dest->titre);
            }
        printf("\n                                           (%d) article(s).\n\n", nbCites);


        supprimerArticle(g, idArt);


        printf("                                       Articles deconnectes    : ");
        aucun = 1;

        for (i = 0; i < nbC; i++) {
            int currentIdx = idsCitants[i];
            if (currentIdx > idArt)
                currentIdx--;


            if (currentIdx < g->V && g->articles[currentIdx] != NULL) {
                if (listeTaille(g->adjList[currentIdx]) == 0) {
                    printf("\n                                           --> [ID %02d] %s", g->articles[currentIdx]->id, g->articles[currentIdx]->titre);
                    aucun = 0;
                }
            }
        }

        if (aucun)
            printf("\n                                            [!] Aucun article deconnecte\n");
        printf("\n\n");
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
}
int neutraliserPropagation(grapheReseau g, int idSrc, int idDest) {
    int predecesseur[100];
    int visited[100];
    int i, j, compteur, courantId, meilleur, minScore;
    int s = idSrc;
    int d = idDest;
    LISTE file;
    ELEMENT courant;
    int cond;

    compteur = 0;

    if (s < 0 || s >= g->V || g->articles[s] == NULL || d < 0 || d >= g->V || g->articles[d] == NULL)
        err_exist("Article introuvable dans le graphe.");
    else {
        system("cls");
        printf("\n\n\n");
        Sleep(50);
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                    NEUTRALISATION DE PROPAGATION                                   ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n");
        printf("\n                                       De   : [ID %02d] %s\n", idSrc, g->articles[idSrc]->titre);
        printf("                                       Vers : [ID %02d] %s\n\n", idDest, g->articles[idDest]->titre);

        cond = cheminExists(g, s, d);
        while (cond) {

            for (i = 0; i < 100; i++) {
                visited[i] = 0;
                predecesseur[i] = -1;
            }

            file = listeCreer();
            visited[s] = 1;
            inserer(file, g->articles[s], 1);

            while (!estVide(file)) {
                courant = recuperer(file, 1);
                supprimer(file, 1);
                if (courant != NULL) {
                    j = 1;
                    while (j <= listeTaille(g->adjList[courant->id])) {
                        ELEMENT v = recuperer(g->adjList[courant->id], j);
                        if (v != NULL && v->id >= 0 && v->id < g->V && !visited[v->id]) {
                            visited[v->id] = 1;
                            predecesseur[v->id] = courant->id;
                            inserer(file, v, listeTaille(file) + 1);
                        }
                        j++;
                    }
                }
            }
            listeDetruire(file);

            if (!visited[d])
                cond = 0;
            else {
                meilleur  = -1;
                minScore  = 101;
                courantId = predecesseur[d];

                while (courantId != -1 && courantId != s) {
                    if (g->articles[courantId] != NULL && g->articles[courantId]->score_fiabilite < minScore) {
                        minScore = g->articles[courantId]->score_fiabilite;
                        meilleur = courantId;
                    }
                    courantId = predecesseur[courantId];
                }

                if (meilleur == -1) {
                    j = 1;
                    while (j <= listeTaille(g->adjList[s])) {
                        if (recuperer(g->adjList[s], j)->id == d) {
                            supprimer(g->adjList[s], j);
                            g->degre_in[d]--;
                        }
                        else
                            j++;
                    }
                    compteur++;
                }
                else {
                    printf("                                       Article supprime : [ID %02d] %-40s (score:%02d)\n",
                        g->articles[meilleur]->id, g->articles[meilleur]->titre, g->articles[meilleur]->score_fiabilite);
                    supprimerArticle(g, meilleur);
                    compteur++;
                    if (s > meilleur)
                        s--;
                    if (d > meilleur)
                     d--;
                }

                cond = cheminExists(g, s, d);
            }
        }


        printf("\n\n");
        printf("                                       Plus aucun chemin de [ID %02d] vers [ID %02d].\n", idSrc, idDest);
        printf("                                       Nombre d'articles supprimes : %d\n\n", compteur);
        printf("                             ========================================================================================================\n\n");
        system("pause");
    }
    return compteur;
}
