#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "ELTARTICLE.h"
#include "LSTPRIM.h"
#include "GRAPHPRIM.h"

char nomFichier[150] = "";




void err_int(char *msg)
 {
    system("cls");
    system("color 4F");
    printf("\n\n\n\n\n\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                          ERREUR DE SAISIE                                          ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    printf("                                       TYPE D'ERREUR : La valeur saisie est invalide.\n");
    printf("                                       SOLUTION      : %s\n\n", msg);
    printf("                             ========================================================================================================\n");
    printf("                                                   Appuyez sur une touche pour retourner au menu principal...                         \n");
    printf("                             ========================================================================================================\n\n");
    system("pause > nul");
    system("color 07");
}



void success(char *msg) {
    system("cls");
    system("color 0A");

    printf("\n\n\n\n\n\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                               SUCCES                                               ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    printf("                                    ETAT     : Operation executee sans problem.\n");
    printf("                                    DETAILS  : %s\n\n", msg);
    printf("                             ========================================================================================================\n");
    printf("                                                    Appuyez sur une touche pour continuer...                                         \n");
    printf("                             ========================================================================================================\n\n");
    system("pause > nul");
    system("color 07");
}

void quit()
{
    system("cls");
    system("color 0B");
    printf("\n\n\n\n\n\n\n\n");

    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                         FIN DU PROGRAMME                                           ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    Sleep (200);
    printf("                                       Merci d'avoir utilise l'application du reseau d'information.\n");
    printf("                                       Toutes les donnees ont ete traitees avec succes.\n\n");
    printf("                                       Statut : Fermeture en cours... [");
    for(int i = 0; i < 40; i++) {
        printf("%c", 219);
        Sleep(20);
    }
    printf("] 100%%\n\n");

    printf("                             ========================================================================================================\n");
    printf("                             ||                            SESSION TERMINEE - APPUYEZ SUR UNE TOUCHE                               ||\n");
    printf("                             ========================================================================================================\n\n");

    system("pause > null");
}


int main()
{
    char h[100];
    int choix = -1;

    grapheReseau g = NULL;
    char nomFichier[100];
    int id1, id2;
    ELEMENT art;
    int succ;
    char idsrc[100];

    do
    {
        system("cls");
        system("color 07");
        printf("\n\n\n");
        Sleep(100);
        printf("                            ========================================================================================================\n");
        printf("                            ||                                                                                                    ||\n");
        printf("                            ||                                 SYSTEME DE GESTION DU RESEAU D'INFORMATION                         ||\n");
        printf("                            ||                                                                                                    ||\n");
        printf("                            ========================================================================================================\n");
        printf("\n\n");
        printf("                            %-50s |    %-50s\n", "    01. Charger un reseau depuis un fichier", " 10. Identifier l'article le plus cite");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    02. Ajouter un nouvel article au reseau", " 11. Trier les articles par date");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    03. Ajouter une citation entre articles", " 12. Trouver le premier article citant");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    04. Supprimer un article du systeme",    " 13. Afficher la chaine de propagation");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    05. Supprimer une citation existante",   " 14. Simulation de propagation (BFS)");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    06. Afficher le reseau d'information",   " 15. Lancer l'analyse des fake news");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    07. Articles cites par un article",      " 16. Articles suspects les plus cites");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    08. Articles qui citent un article",     " 17. Simuler la suppression d'un noeud");
        printf("                             %-50s | %-50s\n", "", "");
        printf("                            %-50s |    %-50s\n", "    09. Sources originales et articles isoles", " 18. Neutraliser la propagation");
        printf("\n\n");
        printf("                            ========================================================================================================\n");
        printf("                                                                     00. QUITTER L'APPLICATION                                      \n");
        printf("                            ========================================================================================================\n\n");
        printf("\n\n");
        printf("                                 Votre choix : ");
        fgets(h, sizeof(h), stdin);
        h[strcspn(h, "\n")] = 0;
        if (verif_num(h) == 0)
        {
            err_char("Veuillez entrer des chiffres uniquement.");
        }
        else
        {
            choix = atoi(h);
            switch (choix) {
                 case 1:
                    system("cls");
                    printf("\n\n\n");
                    printf("                            ========================================================================================================\n");
                    printf("                            ||                                                                                                    ||\n");
                    printf("                            ||                                 CHARGER UN RESEAUX DEPUIS UN FICHIER                               ||\n");
                    printf("                            ||                                                                                                    ||\n");
                    printf("                            ========================================================================================================\n");
                    printf("\n                                 Nom du fichier : ");
                    fgets(nomFichier, sizeof(nomFichier), stdin);
                    nomFichier[strcspn(nomFichier, "\n")] = '\0';
                    g = chargerGraphe(nomFichier);
                    if (g != NULL){
                        char message[150];
                        sprintf(message, "Reseau charge avec succes (%d articles).",g->V);
                        success(message);
                    }
                    break;
                case 2:
                    if (g == NULL)
                        err_num("Chargez d'abord un reseau (option 1).");
                    else {
                        art = elementCreer();
                        elementLire(&art);

                        if (ajouterArticle(g, art)){
                            success("Article ajoute avec succes au reseau");
                            sauvegarderGraphe(g, nomFichier);
                        }
                        else {
                            elementDetruire(art);
                        }
                    }
                    break;
                    case 3:
                        if (g == NULL)
                            err_exist("Chargez d'abord un reseau (option 1).");
                        else {
                            do {
                                succ = 1;
                                system("cls");
                                printf("\n\n\n");
                                printf("                            ========================================================================================================\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ||                                 AJOUTER UNE CITATION ENTRE 2 ARTICLES                              ||\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ========================================================================================================\n");
                                printf("\n                                      ID article citant   : ");
                                fgets(idsrc, sizeof(idsrc), stdin);
                                idsrc[strcspn(idsrc, "\n")] = '\0';
                                if (verif_num(idsrc) == 0)
                                    err_char("Entrer des chiffres uniquement.");
                                else if (atoi(idsrc) < 0) {
                                    err_num("Entrer un ID positif.");
                                    succ = 0;
                                }
                                else if (atoi(idsrc) >= g->V || g->articles[atoi(idsrc)] == NULL) {
                                    err_exist("Pas d'article portant cet ID.");
                                    succ = 0;
                                }
                            } while (verif_num(idsrc) == 0 || succ == 0);
                            id1 = atoi(idsrc);

                            do {
                                succ = 1;
                                system("cls");
                                printf("\n\n\n");
                                printf("                            ========================================================================================================\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ||                                 AJOUTER UNE CITATION ENTRE 2 ARTICLES                              ||\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ========================================================================================================\n");
                                printf("\n                                      ID article citant   : %d", id1);
                                printf("\n                                      ID article cite     : ");
                                fgets(idsrc, sizeof(idsrc), stdin);
                                idsrc[strcspn(idsrc, "\n")] = '\0';
                                if (verif_num(idsrc) == 0)
                                    err_char("Entrer des chiffres uniquement.");
                                else if (atoi(idsrc) < 0) {
                                    err_num("Entrer un ID positif.");
                                    succ = 0;
                                }
                                else if (atoi(idsrc) >= g->V || g->articles[atoi(idsrc)] == NULL) {
                                    err_exist("Pas d'article portant cet ID.");
                                    succ = 0;
                                }
                                else if (id1 == atoi(idsrc)) {
                                    err_num("Un article ne peut pas se citer lui-meme.");
                                    succ = 0;
                                }
                                else if (elementComparer(g->articles[id1], g->articles[atoi(idsrc)]) != 1) {
                                    err_num("Le citant doit etre publiee APRES le cite.");
                                    succ = 0;
                                }
                            } while (verif_num(idsrc) == 0 || succ == 0);
                            id2 = atoi(idsrc);

                            if (ajouterCitation(g, id1, id2) == 1) {
                                sauvegarderGraphe(g, nomFichier);
                                char message[150];
                                sprintf(message, "Citation %d --> %d ajoutee avec succes.", id1, id2);
                                success(message);
                            }
                        }
                        break;
                case 4:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                            ========================================================================================================\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ||                                          SUPPRIMER UN ARTICLE                                      ||\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ========================================================================================================\n");
                            printf("\n                                 ID article a supprimer : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        if (supprimerArticle(g, id1) == 1){
                            sauvegarderGraphe(g, nomFichier);
                            success("Article supprimer avec succes.");
                        }
                    }
                    break;
                case 5:
                     if (g == NULL)
                            err_exist("Chargez d'abord un reseau (option 1).");
                        else {
                            do {
                                system("cls");
                                printf("\n\n\n");
                                printf("                            ========================================================================================================\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ||                                          SUPPRIMER UN ARTICLE                                      ||\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ========================================================================================================\n");
                                printf("\n                                 ID article citant      : ");
                                fgets(h, sizeof(h), stdin);
                                h[strcspn(h, "\n")] = '\0';
                                if (verif_num(h) == 0)
                                    err_char("Entrez un entier positif.");
                            } while (verif_num(h) == 0);
                            id1 = atoi(h);

                            do {
                                system("cls");
                                printf("\n\n\n");
                                printf("                            ========================================================================================================\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ||                                          SUPPRIMER UN ARTICLE                                      ||\n");
                                printf("                            ||                                                                                                    ||\n");
                                printf("                            ========================================================================================================\n");
                                printf("\n                                 ID article citant      : %02d \n",id1);
                                printf("                                 ID article destination : ");
                                fgets(h, sizeof(h), stdin);
                                h[strcspn(h, "\n")] = '\0';
                                if (verif_num(h) == 0)
                                    err_char("Entrez un entier positif.");
                            } while (verif_num(h) == 0);
                            id2 = atoi(h);

                        if (supprimerCitation(g, id1, id2) == 1)
                        {
                            sauvegarderGraphe(g, nomFichier);
                            success("Citation supprimee avec succes.");
                        }

                    }
                    break;
                case 6:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        afficherGraphe(g);
                        system("pause");
                    }
                    break;
                case 7:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                            ========================================================================================================\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ||                                     ARTICLES CITES PAR UN ARTICLE                                  ||\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ========================================================================================================\n");
                            printf("\n                                 ID article citant: ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        articlesCites(g, id1);
                    }
                    break;
                case 8:
                        if (g == NULL)
                            err_exist("Chargez d'abord un reseau (option 1).");
                        else {
                            system("cls");
                            printf("\n\n\n");
                            printf("                            ========================================================================================================\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ||                                       ARTICLES CITANT UN ARTICLE                                   ||\n");
                            printf("                            ||                                                                                                    ||\n");
                            printf("                            ========================================================================================================\n");
                            do {
                            printf("\n                                 ID article cite : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        articlesCitants(g, id1);
                    }
                    break;
                case 9:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        sourcesOriginales(g);
                        articlesIsoles(g);
                        system("pause");
                    }
                    break;
                case 10:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        articlePlusCite(g);
                    }
                    break;
                case 11:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        trierParDate(g);
                        system("pause");
                       }
                    break;
                case 12:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                      PREMIER ARTICLE CITANT                                        ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n\n");
                            printf("\n                                 ID article cible : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        premierCitant(g, id1);
                    }
                    break;
                case 13:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                       CHAINE DE PROPAGATION                                        ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n\n");
                            printf("\n                                 ID article source : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        chainePropagation(g, id1);
                    }
                    break;
                case 14:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                         SIMULER PROPAGATION                                        ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n\n");
                            printf("\n                                 ID article source : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        simulerPropagation(g, id1);
                    }
                    break;
                case 15:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else
                        analyserReseau(g);
                        sauvegarderGraphe(g, nomFichier);
                    break;
                case 16:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else
                        articlesSuspectsCites(g);
                    break;
                case 17:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                   SIMULATION DE SUPPRESSION                                        ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n");
                            printf("\n                                 ID article a supprimer : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);
                        simulerSuppression(g, id1);
                        sauvegarderGraphe(g, nomFichier);
                    }
                    break;


                case 18:
                    if (g == NULL)
                        err_exist("Chargez d'abord un reseau (option 1).");
                    else {
                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                    NEUTRALISATION DE PROPAGATION                                   ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n");
                            printf("\n                                 ID article source      : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id1 = atoi(h);

                        do {
                            system("cls");
                            printf("\n\n\n");
                            printf("                             ========================================================================================================\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ||                                    NEUTRALISATION DE PROPAGATION                                   ||\n");
                            printf("                             ||                                                                                                    ||\n");
                            printf("                             ========================================================================================================\n");
                            printf("\n                                 ID article source      : %d\n",id1);
                            printf("                                 ID article destination : ");
                            fgets(h, sizeof(h), stdin);
                            h[strcspn(h, "\n")] = '\0';
                            if (verif_num(h) == 0)
                                err_char("Entrez un entier positif.");
                        } while (verif_num(h) == 0);
                        id2 = atoi(h);
                        neutraliserPropagation(g, id1, id2);
                        sauvegarderGraphe(g, nomFichier);
                    }
                    break;

                case 0:
                    quit();
                    break;
                default:
                    err_int("Choisissez une valeur entre 0 et 18.");
                    break;
                }
            }
    } while (choix != 0);

    return 0;
}
