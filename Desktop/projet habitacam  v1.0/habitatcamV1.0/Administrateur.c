/* ============================================================
 * @file    administrateur.c
 * @brief   Module Administrateur de HabitatCam.
 *
 * Fournit les outils de supervision et de gestion complete
 * de la plateforme : utilisateurs, logements et statistiques.
 * Accessible uniquement aux comptes de role ROLE_ADMINISTRATEUR.
 *
 * @version 1.0
 * @date    2025-2026
 * @author  SOUOPGUI
 * @author  TOGNANG
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "auth.h"
#include "logement.h"
#include "administrateur.h"

/* ============================================================
 * FONCTIONS UTILITAIRES INTERNES
 * ============================================================ */

static void viderBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void separateur() {
    printf("---------------------------------------------\n");
}

static const char* roleEnTexte(Role r) {
    switch (r) {
        case ROLE_LOCATAIRE:      return "Locataire";
        case ROLE_BAILLEUR:       return "Bailleur";
        case ROLE_ADMINISTRATEUR: return "Administrateur";
        default:                  return "Inconnu";
    }
}

static const char* statutLogementEnTexte(StatutLogement s) {
    switch (s) {
        case STATUT_DISPONIBLE:   return "Disponible";
        case STATUT_RESERVE:      return "Reserve";
        case STATUT_INDISPONIBLE: return "Indisponible";
        default:                  return "Inconnu";
    }
}

static int verifierAdmin() {
    if (!sessionCourante.connecte ||
        sessionCourante.utilisateur.role != ROLE_ADMINISTRATEUR) {
        printf("[ACCES REFUSE] Section reservee a l'administrateur.\n");
        return 0;
    }
    return 1;
}


/* ============================================================
 * GESTION DES UTILISATEURS
 * ============================================================ */

void adminVoirUtilisateurs() {
    if (!verifierAdmin()) return;

    printf("\n[LISTE DES UTILISATEURS]\n");
    if (nbUtilisateurs == 0) { printf("Aucun utilisateur enregistre.\n"); return; }

    separateur();
    printf("%-4s %-15s %-15s %-25s %-14s %-6s\n",
           "ID", "Nom", "Prenom", "Email", "Role", "Statut");
    separateur();
    for (int i = 0; i < nbUtilisateurs; i++) {
        Utilisateur *u = &listeUtilisateurs[i];
        printf("%-4d %-15s %-15s %-25s %-14s %s\n",
               u->id, u->nom, u->prenom, u->email,
               roleEnTexte(u->role), u->actif ? "Actif" : "DESACTIVE");
    }
    separateur();
    printf("Total : %d utilisateur(s)\n", nbUtilisateurs);
}

void adminToggleCompte() {
    if (!verifierAdmin()) return;

    adminVoirUtilisateurs();
    int id;
    printf("\nID de l'utilisateur a activer/desactiver : ");
    scanf("%d", &id); viderBuffer();

    if (id == sessionCourante.utilisateur.id) {
        printf("[ERREUR] Impossible de desactiver votre propre compte.\n"); return;
    }
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (listeUtilisateurs[i].id == id) {
            if (listeUtilisateurs[i].role == ROLE_ADMINISTRATEUR) {
                printf("[ERREUR] Impossible de modifier un compte administrateur.\n"); return;
            }
            listeUtilisateurs[i].actif = !listeUtilisateurs[i].actif;
            sauvegarderUtilisateurs();
            printf("[OK] Compte de %s : %s\n", listeUtilisateurs[i].prenom,
                   listeUtilisateurs[i].actif ? "ACTIF" : "DESACTIVE");
            return;
        }
    }
    printf("[ERREUR] Aucun utilisateur avec l'ID %d.\n", id);
}

void adminSupprimerUtilisateur() {
    if (!verifierAdmin()) return;

    adminVoirUtilisateurs();
    int id;
    printf("\nID de l'utilisateur a supprimer : ");
    scanf("%d", &id); viderBuffer();

    if (id == sessionCourante.utilisateur.id) {
        printf("[ERREUR] Impossible de supprimer votre propre compte.\n"); return;
    }
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (listeUtilisateurs[i].id == id) {
            if (listeUtilisateurs[i].role == ROLE_ADMINISTRATEUR) {
                printf("[ERREUR] Impossible de supprimer un administrateur.\n"); return;
            }
            char nom[TAILLE_NOM];
            strcpy(nom, listeUtilisateurs[i].prenom);
            for (int j = i; j < nbUtilisateurs - 1; j++)
                listeUtilisateurs[j] = listeUtilisateurs[j + 1];
            nbUtilisateurs--;
            sauvegarderUtilisateurs();
            printf("[OK] Compte de %s (ID: %d) supprime.\n", nom, id);
            return;
        }
    }
    printf("[ERREUR] Aucun utilisateur avec l'ID %d.\n", id);
}


