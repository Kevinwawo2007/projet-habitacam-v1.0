/* ============================================================
 * @file    auth.c
 * @brief   Module d'authentification de HabitatCam.
 *
 * Gere l'inscription, la connexion, la deconnexion et
 * la persistance des utilisateurs dans le fichier texte.
 *
 * @version 1.0
 * @date    2025-2026
 * @author  SOUOPGUI
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "auth.h"

/* ============================================================
 * VARIABLES GLOBALES
 * ============================================================ */

Utilisateur listeUtilisateurs[MAX_UTILISATEURS];
int         nbUtilisateurs = 0;
Session     sessionCourante = {0};


/* ============================================================
 * FONCTIONS UTILITAIRES INTERNES
 * ============================================================ */

static void viderBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void saisirMotDePasse(char *mdp, int taille) {
    int i = 0;
    char c;
    printf("Mot de passe : ");
    fflush(stdout);
    while (i < taille - 1) {
        c = getchar();
        if (c == '\n' || c == EOF) break;
        mdp[i++] = c;
        putchar('*');
        fflush(stdout);
    }
    mdp[i] = '\0';
    putchar('\n');
}

int emailExiste(const char *email) {
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(listeUtilisateurs[i].email, email) == 0)
            return 1;
    }
    return 0;
}

int rechercherUtilisateur(const char *email, const char *mdp) {
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (strcmp(listeUtilisateurs[i].email, email) == 0 &&
            strcmp(listeUtilisateurs[i].motDePasse, mdp) == 0 &&
            listeUtilisateurs[i].actif == 1) {
            return i;
        }
    }
    return -1;
}

int genererIdUtilisateur() {
    int maxId = 0;
    for (int i = 0; i < nbUtilisateurs; i++) {
        if (listeUtilisateurs[i].id > maxId)
            maxId = listeUtilisateurs[i].id;
    }
    return maxId + 1;
}


/* ============================================================
 * PERSISTANCE
 * ============================================================ */

void chargerUtilisateurs() {
    FILE *f = fopen(FICHIER_UTILISATEURS, "r");
    if (f == NULL) {
        printf("[INFO] Aucun fichier utilisateurs trouve. Creation de l'admin par defaut.\n");

        Utilisateur admin;
        admin.id = 1;
        strcpy(admin.nom,        "Admin");
        strcpy(admin.prenom,     "Super");
        strcpy(admin.email,      "admin@habitatcam.cm");
        strcpy(admin.telephone,  "000000000");
        strcpy(admin.motDePasse, "admin123");
        admin.role  = ROLE_ADMINISTRATEUR;
        admin.actif = 1;

        listeUtilisateurs[0] = admin;
        nbUtilisateurs = 1;
        sauvegarderUtilisateurs();
        return;
    }

    nbUtilisateurs = 0;
    Utilisateur u;
    while (fscanf(f, "%d|%49[^|]|%49[^|]|%79[^|]|%19[^|]|%49[^|]|%d|%d\n",
                  &u.id, u.nom, u.prenom, u.email,
                  u.telephone, u.motDePasse,
                  (int*)&u.role, &u.actif) == 8) {
        listeUtilisateurs[nbUtilisateurs++] = u;
        if (nbUtilisateurs >= MAX_UTILISATEURS) break;
    }
    fclose(f);
}

void sauvegarderUtilisateurs() {
    system("mkdir -p data");

    FILE *f = fopen(FICHIER_UTILISATEURS, "w");
    if (f == NULL) {
        printf("[ERREUR] Impossible d'ouvrir le fichier utilisateurs.\n");
        return;
    }
    for (int i = 0; i < nbUtilisateurs; i++) {
        Utilisateur *u = &listeUtilisateurs[i];
        fprintf(f, "%d|%s|%s|%s|%s|%s|%d|%d\n",
                u->id, u->nom, u->prenom, u->email,
                u->telephone, u->motDePasse,
                (int)u->role, u->actif);
    }
    fclose(f);
}


/* ============================================================
 * INSCRIPTION
 * ============================================================ */

