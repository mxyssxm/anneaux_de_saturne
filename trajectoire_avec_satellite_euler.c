#include <stdio.h>
#include <math.h>

#define G 6.67430e-20
#define MS 5.683e26
#define M_SAT 2.3e20
#define DT 10.0
#define N 1000000

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

//Méthode d'Euler
void euler(vect *pos, vect *vit, vect sat_pos, double dt, int isAsteroid) {
    vect acc = acceleration(*pos, sat_pos, isAsteroid);

    //maj de la vitesse
    vit->x += acc.x*dt;
    vit->y += acc.y*dt;

    //maj de la vitesse
    pos->x += vit->x*dt;
    pos->y += vit->y*dt;
}


int main(void) {
//positions et vitesses initiales
	vect sat_pos = {1222000000.0, 0.0};   //1 222 000 000 m
	vect sat_vit = {0.0, 5569.0};
	vect ast_pos = {3e8, 0.0}; //30 000 000 m de Saturne
	vect ast_vit = {0.0, 10000};  // m/s
	
	FILE *fichier = fopen("trajectoire_euler.txt", "w");
	if (!fichier) {
		printf("Erreur lors du chargement du fichier");
		return 1;
	}
	
	for (int i = 0; i < N; i++) {
		fprintf(fichier, "%lf %lf %lf %lf\n", ast_pos.x, ast_pos.y, sat_pos.x, sat_pos.y);
		euler(&ast_pos, &ast_vit, sat_pos, DT, 1);
		euler(&sat_pos, &sat_vit, sat_pos, DT, 0);
	}
	
	fclose(fichier);
	return 0;
}








































