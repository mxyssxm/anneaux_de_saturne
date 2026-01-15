# Simulation numérique des anneaux de Saturne

Ce projet simule le mouvement d’astéroïdes gravitant autour de Saturne, avec ou sans l’influence d’un satellite perturbateur. Il a été réalisé dans le cadre d’un cours de **méthodes numériques** à l’EIDD (2024–2025).

## Objectifs

- Simuler les trajectoires d’astéroïdes en orbite autour de Saturne.
- Comparer différentes méthodes numériques d’intégration : Runge-Kutta 4 et Euler.
- Étudier l’effet d’un satellite sur la stabilité des orbites.
- Observer l’évolution de systèmes multi-astéroïdes avec ou sans perturbation.

---

## Fichiers du projet

| Fichier C                            | Description |
|-------------------------------------|-------------|
| `trajectoire.c`                     | 1 astéroïde autour de Saturne sans perturbation (méthode RK4) |
| `trajectoire_avec_satellite.c`     | 1 astéroïde avec un satellite perturbateur (RK4) |
| `trajectoire_avec_satellite_euler.c`| Même simulation, mais avec la méthode d’Euler |
| `aste2.c`                           | 10 astéroïdes sans satellite (orbites circulaires stables) |
| `aste3.c`                           | 10 astéroïdes avec satellite perturbateur |
| `asteroides.c`                      | 10 astéroïdes à vitesses et positions aléatoires + satellite |

---

## Compilation et exécution

Compiler un fichier :
```bash
gcc -o simulation trajectoire.c -lm
./simulation
