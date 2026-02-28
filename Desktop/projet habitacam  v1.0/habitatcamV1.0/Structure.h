/* ============================================================
 *  HabitatCam — Plateforme de logement au Cameroun
 *  Fichier     : structure.h
 *  Version     : 1.0
 *  Description : Definition de toutes les structures de donnees
 *                utilisees dans l'application.
 * ============================================================ */

#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED

/* ── Constantes globales ───────────────────────────────────── */
#define MAX_UTILISATEURS   100
#define MAX_LOGEMENTS      200
#define MAX_RESERVATIONS   300

#define TAILLE_NOM         50
#define TAILLE_EMAIL       80
#define TAILLE_MDP         50
#define TAILLE_TEL         20
#define TAILLE_VILLE       50
#define TAILLE_QUARTIER    50
#define TAILLE_TITRE       80
#define TAILLE_DESC        200
#define TAILLE_TYPE        30

/* ── Fichiers de stockage ──────────────────────────────────── */
#define FICHIER_UTILISATEURS  "data/utilisateurs.txt"
#define FICHIER_LOGEMENTS     "data/logements.txt"
#define FICHIER_RESERVATIONS  "data/reservations.txt"

/* ── Roles utilisateur ─────────────────────────────────────── */
typedef enum {
    ROLE_LOCATAIRE      = 1,
    ROLE_BAILLEUR       = 2,
    ROLE_ADMINISTRATEUR = 3
} Role;

/* ── Statut d'un logement ──────────────────────────────────── */
typedef enum {
    STATUT_DISPONIBLE   = 1,
    STATUT_RESERVE      = 2,
    STATUT_INDISPONIBLE = 3
} StatutLogement;

/* ── Statut d'une reservation ──────────────────────────────── */
typedef enum {
    RES_EN_ATTENTE = 1,
    RES_CONFIRMEE  = 2,
    RES_ANNULEE    = 3
} StatutReservation;

/* ══════════════════════════════════════════════════════════════
 *  Structure : Utilisateur
 * ══════════════════════════════════════════════════════════════ */
typedef struct {
    int  id;
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    char email[TAILLE_EMAIL];
    char telephone[TAILLE_TEL];
    char motDePasse[TAILLE_MDP];
    Role role;
    int  actif;   /* 1 = actif, 0 = desactive par l'admin */
} Utilisateur;

/* ══════════════════════════════════════════════════════════════
 *  Structure : Logement
 * ══════════════════════════════════════════════════════════════ */
typedef struct {
    int   id;
    char  titre[TAILLE_TITRE];
    char  type[TAILLE_TYPE];
    char  description[TAILLE_DESC];
    char  ville[TAILLE_VILLE];
    char  quartier[TAILLE_QUARTIER];
    float superficie;
    int   nbPieces;
    float prixMensuel;
    StatutLogement statut;
    int   idBailleur;
} Logement;

/* ══════════════════════════════════════════════════════════════
 *  Structure : Reservation
 * ══════════════════════════════════════════════════════════════ */
typedef struct {
    int   id;
    int   idLocataire;
    int   idLogement;
    char  dateReservation[20];
    StatutReservation statut;
} Reservation;

/* ══════════════════════════════════════════════════════════════
 *  Structure : Session
 * ══════════════════════════════════════════════════════════════ */
typedef struct {
    int         connecte;
    Utilisateur utilisateur;
} Session;

#endif /* STRUCTURE_H_INCLUDED */