void inscrireUtilisateur() {
    if (nbUtilisateurs >= MAX_UTILISATEURS) {
        printf("[ERREUR] Nombre maximum d'utilisateurs atteint.\n");
        return;
    }

    Utilisateur nouveau;
    char confirmation[TAILLE_MDP];
    int roleChoix;

    printf("\n[INSCRIPTION]\n");

    printf("Nom        : "); fgets(nouveau.nom,    TAILLE_NOM,   stdin); nouveau.nom[strcspn(nouveau.nom, "\n")]       = '\0';
    printf("Prenom     : "); fgets(nouveau.prenom, TAILLE_NOM,   stdin); nouveau.prenom[strcspn(nouveau.prenom, "\n")] = '\0';
    printf("Email      : "); fgets(nouveau.email,  TAILLE_EMAIL, stdin); nouveau.email[strcspn(nouveau.email, "\n")]   = '\0';

    if (emailExiste(nouveau.email)) {
        printf("[ERREUR] Cet email est deja utilise.\n");
        return;
    }

    printf("Telephone  : "); fgets(nouveau.telephone, TAILLE_TEL, stdin); nouveau.telephone[strcspn(nouveau.telephone, "\n")] = '\0';

    do {
        saisirMotDePasse(nouveau.motDePasse, TAILLE_MDP);
        printf("Confirmation : ");
        saisirMotDePasse(confirmation, TAILLE_MDP);
        if (strcmp(nouveau.motDePasse, confirmation) != 0)
            printf("[ERREUR] Les mots de passe ne correspondent pas.\n");
    } while (strcmp(nouveau.motDePasse, confirmation) != 0);

    printf("\n1. Locataire\n2. Bailleur\nChoix : ");
    scanf("%d", &roleChoix);
    viderBuffer();

    if      (roleChoix == 1) nouveau.role = ROLE_LOCATAIRE;
    else if (roleChoix == 2) nouveau.role = ROLE_BAILLEUR;
    else { printf("[ERREUR] Choix invalide.\n"); return; }

    nouveau.id    = genererIdUtilisateur();
    nouveau.actif = 1;

    listeUtilisateurs[nbUtilisateurs++] = nouveau;
    sauvegarderUtilisateurs();
    printf("[OK] Inscription reussie ! Bienvenue, %s.\n", nouveau.prenom);
}


/* ============================================================
 * CONNEXION
 * ============================================================ */

void connecterUtilisateur() {
    char email[TAILLE_EMAIL];
    char mdp[TAILLE_MDP];
    int  tentatives = 0, index;

    printf("\n[CONNEXION]\n");

    while (tentatives < 3) {
        printf("Email : "); fgets(email, TAILLE_EMAIL, stdin); email[strcspn(email, "\n")] = '\0';
        saisirMotDePasse(mdp, TAILLE_MDP);
        index = rechercherUtilisateur(email, mdp);

        if (index != -1) {
            sessionCourante.connecte    = 1;
            sessionCourante.utilisateur = listeUtilisateurs[index];
            printf("[OK] Bienvenue, %s %s !\n",
                   sessionCourante.utilisateur.prenom,
                   sessionCourante.utilisateur.nom);
            return;
        }
        printf("[ERREUR] Identifiants incorrects. (%d/3)\n", ++tentatives);
    }
    printf("[SECURITE] Trop de tentatives. Retour au menu.\n");
}


/* ============================================================
 * DECONNEXION
 * ============================================================ */

void deconnecterUtilisateur() {
    printf("\n[OK] Au revoir, %s !\n", sessionCourante.utilisateur.prenom);
    sessionCourante.connecte = 0;
    memset(&sessionCourante.utilisateur, 0, sizeof(Utilisateur));
}


/* ============================================================
 * MENU AUTHENTIFICATION
 * ============================================================ */

void menuAuthentification() {
    int choix;
    do {
        printf("\n=== HABITATCAM v1.0 ===\n");
        printf("1. Se connecter\n");
        printf("2. Creer un compte\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);
        viderBuffer();

        switch (choix) {
            case 1: connecterUtilisateur(); break;
            case 2: inscrireUtilisateur();  break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("[ERREUR] Choix invalide.\n");
        }
    } while (choix != 0 && sessionCourante.connecte == 0);
}
