#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "foncGesJeu.h"
#include <semaphore.h>


void initPaquet(paquet *p, int capacite)

{
    p =(paquet*)malloc(sizeof(paquet));
    p->capa = capacite;
    p->tab = (carte *)malloc(capacite * sizeof(carte));

    //vérifier si l'allocation de mémoire a réussi.
    if (p->tab == NULL) {
        // Gestion de l'erreur d'allocation mémoire
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
}

void liberePaquet(paquet *p)
{
    free(p->tab);
    free(p);
}

void afficheCarte(carte c)
{
     printf("___________________________\n");

    printf("Carte n° [%d] \n", c.numero ); 
    for ( int i = 0  ; i<  c.points ; i ++ )
            printf("🐮 ") ; 
    printf("\n---------------------------\n");
}

void affichePaquet(paquet *p)
{

    for (int i = 0; i < p->nbr; i++)
    {
    
        afficheCarte(p->tab[i]);
        printf("\n\n");
    }
}

void videPaquet(paquet* p)

{

    p->nbr =0 ; 
}


carte creeCarte(int num)
{
    carte carte;
    carte.numero = num;


    if(((11*(num%10)) == num))
    {
        carte.points = 5;  
    } 
    else if(num % 5 == 0)
    {
        carte.points = 2;  // Exemple : points de 1 à 10 en fonction du dernier chiffre
    }
    else
    {
        carte.points = 1;
    }
    if(num % 10 == 0)
    {
        carte.points = 3;
    }
    else if(num == 55)
    {
        carte.points = 7;
    }

    return carte;


}


void produitJeu(paquet* p) 
{
    for(int  i =0 ; i<104  ; i++){
        p->tab[i]  = creeCarte(i+1) ; 
    }
    p->nbr = p->capa ; 
}




// traitement consernant le ROBOT ==========================

void *connexionRobot(ConnexionData *dataR)
{
    int n;
    // verification 
    if( dataR->socket< 0 )
    { 
        error("\n Connexion   [Robot]  non accepter  !  \n") ; 
    }else{ 
        printf( "\n=====>    connexion [Robot] reussi +++++\n") ;    
    }
    
}

    

// traitement consernant l'Humain   ==========================

void *connexionHumain(ConnexionData *dataH)
{
    
    

    int n;
    

    // verification 
    if( dataH->socket<0 )
    { 
        error("\n Connexion [Joueur] non accepter  !  \n") ; 
    }else{
        printf( "\n=====>    connexion [Joueur] reussi +++++\n") ;  
      
    }
}

int CalculScore(paquet *p){

    int somme=0;
    for(int j=0; j< p->nbr;j++){
        somme+=p->tab[j].points;
    }
    return somme;

}


///////init jeu///////////
paquet* initJeu(int *socket, paquet *p) {

    paquet *joueur;
    joueur = (paquet*)malloc(sizeof(paquet));
    joueur->capa = 30;
    joueur->tab = (carte *)malloc(10 * sizeof(carte));
    initPaquet(joueur, 10);
    distribuerCartes(p, joueur, 10);
    envoyerPaquetAuClient(*socket, joueur);
    return joueur;
}


void envoyerPaquetAuClient(int socket, paquet *p) {
    // Envoi de la taille du paquet en premier
    int taillePaquet = sizeof(paquet);
    int n = send(socket, &taillePaquet, sizeof(int), 0);
    if (n < 0) {
        error("Erreur lors de l'envoi de la taille du paquet");
        return;
    }

    // Envoi des informations du paquet (capacité et nombre de cartes)
    n = send(socket, p, sizeof(paquet) - sizeof(carte *), 0);
    if (n < 0) {
        error("Erreur lors de l'envoi des informations du paquet");
        return;
    }

    // Envoi des données pointées par carte *tab
    n = send(socket, p->tab, p->capa * sizeof(carte), 0);
    if (n < 0) {
        error("Erreur lors de l'envoi des données du paquet");
        return;
    }
}



void distribuerCartes(paquet *jeu, paquet *joueur, int nbCartesParJoueur) {
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
    
    for (int i = 0; i < nbCartesParJoueur; ++i) {
        int indexCarte = rand() % jeu->nbr;
        joueur->tab[i] = jeu->tab[indexCarte];
        joueur->nbr++;

        // Décaler les cartes restantes dans le paquet du jeu
        for (int j = indexCarte; j < jeu->nbr - 1; ++j) {
            jeu->tab[j] = jeu->tab[j + 1];
        }    

        jeu->nbr--;
    }
}

void creerLigne(paquet *jeu,paquet *lignes[])
{
    srand(time(NULL));
    for (int i = 0; i < 4; ++i) {
        lignes[i] = (paquet*)malloc(sizeof(paquet));
        lignes[i]->capa = 6;
        lignes[i]->tab = (carte *)malloc(6 * sizeof(carte));
        // Remplir la première carte de la ligne avec une carte aléatoire du paquet du jeu
        int indexCarte =rand()%jeu->nbr;
        lignes[i]->tab[0] = jeu->tab[indexCarte];
        lignes[i]->nbr++;
        // Décaler les cartes restantes dans le paquet du jeu
        for (int j = indexCarte; j < jeu->nbr - 1; ++j) {
            jeu->tab[j] = jeu->tab[j + 1];
        }    

        jeu->nbr--;
    }
}


void envoyerLignesAuClient(int socket, paquet *lignes[]) {
    char buffer[256];
    int delayMicroseconds = 50000; // Exemple de délai de 50 millisecondes

    for (int i = 0; i < 4; i++) {
        // Envoyer le numéro de la ligne
        sprintf(buffer, "Ligne numéro %d\n", i + 1);
        if (send(socket, buffer, strlen(buffer), 0) < 0) {
            printf("Erreur lors de l'envoi du numéro de la ligne au client.\n");
            break;
        }
        usleep(delayMicroseconds); // Délai entre l'envoi du numéro de ligne et des cartes

        // Envoyer les cartes de la ligne au client
        for (int j = 0; j < lignes[i]->nbr; j++) {
            sprintf(buffer, "Carte n° [%d] :: ", lignes[i]->tab[j].numero);
            // Ajout du nombre de têtes de vache dans la chaîne
            for (int k = 0; k < lignes[i]->tab[j].points; ++k) {
                sprintf(buffer + strlen(buffer), "🐮 "); // Ajoutez le caractère '🐮' pour chaque point
            }

            // Nouvelle ligne pour la prochaine carte
            strcat(buffer, "\n");
            if (send(socket, buffer, strlen(buffer), 0) < 0) {
                printf("Erreur lors de l'envoi de la carte au client.\n");
                break;
            }
            usleep(delayMicroseconds); // Délai entre l'envoi de chaque carte
        }
    }
    // Envoi d'un message spécial pour indiquer la fin des données
    char finMessage[] = "Fin des données.\n";
    if (send(socket, finMessage, strlen(finMessage), 0) < 0) {
        printf("Erreur lors de l'envoi du message de fin au client.\n");
    }
}

// Fonction pour envoyer un message
void envoyerMessage(int socket, const char *message) {
    send(socket, message, strlen(message), 0);
}

// Fonction pour recevoir un message
void recevoirMessage(int socket, char *buffer, int taille) {
    int n = recv(socket, buffer, taille - 1, 0);
    if (n < 0) {
        error("Erreur lors de la réception du message");
        return;
    }
    buffer[n] = '\0';  // Ajoute le caractère de fin de chaîne
}

void rangercarte(carte *tapis[],int Joueursindex[],int NB_JOUEURS) {
    
    carte *temp;
    int tempJ;
    // Tri à bulles pour trier les cartes dans l'ordre croissant
    for (int i = 0; i < NB_JOUEURS-1; i++) {
        for (int j = 0; j < NB_JOUEURS-i-1; j++) {
            if (tapis[j]->numero > tapis[j + 1]->numero) {
                // Échanger les cartes
                temp = tapis[j];
                tapis[j] = tapis[j + 1];
                tapis[j + 1] = temp;
                //echnager les indices des Joueurs
                tempJ=Joueursindex[j];
                Joueursindex[j]=Joueursindex[j+1];
                Joueursindex[j+1]=tempJ;
            }
        }
    }
}




void jouerTour(int socket[],paquet *joueur[], paquet *lignes[],int NB_JOUEURS) {
    char choix[256];
    int carteChoisie;
    carte *tapis[NB_JOUEURS];
    int Joueursindex[NB_JOUEURS];
    int score=0;
    for(int i=0;i<NB_JOUEURS;i++)
    {
        do {

            envoyerMessage(socket[i],"Choisissez La Carte : ");
            usleep(50000);
            recevoirMessage(socket[i],choix,sizeof(choix));
            carteChoisie=atoi(choix);
            carteChoisie--;
        } while (carteChoisie < 0 || carteChoisie >= joueur[i]->nbr);
        // Supprimer la carte du paquet du joueur
        carte *carteJouee = malloc(sizeof(carte));
        *carteJouee = joueur[i]->tab[carteChoisie];
        for (int j = carteChoisie; j < joueur[i]->nbr - 1; ++j) {
            joueur[i]->tab[j] = joueur[i]->tab[j + 1];
        }
        joueur[i]->nbr--;
        tapis[i]=carteJouee;
        Joueursindex[i]=i;
    }

    rangercarte(tapis,Joueursindex,NB_JOUEURS);

    for (int i = 0; i < NB_JOUEURS; i++) {
        int interval;
        int minInterval = 103; // Initialisation à une valeur maximale
        int nligne = -1; // Initialisation à une valeur non utilisée
        int indexofJoueursindex=0; 
        for (int j = 0; j < 4; j++) {
            if (lignes[j]->nbr > 0 && tapis[i]->numero > lignes[j]->tab[lignes[j]->nbr - 1].numero) {
                interval = tapis[i]->numero - lignes[j]->tab[lignes[j]->nbr - 1].numero;
                if (interval < minInterval) {
                    minInterval = interval;
                    nligne = j;
                    indexofJoueursindex=i;
                }
            }
        }

        if (nligne != -1) {
            if(lignes[nligne]->nbr<5)
            {
                // Placer la carte dans la ligne appropriée
                lignes[nligne]->tab[lignes[nligne]->nbr++] = *tapis[i];
                printf("Carte placée dans la ligne : %d\n", nligne);
            }
            else{
                // Ajouter les cartes de la ligne au paquet du joueur
                for (int k = 0; k < lignes[nligne]->nbr; k++) {
                    joueur[Joueursindex[indexofJoueursindex]]->tab[joueur[Joueursindex[indexofJoueursindex]]->nbr++] = lignes[nligne]->tab[k];
                }
                // Remplacer les cartes dans la ligne avec la sixième carte
                lignes[nligne]->nbr = 1;
                lignes[nligne]->tab[0] = *tapis[i];
                printf("Cartes de la ligne ajoutées au paquet du joueur\n");
            }
            
        } else {
            // Gérer le cas où aucune ligne ne convient pour la carte
            printf("Aucune ligne ne convient pour la carte %d\n", tapis[i]->numero);
            srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
            nligne = rand() % 4; // Génère un nombre aléatoire entre 0 et 3 (inclus)
            // Ajouter les cartes de la ligne au paquet du joueur
            for (int k = 0; k < lignes[nligne]->nbr; k++) {
                joueur[Joueursindex[indexofJoueursindex]]->tab[joueur[Joueursindex[indexofJoueursindex]]->nbr++] = lignes[nligne]->tab[k];
            }
            // Remplacer les cartes dans la ligne avec la sixième carte
            lignes[nligne]->nbr = 1;
            lignes[nligne]->tab[0] = *tapis[i];
            printf("Cartes de la ligne ajoutées au paquet du joueur\n");
        }
    }


    for (int i = 0; i < NB_JOUEURS; i++) {
        free(tapis[i]); // Libérer la mémoire allouée pour chaque carte
    }
   
    for (int i=0;i<NB_JOUEURS;i++){
        envoyerPaquetAuClient(socket[i],joueur[i]);
    }
    
}



