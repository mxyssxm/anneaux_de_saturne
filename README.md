# Anneaux de Saturne — simulations numériques en C

Projet de simulation des **anneaux de Saturne** et de l’influence gravitationnelle de satellites naturels sur des particules en orbite.

Le dépôt contient :

- des **anciens programmes de calcul** en C et Python ;
- une **simulation temps réel en C avec SDL2** ;
- une **simulation d’intrusion gravitationnelle** d’un satellite dans la ceinture d’astéroïdes.

## Aperçu

L’objectif est d’observer visuellement comment des astéroïdes orbitent autour de Saturne, puis comment leurs trajectoires sont modifiées par la présence d’un satellite :

- orbites quasi circulaires stables ;
- perturbations gravitationnelles ;
- déviation des trajectoires ;
- possible éjection ou rapprochement vers Saturne.

## Structure du projet

### 1. Simulation principale en temps réel

Dossier : [simulation_temps_reel](simulation_temps_reel)

Contenu :

- [simulation_temps_reel/simulation.c](simulation_temps_reel/simulation.c) : visualisation temps réel en C avec SDL2
- [simulation_temps_reel/simulation](simulation_temps_reel/simulation) : binaire compilé

Cette simulation affiche :

- Saturne au centre ;
- des astéroïdes orbitant autour d’elle ;
- un satellite naturel perturbateur ;
- les trajectoires en pointillé / traînée.

### 2. Simulation d’intrusion d’un satellite dans la ceinture

Dossier : [simulation_intrusion_satellite](simulation_intrusion_satellite)

Contenu :

- [simulation_intrusion_satellite/simulation_intrusion.c](simulation_intrusion_satellite/simulation_intrusion.c)
- [simulation_intrusion_satellite/simulation_intrusion](simulation_intrusion_satellite/simulation_intrusion)

Cette version montre un satellite sur une **orbite elliptique** qui traverse la ceinture d’astéroïdes et perturbe directement leurs orbites.

### 3. Anciens programmes

Dossier : [anciens_projets](anciens_projets)

On y retrouve les premières versions du projet :

- simulation d’un seul astéroïde ;
- comparaison Euler / Runge-Kutta ;
- systèmes avec plusieurs astéroïdes ;
- scripts Python de visualisation.

## Technologies utilisées

- **C** pour les simulations numériques
- **SDL2** pour l’affichage temps réel
- **Python** pour certaines visualisations historiques

## Lancer les simulations

### Pré-requis

- compilateur C (`gcc`)
- bibliothèque **SDL2**

Sur macOS avec Homebrew :

```bash
brew install sdl2
```

### Lancer la simulation temps réel

```bash
cd simulation_temps_reel
gcc -O2 -o simulation simulation.c $(sdl2-config --cflags --libs) -lm
./simulation
```

### Lancer la simulation d’intrusion

```bash
cd simulation_intrusion_satellite
gcc -O2 -o simulation_intrusion simulation_intrusion.c $(sdl2-config --cflags --libs) -lm
./simulation_intrusion
```

## Ce qu’on observe physiquement

Les simulations illustrent plusieurs idées importantes de mécanique orbitale :

- plus un corps est proche de Saturne, plus sa vitesse orbitale est grande ;
- un satellite peut transférer de l’énergie à un astéroïde ;
- une orbite circulaire peut devenir elliptique après perturbation ;
- un astéroïde peut se rapprocher de Saturne, être éjecté vers l’extérieur, ou rester lié gravitationnellement.

Le critère de liaison gravitationnelle dépend du signe de l’énergie mécanique spécifique :

$$
\varepsilon = \frac{v^2}{2} - \frac{GM}{r}
$$

- si $\varepsilon < 0$ : l’astéroïde reste lié à Saturne ;
- si $\varepsilon > 0$ : il est sur une trajectoire d’échappement.

## Contexte

Projet réalisé dans le cadre d’un travail de **méthodes numériques** à l’EIDD, puis prolongé ici avec une partie **visualisation temps réel**.

## Auteur

Projet développé par Meyssem.
