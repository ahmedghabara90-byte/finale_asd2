#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "ELTARTICLE.h"

#include "GRAPHPRIM.h"

int verif_no_espace(char *s) {
    int i = 0;
    int valid = 1;
    if (s[0] == '\0') valid = 0;
    while (s[i] != '\0' && valid) {
        if (s[i] == ' ')
            valid = 0;
        i++;
    }
    return valid;
}

int verif_num(char ch[])
{
    int i = 0;
    int valid = 1;
    int len = strlen(ch);
    if (len == 0)
        valid = 0;
    else
    {
        while (ch[i] != '\0' && valid == 1)
        {
            if (ch[i] < '0' || ch[i] > '9')
                valid = 0;
            i++;
        }
    }
    return valid;
}


void err_char(char *msg) {
    system("cls");
    system("color 4F");
    printf("\n\n\n\n\n\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                          ERREUR DE SAISIE                                          ||\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ========================================================================================================\n\n");
    printf("                                       TYPE D'ERREUR : Entree invalide.\n");
    printf("                                       SOLUTION      : %s\n\n", msg);
    printf("                             ========================================================================================================\n");
    printf("                                                  Appuyez sur une touche pour retourner au menu principal...                         \n");
    printf("                             ========================================================================================================\n\n");
    system("pause > nul");
    system("color 07");
}

