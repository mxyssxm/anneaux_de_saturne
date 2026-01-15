#include <stdio.h>
#include <math.h>

#define G 6.67e-11     //constante gravitationnelle
#define MS 5.68e26     //masse de Saturne
#define M_SAT 1.35e23  //masse du satellite
#define DT 8000
#define N 200


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
	vect ast_pos = {3e8, 0.0}; //30 000 000 m de Saturne
	vect ast_vit = {0.0, 10000};  // m/s
	
	FILE *fichier = fopen("trajectoire.txt", "w");
	if (!fichier) {
		printf("Erreur lors du chargement du fichier");
		return 1;
	}
	
	for (int i = 0; i < N; i++) {
		fprintf(fichier, "%lf %lf %lf %lf\n", ast_pos.x, ast_pos.y, sat_pos.x, sat_pos.y);
		RK4(&ast_pos, &ast_vit, sat_pos, DT, 1);
		RK4(&sat_pos, &sat_vit, sat_pos, DT, 0);
	}
	
	fclose(fichier);
	return 0;
}























	

