# # 6-Qui-Prend-Reseau-C

# Projet de Systèmes et Réseaux (C et Shell)
    Ce projet a été réalisé dans le cadre du cours de Systèmes et Réseaux en cinquième semestre de Licence Informatique. L'objectif principal était de développer une version en ligne du jeu de cartes "6 qui prend", en intégrant les concepts fondamentaux de la programmation en C, des mécanismes réseaux, et un algorithme bot capable de jouer contre les utilisateurs.

# Technologies Utilisées
Langage principal : C
Scripts complémentaires : Shell
Protocoles Réseaux : TCP/IP (Sockets)
Environnement de Développement : Linux (recommandé)

# Description du Jeu : "6 qui prend"
"6 qui prend" est un jeu de cartes stratégique où chaque joueur tente d'éviter de collecter des cartes contenant des têtes de bœuf.
Chaque carte a une valeur et un nombre de têtes de bœuf, et les joueurs doivent placer leurs cartes de manière optimale sur des lignes.
L'objectif est de terminer la partie avec le plus petit score possible.

# Fonctionnalités Principales
  # Réseau
    Communication en temps réel entre un serveur et plusieurs clients (joueurs).
    Gestion des connexions multiples via des sockets TCP.
    Synchronisation des données entre le serveur et les clients pour garantir le bon déroulement du jeu.

  # Algorithme Bot
    Un joueur bot est intégré au système.
    Le bot suit une stratégie algorithmique pour choisir les cartes et jouer automatiquement.
    Les utilisateurs peuvent jouer contre le bot s’il n’y a pas assez de joueurs humains connectés.

  # Jeu
    Implémentation complète des règles de "6 qui prend".
    Gestion des tours de jeu, des lignes de cartes, et des scores.
    Mise à jour en temps réel des informations pour tous les joueurs.
    
  # Shell
    Scripts Shell pour automatiser certaines tâches (Comme le déroulement de la partie...).

# Structure du Projet

  # Serveur :

    Gère les connexions des clients et orchestre les règles du jeu.
    Maintient l'état global du jeu (lignes, cartes restantes, scores).
    Intègre et gère les bots lorsque des utilisateurs sont absents.

  # Client :
  
    Interface en ligne de commande permettant aux joueurs de participer à la partie.
    Reçoit les mises à jour du serveur et envoie les actions des joueurs.

  # Bot :
  
    Implémentation d’un bot jouant automatiquement.
    Le bot applique une logique simplifiée pour choisir les meilleures cartes à jouer.

  # Scripts Shell :
  
    Simplifient le démarrage du serveur, le lancement des clients, et la gestion des fichiers de logs.

# Installation et Utilisation
# Prérequis
- Système d'exploitation : Linux recommandé (Ubuntu, Fedora, etc.).
- Compilateur : GCC (ou tout compilateur supportant le C).
- Make : Pour simplifier la compilation.
# Installation
  # Cloner le dépôt :

    bash
    Copier le code
    git clone https://github.com/nom-utilisateur/6-Qui-Prend-Reseau-C.git
    cd 6-Qui-Prend-Reseau-C
    Compiler le projet :

    bash
    Copier le code
    make


# Utilisation
# Lancer le serveur :

    bash
    Copier le code
    ./serveur
    Lancer un client (dans une autre fenêtre/terminal) :
    
    bash
    Copier le code
    ./client
    Suivez les instructions affichées pour jouer. Si aucun autre joueur humain n’est disponible, le bot prendra automatiquement part à la partie.

# Concepts Appris et Appliqués
    Programmation réseau en C : sockets TCP/IP.
    Synchronisation et gestion des threads.
    Conception et implémentation d’un algorithme de jeu pour le bot.
    Automatisation avec des scripts Shell.
    Gestion des erreurs et robustesse du code.

# Licence
Ce projet est réalisé dans un cadre académique. Toute réutilisation ou modification est soumise à des conditions éducatives ou personnelles.