/* ============================================================
 * GESTION DES LOGEMENTS
 * ============================================================ */

void adminVoirLogements() {
    if (!verifierAdmin()) return;

    printf("\n[LISTE DES LOGEMENTS]\n");
    if (nbLogements == 0) { printf("Aucun logement enregistre.\n"); return; }

    separateur();
    printf("%-4s %-20s %-12s %-15s %-10s %-12s %-12s\n",
           "ID", "Titre", "Type", "Ville", "Surf(m2)", "Prix(FCFA)", "Statut");
    separateur();
    for (int i = 0; i < nbLogements; i++) {
        Logement *l = &listeLogements[i];
        printf("%-4d %-20s %-12s %-15s %-10.1f %-12.0f %s\n",
               l->id, l->titre, l->type, l->ville,
               l->superficie, l->prixMensuel,
               statutLogementEnTexte(l->statut));
    }
    separateur();
    printf("Total : %d logement(s)\n", nbLogements);
}

void adminSupprimerLogement() {
    if (!verifierAdmin()) return;

    adminVoirLogements();
    int id;
    printf("\nID du logement a supprimer : ");
    scanf("%d", &id); viderBuffer();

    for (int i = 0; i < nbLogements; i++) {
        if (listeLogements[i].id == id) {
            char titre[TAILLE_TITRE];
            strcpy(titre, listeLogements[i].titre);
            for (int j = i; j < nbLogements - 1; j++)
                listeLogements[j] = listeLogements[j + 1];
            nbLogements--;
            sauvegarderLogements();
            printf("[OK] Logement \"%s\" (ID: %d) supprime.\n", titre, id);
            return;
        }
    }
    printf("[ERREUR] Aucun logement avec l'ID %d.\n", id);
}


/* ============================================================
 * STATISTIQUES
 * ============================================================ */

void adminStatistiques() {
    if (!verifierAdmin()) return;

    int nbLoc=0, nbBaill=0, nbAdm=0, nbActifs=0, nbInactifs=0;
    int nbDispo=0, nbRes=0, nbIndispo=0;

    for (int i = 0; i < nbUtilisateurs; i++) {
        switch (listeUtilisateurs[i].role) {
            case ROLE_LOCATAIRE:      nbLoc++;   break;
            case ROLE_BAILLEUR:       nbBaill++; break;
            case ROLE_ADMINISTRATEUR: nbAdm++;   break;
        }
        listeUtilisateurs[i].actif ? nbActifs++ : nbInactifs++;
    }
    for (int i = 0; i < nbLogements; i++) {
        switch (listeLogements[i].statut) {
            case STATUT_DISPONIBLE:   nbDispo++;   break;
            case STATUT_RESERVE:      nbRes++;     break;
            case STATUT_INDISPONIBLE: nbIndispo++; break;
        }
    }

    printf("\n[TABLEAU DE BORD]\n");
    printf("\nUTILISATEURS (%d)\n", nbUtilisateurs);
    separateur();
    printf("  Locataires      : %d\n", nbLoc);
    printf("  Bailleurs       : %d\n", nbBaill);
    printf("  Administrateurs : %d\n", nbAdm);
    printf("  Actifs          : %d  |  Desactives : %d\n", nbActifs, nbInactifs);
    printf("\nLOGEMENTS (%d)\n", nbLogements);
    separateur();
    printf("  Disponibles     : %d\n", nbDispo);
    printf("  Reserves        : %d\n", nbRes);
    printf("  Indisponibles   : %d\n", nbIndispo);
    printf("\nRESERVATIONS\n");
    separateur();
    printf("  Total           : %d\n", nbReservations);
    separateur();
}


/* ============================================================
 * MENU PRINCIPAL ADMINISTRATEUR
 * ============================================================ */

void menuAdministrateur() {
    if (!verifierAdmin()) return;
    int choix;

    do {
        printf("\n=== PANNEAU ADMINISTRATEUR ===\n");
        printf("Connecte : %s\n", sessionCourante.utilisateur.prenom);
        separateur();
        printf("1. Voir tous les utilisateurs\n");
        printf("2. Activer / Desactiver un compte\n");
        printf("3. Supprimer un utilisateur\n");
        printf("4. Voir tous les logements\n");
        printf("5. Supprimer un logement\n");
        printf("6. Statistiques generales\n");
        printf("0. Se deconnecter\n");
        printf("Choix : ");
        scanf("%d", &choix); viderBuffer();

        switch (choix) {
            case 1: adminVoirUtilisateurs();    break;
            case 2: adminToggleCompte();         break;
            case 3: adminSupprimerUtilisateur(); break;
            case 4: adminVoirLogements();        break;
            case 5: adminSupprimerLogement();    break;
            case 6: adminStatistiques();         break;
            case 0: deconnecterUtilisateur();    break;
            default: printf("[ERREUR] Choix invalide.\n");
        }
    } while (choix != 0 && sessionCourante.connecte);
}
