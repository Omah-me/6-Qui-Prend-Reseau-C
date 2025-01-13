
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
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

void error(char *msg)
{
    perror(msg);
    exit(1);
}




// =====================DEBUT===================
int main(int argc, char *argv[])
{

    int NB_JOUEURSH;
    int NB_JOUEURSR;
    int NB_JOUEURS;
    printf("CHOISISSEZ LE NOMBRE DE JOUEURS HUMAINS  :  ");
    scanf("%d",&NB_JOUEURSH);
    printf("CHOISISSEZ LE NOMBRE DE JOUEURS ROBOTS  :   ");
    scanf("%d",&NB_JOUEURSR);
    NB_JOUEURS=NB_JOUEURSH+NB_JOUEURSR;

    FILE *fichier;
    fichier = fopen("donnees.txt", "w+"); // Ouvre le fichier en mode écriture

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }

    // Écriture des données dans le fichier
    fprintf(fichier, "%d>%d>%d>%d>",NB_JOUEURS,NB_JOUEURSH,NB_JOUEURSR,NB_MANCHES); 

    
    
  
    int sockfdR, sockfdH, portnoR, portnoH, tailleAddrCliH,tailleAddrCliR, newsocketR, newsocketH;
    int n;

    struct sockaddr_in serv_addrR, serv_addrH, cli_addrH,cli_addrR;

    // creation du socket serveur Robot
    // socket TCP
    sockfdR = socket(AF_INET, SOCK_STREAM, 0);

    // creation du socket serveur Humain
    // socket TCP
    sockfdH = socket(AF_INET, SOCK_STREAM, 0);

    // verification de la creation du  socket

    if (sockfdR < 0)
        error("Ouverture de la connnexion echoué ( robot  ) :  ");

    // verification de la creation du  socket

    if (sockfdR < 0)
        error("Ouverture de la connnexion echoué ( joueur ) :  ");

    // initialisation de la zone memoire de l'addresse du serveur ( Gestion jeu  ) a zeros
    bzero((char *)&serv_addrR, sizeof(serv_addrR));
    bzero((char *)&serv_addrH, sizeof(serv_addrH));
    // on converti l'argument 1 en entier ( port )
    portnoR = 8080;
    serv_addrR.sin_family = AF_INET;
    serv_addrR.sin_addr.s_addr = INADDR_ANY;
    serv_addrR.sin_port = htons(portnoR);

    //  =========connexion Humain===============
    portnoH = 8084;
    serv_addrH.sin_family = AF_INET;
    serv_addrH.sin_addr.s_addr = INADDR_ANY;
    serv_addrH.sin_port = htons(portnoH);


    printf("+++++++++++++++++++ En attente de connexion ++++++++++++++++++++\n");
    // etablissement de la connexion  avec le Robot
    if (bind(sockfdR, (struct sockaddr *)&serv_addrR, sizeof(serv_addrR)) < 0)
    {
        error("\nERREUR  de connexion  avec le Robot");
    }
    // eta
    if (bind(sockfdH, (struct sockaddr *)&serv_addrH, sizeof(serv_addrH)) < 0)
    {
        error("\nERREUR  de connexion avec le Joueur");
    }


    // Initialisez le paquet de jeu une seule fois
    paquet* jeu = (paquet*)malloc(sizeof(paquet));
    jeu->capa = 104;
    jeu->tab = (carte*)malloc(104 * sizeof(carte));
    produitJeu(jeu);

    paquet* Lignes[4]; 

    ConnexionData *dataR = (ConnexionData *)malloc(sizeof(ConnexionData));
    ConnexionData *dataH = (ConnexionData *)malloc(sizeof(ConnexionData));
    dataH->jeu = jeu;
    dataR->jeu = jeu;

    creerLigne(jeu,Lignes);
    for (int i=0;i<4;i++){
        printf("la range numero %d \n",i+1);
        affichePaquet(Lignes[i]);
    }

    // serveur a l'ecoute de 10 connexion Robots
    listen(sockfdR, NB_JOUEURSR);

    // serveur a l'ecoute de  max 10 connexions Humains
    listen(sockfdH, NB_JOUEURSH);

    int socketsR[NB_JOUEURSR];
    paquet *paquetsJR[NB_JOUEURSR];
    for (int i = 0; i < NB_JOUEURSR; ++i) {

        tailleAddrCliR = sizeof(cli_addrR);
        // acceptation  de la connexion de la part du (des) Robot(s)
        newsocketR = accept(sockfdR, (struct sockaddr *)&cli_addrR, (socklen_t *)&tailleAddrCliR);
        dataR->socket = newsocketR;
        if (newsocketR < 0) {
            error("Erreur lors de l'acceptation de la connexion du joueur");
        }

        pthread_t thread_R;
        if (pthread_create(&thread_R, NULL, (void *)connexionRobot, (void *)dataR) < 0){
            printf(" Erreur de la creation du thead Robot ");
            close(newsocketR); // Fermeture du socket du joueur
        }
        socketsR[i]=newsocketR;
        pthread_join(thread_R, NULL);
        paquetsJR[i] = initJeu(&dataR->socket, dataR->jeu);
    }

    int socketsH[NB_JOUEURSH];
    paquet *paquetsJH[NB_JOUEURSH];
    for (int i = 0; i < NB_JOUEURSH; ++i) {
        tailleAddrCliH = sizeof(cli_addrH);
        newsocketH = accept(sockfdH, (struct sockaddr *)&cli_addrH, (socklen_t *)&tailleAddrCliH);
        dataH->socket = newsocketH;
        if (newsocketH < 0) {
            error("Erreur lors de l'acceptation de la connexion du joueur");
        }

        pthread_t thread_H;
        if (pthread_create(&thread_H, NULL, (void *)connexionHumain, (void *)dataH) < 0) {
            printf("Erreur de création du thread pour le joueur\n");
            close(newsocketH); // Fermeture du socket du joueur
            
        }
        socketsH[i]=newsocketH;
        pthread_join(thread_H, NULL);
        paquetsJH[i] = initJeu(&dataH->socket, dataH->jeu);
        
    }

    paquet *paquetsJ[NB_JOUEURS];
    int sockets[NB_JOUEURS];

    // Copier les paquets du Joueur Humain dans le tableau total
    for (int i = 0; i < NB_JOUEURSH; i++) {
        paquetsJ[i] = paquetsJH[i];
        sockets[i] = socketsH[i];
    }

    // Copier les paquets du Joueur Robot dans le tableau total
    for (int i = 0; i < NB_JOUEURSR; i++) {
        paquetsJ[i + NB_JOUEURSH] = paquetsJR[i];
        sockets[i+NB_JOUEURSH] = socketsR[i];
    }
    
    
    for (int i = 0; i < NB_JOUEURS; ++i) {
        
        envoyerLignesAuClient(sockets[i], Lignes);
           
    }

    
    
    int nbmanche=0;
    while(nbmanche!=NB_MANCHES){
        
        for (int i = 0; i < NB_JOUEURSR; ++i) {
            for (int j = 0; j < 4; ++j){ 
                envoyerPaquetAuClient(socketsR[i],Lignes[j]);
            }
        }
        usleep(50000);
        for (int i = 0; i < NB_JOUEURS; ++i){ 
            char NumJoueur[256];
            sprintf(NumJoueur, "VOUS ETES LE JOUEUR NUMERO --> ( %d )\nVous Jouez la Manche Numero => %d \n", i+1,nbmanche+1);
            envoyerMessage(sockets[i], NumJoueur);
        }   
        usleep(50000);
        jouerTour(sockets,paquetsJ,Lignes,NB_JOUEURS);
        char som[256];
        int score;
        for (int i = 0; i < NB_JOUEURS; ++i){ 
            score=0;
            score=CalculScore(paquetsJ[i]);
            sprintf(som,"SCORE (NB TETE DE VACHES):%d\n",score);
            envoyerMessage(sockets[i],som);
        }  
        for (int i = 0; i < NB_JOUEURS; ++i) {
            envoyerLignesAuClient(sockets[i], Lignes);   
        }
        
        nbmanche++;
    }

    usleep(50000);
    int somme=0;
    int JPerdant=-1;
    for (int i = 0; i < NB_JOUEURS; ++i) {
        int sommetemp=0;
        
        for(int j=0; j< paquetsJ[i]->nbr;j++){
            sommetemp+=paquetsJ[i]->tab[j].points;
        }
        if(somme<sommetemp){
            somme=sommetemp;
            JPerdant=i;
        }
    }

    
    fprintf(fichier, "%d>",(JPerdant+1));
    fclose(fichier);
    
    

    for (int i = 0; i < NB_JOUEURS; ++i)
    {
        if(JPerdant==i){
            envoyerMessage(sockets[i],"Vous avez perdu! :(");
            usleep(50000);
            envoyerMessage(sockets[i],"\n");
        } 
        else{
            if(JPerdant<NB_JOUEURSH-1)
            {
                
                
                envoyerMessage(sockets[i],"Vous Avez Gagné Contre Le Joueur Humain N° : ");
                usleep(50000);
                char Perdant[256];
                sprintf(Perdant,"%d\n",(JPerdant+1));
                envoyerMessage(sockets[i],Perdant);
                
            }
            else{
                envoyerMessage(sockets[i],"Vous Avez Gagné Contre Le Joueur Robot N° : ");
                usleep(50000);
                char Perdant[256];
                sprintf(Perdant,"%d\n",(JPerdant+1));
                envoyerMessage(sockets[i],Perdant);
            }
        }   

    }
    
    pthread_exit(EXIT_SUCCESS);
    
    for (int i = 0; i < NB_JOUEURSR; ++i) {
        liberePaquet(paquetsJR[i]);
    }

    for (int i = 0; i < NB_JOUEURSH; ++i) {
        liberePaquet(paquetsJH[i]);
    }

    for (int i = 0; i < NB_JOUEURS; ++i) {
        liberePaquet(paquetsJ[i]);
    }
    liberePaquet(jeu);
    free(dataH);
    free(dataR);
    
    // ============+END+=================//
    close(sockfdH);
    close(sockfdR);
    return 0;
}