void err_num(char *msg)
 {
    system("cls");
    system("color 4F");
    printf("\n\n\n\n\n\n\n\n");
    printf("                             ========================================================================================================\n");
    printf("                             ||                                                                                                    ||\n");
    printf("                             ||                                          ERREUR DE VALEUR                                          ||\n");
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

ELEMENT elementCreer(void) {
    ELEMENT e = (ELEMENT) malloc(sizeof(articleStruct));
    if (!e)
        printf("\nPlus d'espace memoire");
    return e;
}

void elementDetruire(ELEMENT e) {
    free(e);
}

void elementAffecter(ELEMENT *e1, ELEMENT e2) {
    *e1 = e2;
}

void elementCopier(ELEMENT *e1, ELEMENT e2) {
    (*e1)->id = e2->id;
    strcpy((*e1)->titre,  e2->titre);
    strcpy((*e1)->source, e2->source);
    (*e1)->score_fiabilite = e2->score_fiabilite;
    (*e1)->jour   = e2->jour;
    (*e1)->mois   = e2->mois;
    (*e1)->annee  = e2->annee;
    (*e1)->heure  = e2->heure;
    (*e1)->minute = e2->minute;
}


int elementComparer(ELEMENT e1, ELEMENT e2) {
    int res = 0;

    if (e1->annee != e2->annee) {
        if (e1->annee < e2->annee)
            res = -1;
        else
            res = 1;
    }
    else if (e1->mois != e2->mois) {
        if (e1->mois < e2->mois) {
            res = -1;
        } else {
            res = 1;
        }
    }
    else if (e1->jour != e2->jour) {
        if (e1->jour < e2->jour)
            res = -1;
        else
            res = 1;
    }
    else if (e1->heure != e2->heure) {
        if (e1->heure < e2->heure)
            res = -1;
        else
            res = 1;
    }
    else if (e1->minute != e2->minute) {
        if (e1->minute < e2->minute)
            res = -1;
        else
            res = 1;
    }

    return res;
}



void elementAfficher(ELEMENT e) {
    if (e == ELEMENT_VIDE)
        printf("(vide)");
    else
        printf("---> %s (id:%d, source:%s, score:%d, %02d/%02d/%d %02dh%02d)",
        e->titre, e->id, e->source, e->score_fiabilite,
        e->jour, e->mois, e->annee, e->heure, e->minute);
}

void elementLire(ELEMENT *e) {
    char buffer[100];
    int etape = 1;
    int temp;

    while (etape <= 7) {
        system("cls");
        printf("\n\n\n");
        printf("                             ========================================================================================================\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ||                                         AJOUTER UN ARTICLE                                         ||\n");
        printf("                             ||                                                                                                    ||\n");
        printf("                             ========================================================================================================\n\n");
        (*e)->id = prochainId;
        if (etape > 0)
            printf("                                ID attribue : %d\n", (*e)->id);
        if (etape > 1)
            printf("                                Titre       : %s\n", (*e)->titre);
        if (etape > 2)
            printf("                                Source      : %s\n", (*e)->source);
        if (etape > 3)
            printf("                                Score       : %02d /100\n", (*e)->score_fiabilite);
        if (etape > 4)
            printf("                                Date        : %02d/", (*e)->jour);
        if (etape > 5)
            printf("%02d/", (*e)->mois);
        if (etape > 6)
            printf("%d\n", (*e)->annee);
        if (etape > 7)
            printf("                                Heure       : %02dh%02d\n", (*e)->heure, (*e)->minute);

        printf("\n                                  >>> ");

        switch (etape) {
            case 1:
                do {
                    system("cls");
                    printf("\n\n\n");
                    printf("                             ========================================================================================================\n");
                    printf("                             ||                                                                                                    ||\n");
                    printf("                             ||                                         AJOUTER UN ARTICLE                                         ||\n");
                    printf("                             ||                                                                                                    ||\n");
                    printf("                             ========================================================================================================\n\n");
                    printf("                                ID          : %d\n", (*e)->id);
                    printf("\n                                  >>> ");
                    printf("Titre  : ");
                    fgets((*e)->titre, sizeof((*e)->titre), stdin);
                    (*e)->titre[strcspn((*e)->titre, "\n")] = '\0';
                    if (verif_no_espace((*e)->titre) == 0)
                        err_char("Utilisez '_' a la place des espaces.");
                } while (verif_no_espace((*e)->titre) == 0);
                etape++;
                break;
            case 2:
                printf("Source : ");
                fgets((*e)->source, 50, stdin);
                (*e)->source[strcspn((*e)->source, "\n")] = 0;
                etape++;
                break;
            case 3:
                printf("Score (0-100) : ");
                fgets(buffer, 100, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (verif_num(buffer) == 0) {
                    err_char("Le score doit etre numerique.");
                } else {
                    temp = atoi(buffer);
                    if (temp < 0 || temp > 100)
                        err_num("Score doit etre entre 0 et 100.");
                    else {
                        (*e)->score_fiabilite = temp;
                        etape++;
                    }
                }
                break;
            case 4:
                printf("Jour (1-31) : ");
                fgets(buffer, 100, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (verif_num(buffer) == 0)
                    err_char("Jour doit etre numerique.");
                else {
                    temp = atoi(buffer);
                    if (temp < 1 || temp > 31)
                        err_num("Jour doit etre entre 1 et 31.");
                    else {
                        (*e)->jour = temp;
                        etape++;
                    }
                }
                break;
            case 5:
                printf("Mois (1-12) : ");
                fgets(buffer, 100, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (verif_num(buffer) == 0)
                    err_char("Mois doit etre numerique.");
                else {
                    temp = atoi(buffer);
                    if (temp< 1 || temp > 12)
                        err_num("Mois doit etre entre 1 et 12.");
                    else {
                        (*e)->mois = temp;
                        etape++;
                        }
                    }
                break;
            case 6:
                printf("Annee : ");
                fgets(buffer, 100, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (verif_num(buffer) == 0)
                    err_char("Annee doit etre numerique.");
                else {
                    temp = atoi(buffer);
                    if (temp < 1)
                        err_num("Annee doit etre >1 .");
                    else {
                        (*e)->annee = temp;
                        etape++; }
                    }
                break;
            case 7:
                printf(" Heure (0-23) : ");
                fgets(buffer, 100, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                if (verif_num(buffer) == 0)
                    err_char("Heure doit etre numerique.");
                else {
                    int h = atoi(buffer);
                    if (h < 0 || h > 23)
                        err_num("Heure doit etre entre 0 et 23.");
                    else {
                        (*e)->heure = h;
                        printf("                                      Minute (0-59) : ");
                        fgets(buffer, 100, stdin);
                        buffer[strcspn(buffer, "\n")] = 0;
                        if (verif_num(buffer) == 0)
                            err_char("Minute doit etre numerique.");
                        else {
                            int m = atoi(buffer);
                            if (m < 0 || m > 59)
                                err_num("Minute doit etre entre 0 et 59.");
                            else {
                                (*e)->minute = m;
                                etape++;
                            }
                        }
                    }
                }
                break;
        }
    }
}
