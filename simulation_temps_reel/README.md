# Simulation temps réel — Titan et les anneaux de Saturne

Ce dossier contient une simulation en **C avec SDL2** montrant des astéroïdes orbitant autour de Saturne, avec **Titan** comme satellite naturel perturbateur.

## Contenu

- [simulation.c](simulation.c) : code source de la simulation
- [simulation](simulation) : binaire compilé

## Ce que montre la simulation

- **Saturne** au centre de la scène
- des **astéroïdes** en orbite circulaire autour de Saturne
- **Titan** en orbite autour de Saturne
- des **traînées** pour visualiser les trajectoires
- un **chronomètre** de temps simulé
- une **légende** affichée dans la fenêtre

## Interprétation physique

Cette simulation illustre que :

- les corps les plus proches de Saturne orbitent plus vite ;
- Titan perturbe faiblement mais réellement les trajectoires ;
- les astéroïdes peuvent voir leur orbite légèrement modifiée au cours du temps.

Titan est modélisé avec une distance orbitale réaliste d’environ **1 221 870 km**, ce qui donne une période d’environ **15,9 jours**.

## Compilation

```bash
gcc -O2 -o simulation simulation.c $(sdl2-config --cflags --libs) -lm
```

## Exécution

```bash
./simulation
```

## Contrôles

- **Échap** : fermer la simulation

## Affichage

- **jaune** : Saturne
- **petits points** : astéroïdes
- **rouge** : satellite naturel
