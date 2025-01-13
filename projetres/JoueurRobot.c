#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include "foncGesJeu.h"
#include <semaphore.h>
#include <time.h>


void error(char *msg)
{
    perror(msg);
    exit(1);
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
void liberePaquet(paquet *p)
{
    free(p->tab);
    free(p);
}



void recevoirPaquetDuServeur(int socket,paquet **p) {
    int taillePaquet;
    int n = recv(socket, &taillePaquet, sizeof(int), 0);
    if (n < 0) {
        error("Erreur lors de la réception de la taille du paquet");
    }
    *p = (paquet*)malloc(taillePaquet);

    if (*p == NULL) {
        error("Erreur d'allocation de mémoire pour le paquet");
    }

    // Réception des informations du paquet (sauf le pointeur carte *tab)
    n = recv(socket, *p, sizeof(paquet) - sizeof(carte*), 0);
    if (n < 0) {
        error("Erreur lors de la réception des informations du paquet");
        free(*p);
        *p=NULL;
    }

    // Allocation de mémoire pour le pointeur carte *tab
    (*p)->tab = (carte*)malloc((*p)->capa * sizeof(carte));
    if ((*p)->tab == NULL) {
        error("Erreur d'allocation de mémoire pour les données du paquet");
        free(*p);
        *p=NULL;

    }

    // Réception des données pointées par carte *tab
    n = recv(socket, (*p)->tab, (*p)->capa * sizeof(carte), 0);
    if (n < 0) {
        error("Erreur lors de la réception des données du paquet");
        free((*p)->tab);
        free(*p);
        *p=NULL;
    }
}

void recevoirLignesDuServeur(int socket) {
    char buffer[256];
    int bytesReceived;

    while ((bytesReceived = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesReceived] = '\0';  // Ajouter le caractère de fin de chaîne
        printf("%s", buffer);  // Afficher les données reçues
        // Vérifier si le message de fin est reçu
        if (strcmp(buffer, "Fin des données.\n") == 0) {
            break; // Sortir de la boucle
        }
    }

    if (bytesReceived < 0) {
        // Gérer les erreurs de réception
        printf("Erreur lors de la réception des données.\n");
    }
}



// Fonction pour déterminer la meilleure carte à jouer
int determinerMeilleureCarte(paquet *p, paquet *ranger[]) {
    int meilleureCarte = -1; // Indice de la meilleure carte
    int intervalleMin = 103; // Initialisation de l'intervalle à une valeur maximale
    int plusPetiteCarte = 104; // Initialisation de la plus petite carte
    
    // Trouver la plus petite carte parmi les dernières cartes des 4 rangées
    for (int i = 0; i < 4; i++) {
        if (ranger[i]->nbr > 0) {
            int derniereCarte = ranger[i]->tab[ranger[i]->nbr - 1].numero;
            if (derniereCarte < plusPetiteCarte) {
                plusPetiteCarte = derniereCarte;
            }
        }
    }
    
    // Parcourir les cartes dans la main du joueur
    for (int i = 0; i < p->nbr; i++) {
        int carteActuelle = p->tab[i].numero;
        int teteVachesActuelle = p->tab[i].points;
        
        // Vérifier si la carte est plus grande que la plus petite carte
        if (carteActuelle > plusPetiteCarte) {
            // Calculer l'intervalle avec la plus petite carte
            int intervalle = carteActuelle - plusPetiteCarte;
            
            // Mettre à jour si l'intervalle est le plus petit
            if (intervalle < intervalleMin) {
                intervalleMin = intervalle;
                meilleureCarte = i; // Mettre à jour l'indice de la meilleure carte
            }
        }
        else{
            srand(time(NULL)); 
            carteActuelle = rand() % p->nbr;
        }
    }
    
    return (meilleureCarte+1);
}

int  main( int argc , char *argv[])
{

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    } 

    server = gethostbyname(argv[1]);
    // port 
    portno = atoi(argv[2]); 
    // creation d'un socket TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
    error("ERREUR d'ouverture du  socket\n");
    


    if (server == NULL) {
        fprintf(stderr,"ERREUR aucun hote trouver \n");
        exit(0);
    }
    // initialisation de la zone memoire pour stocke l'addresse du serveur 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // famille d'addresse en IPV4 
    serv_addr.sin_family = AF_INET;
    // on copie de nom de l'hote dans la zone memoire de s_addr
    memcpy((char *)&serv_addr.sin_addr.s_addr,
       (char *)server->h_addr,
       server->h_length);
       serv_addr.sin_port = htons(portno);
    // on tente la connexion entre le client et le serveur 
    if (connect(sockfd,(const struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    { 
        error("ERREUR  de connexion \n");
        // Fermeture du socket client
        close(sockfd);
    }else
    {
        printf("===>  connexion reussi \n") ; 
    }

    paquet *p=NULL;
    paquet *lignes[4];
    recevoirPaquetDuServeur(sockfd,&p);
    if (p != NULL) {
        // Utilisez les données du paquet reçu
        affichePaquet(p);
    } else {
        // Gérer les erreurs de réception du paquet
        printf("error lors de la reception du paquet");
    }
    int nbmanche=0;
    char message[256]; // Taille arbitraire pour le message

    

    
    printf("RECEPTION DES LIGNES ... \n");
    recevoirLignesDuServeur(sockfd);

    while(nbmanche!=NB_MANCHES){
        for (int j = 0; j < 4; ++j) {
            recevoirPaquetDuServeur(sockfd,&lignes[j]);
        }
        //recevoir le numero du Joueur
        recevoirMessage(sockfd,message,sizeof(message));
        printf("%s",message);

        // Réception du message du serveur
        recevoirMessage(sockfd,message,sizeof(message));

        char choix[256];
        int choixEntier;
        ////////// Joueur pas intelligents risque de bugue ///////////
        /*if (p->nbr != 0) {
            srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
            choixEntier = rand() % p->nbr;
            sprintf(choix, "%d", choixEntier);
            envoyerMessage(sockfd, choix);
        } else {
            // Gérer le cas où p->nbr est égal à zéro
            printf("Erreur : p->nbr est égal à zéro\n");
            // Traitez cette situation en envoyant un message ou en prenant une autre action appropriée
        }*/


        // Joueur intelligent
        choixEntier=determinerMeilleureCarte(p,lignes);
        sprintf(choix, "%d", choixEntier);
        envoyerMessage(sockfd, choix);

        liberePaquet(p);    
        recevoirPaquetDuServeur(sockfd,&p);
        if (p != NULL) {
            printf("Votre Main : \n");
            // Utilisez les données du paquet reçu
            affichePaquet(p);
            // N'oubliez pas de libérer la mémoire allouée pour le paquet
            liberePaquet(p);    
        } else {
            // Gérer les erreurs de réception du paquet
            printf("error lors de la reception du paquet");
        }    
        ////score
        recevoirMessage(sockfd,message,sizeof(message));
        recevoirLignesDuServeur(sockfd);
        nbmanche++;
    }

        
    recevoirMessage(sockfd,message,sizeof(message));
    printf("%s",message);
    recevoirMessage(sockfd,message,sizeof(message));
    printf("%s",message);

    close(sockfd) ; 

    return EXIT_SUCCESS ; 
}