#ifndef FONCGESJEU_H
#define FONCGESJEU_H




//================STRUCTURE DES CARTES ==================
//#define NB_JOUEURS 5
//#define NB_JOUEURSH 2
//#define NB_JOUEURSR 3
#define NB_MANCHES 4

typedef struct
{
    int points;
    int numero;
} carte;


typedef struct
{
    int capa;
    int nbr; // nombre de  carte
    carte *tab;
} paquet; 


// Structure pour encapsuler les données
typedef struct {
    int socket;
    paquet *jeu;
} ConnexionData;




// ============+END+=================//


// Déclaration des fonctions
void initPaquet(paquet *p, int capacite);
void liberePaquet(paquet *p);
void videPaquet(paquet* p) ;
void afficheCarte(carte c);
void affichePaquet(paquet *p);
carte creeCarte(int num) ;
void produitJeu(paquet* p) ;
void *connexionRobot(ConnexionData *dataR) ; 
void *connexionHumain(ConnexionData *dataH ) ;
void error(char *msg) ; 
//fonction pour l'envoi et la réception de messages
void envoyerMessage(int socket, const char *message);
void recevoirMessage(int socket, char *buffer, int taille);

void envoyerPaquetAuClient(int socket, paquet *p);
void distribuerCartes(paquet *jeu, paquet *joueur, int nbCartesParJoueur);
paquet* initJeu(int *socket, paquet *p);

void creerLigne(paquet *jeu,paquet *lignes[]);
void envoyerLignesAuClient(int socket, paquet *lignes[]);

void jouerTour(int socket[],paquet *joueur[], paquet *lignes[],int NB_JOUEURS);
int CalculScore(paquet *p);

#endif
