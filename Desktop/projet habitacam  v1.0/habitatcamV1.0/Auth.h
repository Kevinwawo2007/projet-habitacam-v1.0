/* ============================================================
 *  HabitatCam — Plateforme de logement au Cameroun
 *  Fichier     : auth.h
 *  Version     : 1.0
 *  Description : Module d'authentification — inscriptions,
 *                connexions, gestion de session.
 * ============================================================ */

#ifndef AUTH_H_INCLUDED
#define AUTH_H_INCLUDED

#include "structure.h"

/* ── Variables globales partagees ──────────────────────────── */
extern Utilisateur  listeUtilisateurs[MAX_UTILISATEURS];
extern int          nbUtilisateurs;
extern Session      sessionCourante;

/* ── Fonctions principales ─────────────────────────────────── */
void menuAuthentification();
void inscrireUtilisateur();
void connecterUtilisateur();
void deconnecterUtilisateur();

/* ── Fonctions utilitaires ─────────────────────────────────── */
int emailExiste(const char *email);
int rechercherUtilisateur(const char *email, const char *mdp);
int genererIdUtilisateur();

/* ── Fonctions de persistance ──────────────────────────────── */
void chargerUtilisateurs();
void sauvegarderUtilisateurs();

#endif /* AUTH_H_INCLUDED */
