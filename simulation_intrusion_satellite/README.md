# Simulation d’intrusion — satellite traversant la ceinture

Ce dossier contient une simulation en **C avec SDL2** d’un satellite qui suit une **orbite elliptique** et traverse la ceinture d’astéroïdes autour de Saturne.

## Contenu

- [simulation_intrusion.c](simulation_intrusion.c) : code source de la simulation
- [simulation_intrusion](simulation_intrusion) : binaire compilé

## Ce que montre la simulation

- **Saturne** au centre
- une **ceinture d’astéroïdes** répartis autour de la planète
- un **satellite naturel perturbateur** en rouge
- une **orbite elliptique pointillée**
- des **traînées** pour suivre les déviations des trajectoires
- un **chronomètre** de temps simulé
- une **légende** dans la fenêtre

## Objectif

Cette simulation sert à visualiser l’effet du **champ gravitationnel d’un satellite** lorsqu’il traverse la ceinture :

- déviation locale des astéroïdes ;
- modification de certaines orbites ;
- possible rapprochement vers Saturne ;
- possible éjection vers l’extérieur.

## Important

Cette simulation est **démonstrative** :

- la masse du satellite a été **augmentée volontairement** pour rendre l’effet visible ;
- l’orbite a été choisie pour **couper la ceinture d’astéroïdes**.

Elle ne cherche donc pas à représenter fidèlement un satellite réel de Saturne, mais à rendre les perturbations gravitationnelles faciles à observer.

## Paramètres orbitaux du satellite

- apoapside : **350 000 km**
- périapside : **80 000 km**
- période orbitale : environ **1,18 jour simulé**

## Compilation

```bash
gcc -O2 -o simulation_intrusion simulation_intrusion.c $(sdl2-config --cflags --libs) -lm
```

## Exécution

```bash
./simulation_intrusion
```

## Contrôles

- **Échap** : fermer la simulation

## Affichage

- **jaune** : Saturne
- **petits points** : astéroïdes
- **rouge** : satellite naturel
