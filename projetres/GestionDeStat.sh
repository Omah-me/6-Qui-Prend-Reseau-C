#!/bin/bash
# Vérifie si le fichier contenant i existe
if [ ! -f compteur.txt ] || [ ! -f contenu.txt ]; then
    echo "0" > compteur.txt  # Si le fichier n'existe pas, initialise i à 0
fi
# Lit la valeur de i depuis le fichier
i=$(<compteur.txt)
# Incrémente i
((i++))
# Enregistre la nouvelle valeur de i dans le fichier
echo "$i" > compteur.txt

# Lit les données depuis le fichier
donnees=$(<donnees.txt)
# Stocke les données dans des variables
IFS='>' 
read -r NB_JOUEURS NB_JOUEURSH NB_JOUEURSR NB_MANCHES JPerdant <<< "$donnees"

# Écrit les données dans un fichier texte avec des sauts de ligne
echo "NOUVELLES PARTIE" >> contenu.txt
echo "Partie Numero : $i" >> contenu.txt
echo "INFORMATION PARTIE:" >> contenu.txt
echo "Nombre De Joueurs : $NB_JOUEURS" >> contenu.txt
echo "Nombre De Joueurs Humain : $NB_JOUEURSH" >> contenu.txt
echo "Nombre De Joueurs Robot : $NB_JOUEURSR" >> contenu.txt
echo "Nombre De Manches : $NB_MANCHES" >> contenu.txt
if [ $JPerdant -lt $NB_JOUEURSH ]; then
    echo "Numero Du Joueur Qui a perdu : $JPerdant => C'est Un Joueur Humain" >> contenu.txt
else
    echo "Numero Du Joueur Qui a perdu : $JPerdant => C'est Un Joueur Robot" >> contenu.txt
fi


# Génère le contenu LaTeX dans un fichier temporaire avec des sauts de ligne
cat << EOF > contenu.tex
\documentclass{article}
\usepackage{verbatim}

\begin{document}
\verbatiminput{contenu.txt}
\end{document}
EOF

# Compile le fichier LaTeX en PDF
pdflatex contenu.tex

# Nettoie les fichiers temporaires générés par pdflatex
rm contenu.aux contenu.log contenu.tex
