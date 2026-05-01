#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define G 6.67430e-20  // Constante gravitationnelle en km^3/kg/s^2
#define MS 5.683e26    // Masse de Saturne en kg
#define M_SAT 0.0 // 2.3e20   // Masse du satellite (exemple : Encelade) en kg
#define DT 1.0        // Pas de temps en secondes
#define N 100000        // Nombre d'itérations
#define NUM_ASTEROIDES 10 // Nombre d'astéroïdes

// Structure pour représenter un vecteur 3D
typedef struct {
    double x, y;
} Vector3D;


// Fonction pour calculer l'accélération gravitationnelle due à Saturne et au satellite
Vector3D acceleration(Vector3D pos, Vector3D sat_pos) {
    Vector3D acc = {0.0, 0.0};

    // Gravité de Saturne
    double r_s = sqrt(pos.x*pos.x + pos.y*pos.y);
    if (r_s > 1e-3) { // Évite la division par zéro
        double factor_s = -G * MS / (r_s * r_s * r_s);
        acc.x += factor_s * pos.x;
        acc.y += factor_s * pos.y;
    }

    // Gravité du satellite
    double dx = sat_pos.x-pos.x;
    double dy = sat_pos.y-pos.y;
    double r_sat = sqrt(dx * dx + dy * dy);

    if (r_sat > 1e-3) {
        double factor_sat = G*M_SAT / (r_sat*r_sat*r_sat);
        acc.x += factor_sat*dx;
        acc.y += factor_sat*dy;
    }

    return acc;
}

// Méthode de Runge-Kutta 4 pour l'intégration
void rk4(Vector3D *pos, Vector3D *vel, Vector3D sat_pos, double dt) {
    Vector3D k1_v = acceleration(*pos, sat_pos);
    Vector3D k1_r = *vel;
    Vector3D temp_pos = {pos->x+0.5 * dt * k1_r.x, pos->y+0.5 * dt * k1_r.y};
    Vector3D temp_vel = {vel->x+0.5 * dt * k1_v.x, vel->y+0.5 * dt * k1_v.y};
    Vector3D k2_v = acceleration(temp_pos, sat_pos);
    Vector3D k2_r = temp_vel;

    temp_pos.x = pos->x+0.5 * dt * k2_r.x;
    temp_pos.y = pos->y+0.5 * dt * k2_r.y;
    temp_vel.x = vel->x+0.5 * dt * k2_v.x;
    temp_vel.y = vel->y+0.5 * dt * k2_v.y;

    Vector3D k3_v = acceleration(temp_pos, sat_pos);
    Vector3D k3_r = temp_vel;

    temp_pos.x = pos->x+dt * k3_r.x;
    temp_pos.y = pos->y+dt * k3_r.y;
    temp_vel.x = vel->x+dt * k3_v.x;
    temp_vel.y = vel->y+dt * k3_v.y;

    Vector3D k4_v = acceleration(temp_pos, sat_pos);
    Vector3D k4_r = temp_vel;

    pos->x += (dt/6.0) * (k1_r.x+2 * k2_r.x+2 * k3_r.x+k4_r.x);
    pos->y += (dt/6.0) * (k1_r.y+2 * k2_r.y+2 * k3_r.y+k4_r.y);

    vel->x += (dt/6.0) * (k1_v.x+2 * k2_v.x+2 * k3_v.x+k4_v.x);
    vel->y += (dt/6.0) * (k1_v.y+2 * k2_v.y+2 * k3_v.y+k4_v.y);

}


int main() {
    srand(time(NULL));

    // Position et vitesse du satellite (ex : Encelade)
    Vector3D sat_pos = {238000.0, 0.0};
    Vector3D sat_vel = {0.0, 12.6};

    // Astéroïdes
    Vector3D ast_pos[NUM_ASTEROIDES];
    Vector3D ast_vel[NUM_ASTEROIDES];


    for (int i = 0; i < NUM_ASTEROIDES; i++) {
        double distance = 250000.0 + rand()%100000;
        double angle = (rand()%360) * M_PI/180.0;
        double speed = 9.0 + (rand()%4);

        ast_pos[i].x = distance*cos(angle);
        ast_pos[i].y = distance*sin(angle);
        ast_vel[i].x = -speed*sin(angle);
        ast_vel[i].y = speed*cos(angle);
    }

    FILE *file = fopen("trajectoire_multi.txt", "w");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        fprintf(file, "%lf %lf ", sat_pos.x, sat_pos.y);
        for (int j = 0; j < NUM_ASTEROIDES; j++) {
            fprintf(file, "%lf %lf ", ast_pos[j].x, ast_pos[j].y);
            rk4(&ast_pos[j], &ast_vel[j], sat_pos, DT);
        }
        rk4(&sat_pos, &sat_vel, (Vector3D){0,0}, DT);
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;

}