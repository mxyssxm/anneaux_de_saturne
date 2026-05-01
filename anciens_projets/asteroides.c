#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define G 6.67e-11     //constante gravitationnelle
#define MS 5.68e26     //masse de Saturne
#define M_SAT 1.35e23  //masse du satellite
#define DT 8000
#define N 200
#define ASTE 10


//Vecteur en 2D
typedef struct {
	double x, y;
} vect ;

//Calcul de l'acceleration
vect acceleration(vect pos, vect sat_pos, int isAsteroid) {
	double r_s = sqrt(pos.x*pos.x + pos.y*pos.y);
	double F_s = -G*MS/(r_s*r_s*r_s);
	vect acc = {F_s*pos.x, F_s*pos.y};
	
	//gravité du satellite uniquement si c'est l'astéroïde
	if (isAsteroid) {
		double dx = sat_pos.x - pos.x;
		double dy = sat_pos.y - pos.y;
		double r_sat = sqrt(dx*dx + dy*dy);
		
		if (r_sat > 1e-3) { //évite la division par 0
			double F_sat = G*M_SAT/(r_sat*r_sat*r_sat);
			acc.x += F_sat*dx;
			acc.y += F_sat*dy;
		}
	}
	
	return acc;
}

//Methode RK4
void RK4(vect *pos, vect *vit, vect sat_pos, double dt, int isAsteroid) {
	vect k1_v = acceleration(*pos, sat_pos, isAsteroid);
	vect k1_r = *vit;
	
	vect temps_pos = {pos -> x+0.5*dt*k1_r.x, pos -> y+0.5*dt*k1_r.y};
	vect temps_vit = {vit -> x+0.5*dt*k1_v.x, vit -> y+0.5*dt*k1_v.y};
	
	vect k2_v = acceleration(temps_pos, sat_pos, isAsteroid);
	vect k2_r = temps_vit;
	
	temps_pos.x = pos->x+0.5*dt*k2_r.x;
	temps_pos.y = pos->y+0.5*dt*k2_r.y;
	temps_vit.x = vit->x+0.5*dt*k2_v.x;
	temps_vit.y = vit->y+0.5*dt*k2_v.y;
	
	vect k3_v = acceleration(temps_pos, sat_pos, isAsteroid);
	vect k3_r = temps_vit;
	
	temps_pos.x = pos->x+dt*k3_r.x;
	temps_pos.y = pos->y+dt*k3_r.y;
	temps_vit.x = vit->x+dt*k3_v.x;
	temps_vit.y = vit->y+dt*k3_v.y;
	
	vect k4_v = acceleration(temps_pos, sat_pos, isAsteroid);
	vect k4_r = temps_vit;
	
	pos->x += (dt/6.0)*(k1_r.x + 2*k2_r.x + 2*k3_r.x + k4_r.x);
	pos->y += (dt/6.0)*(k1_r.y + 2*k2_r.y + 2*k3_r.y + k4_r.y);
	
	vit->x += (dt/6.0)*(k1_v.x + 2*k2_v.x + 2*k3_v.x + k4_v.x);
	vit->y += (dt/6.0)*(k1_v.y + 2*k2_v.y + 2*k3_v.y + k4_v.y);
}
	

int main(void)  {
	//positions initiales
	vect sat_pos = {1222000000.0, 0.0};   //1 222 000 000 m
	vect sat_vit = {0.0, 5569.0};

    //initialisation des astéroïdes avec positions et vitesses aléatoires
    vect ast_pos[ASTE];
    vect ast_vit[ASTE];

    for (int i = 0; i < ASTE; i++) {
        double distance = 250000.0 + rand()%100000; //entre 250 000km et 350 000km
        double angle = (rand()%360)*M_PI/180.0;  //angle aléatoire
        double vitesse = 9.0 + (rand()%4); //entre 9 et 12 km/s

        ast_pos[i].x = distance*cos(angle);
        ast_pos[i].y = distance*sin(angle);
        ast_vit[i].x = -vitesse*cos(angle);
        ast_vit[i].y = vitesse*sin(angle);
    }
	
	FILE *fichier = fopen("trajectoire.txt", "w");
	if (!fichier) {
		printf("Erreur lors du chargement du fichier");
		return 1;
	}
	
	for (int i = 0; i < N; i++) {
		fprintf(fichier, "%lf %lf\n", sat_pos.x, sat_pos.y);
	        for (int j = 0; j < ASTE; j++) {
	            fprintf(fichier, "%lf %lf\n", ast_pos[j].x, ast_pos[j].y);
            RK4(&ast_pos[j], &ast_vit[j], sat_pos, DT, 1);
        }
		RK4(&sat_pos, &sat_vit, sat_pos, DT, 0);
        fprintf(fichier, "\n");
	}
	
	fclose(fichier);
	return 0;
}























	

