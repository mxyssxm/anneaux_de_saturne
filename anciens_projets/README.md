# Anciens projets — simulations numériques des anneaux de Saturne

Ce dossier contient les **premières versions** du projet réalisées en C et en Python autour de la simulation d’astéroïdes gravitant autour de Saturne, avec ou sans satellite perturbateur.

## Objectifs

- simuler les trajectoires d’astéroïdes en orbite autour de Saturne ;
- comparer différentes méthodes numériques d’intégration ;
- étudier l’effet d’un satellite sur la stabilité des orbites ;
- observer l’évolution de systèmes à plusieurs astéroïdes.

## Contenu du dossier

| Fichier | Description |
|---|---|
| [trajectoire.c](trajectoire.c) | 1 astéroïde autour de Saturne sans perturbation, méthode RK4 |
| [trajectoire.py](trajectoire.py) | visualisation Python associée |
| [trajectoire_avec_satellite.c](trajectoire_avec_satellite.c) | 1 astéroïde avec satellite perturbateur, méthode RK4 |
| [trajectoire_avec_satellite.py](trajectoire_avec_satellite.py) | visualisation Python associée |
| [trajectoire_avec_satellite_euler.c](trajectoire_avec_satellite_euler.c) | même idée avec la méthode d’Euler |
| [trajectoire_avec_satellite_euler.py](trajectoire_avec_satellite_euler.py) | visualisation Python associée |
| [aste2.c](aste2.c) | plusieurs astéroïdes sans satellite |
| [aste2.py](aste2.py) | visualisation Python multi-astéroïdes |
| [aste3.c](aste3.c) | plusieurs astéroïdes avec satellite perturbateur |
| [asteroides.c](asteroides.c) | version plus libre avec positions et vitesses aléatoires |
| [Rapport.pdf](Rapport.pdf) | rapport du projet |
| [Soutenance.pdf](Soutenance.pdf) | support de soutenance |

## Compilation

Exemple de compilation d’un programme C :

```bash
gcc -o simulation trajectoire.c -lm
./simulation
```

## Visualisation Python

Exemple :

```bash
python3 trajectoire.py
```

## Remarque

Ces fichiers correspondent à l’**ancienne organisation** du projet, avant la séparation en dossiers dédiés pour les simulations temps réel SDL2.
