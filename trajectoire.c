#include <stdio.h>
#include <math.h>

#define G 6.67e-11     //constante gravitationnelle
#define MS 5.68e26     //masse de Saturne
#define DT 8000.
#define N 100


//Vecteur en 2D
typedef struct {
	double x, y;
} vect ;

//Calcul de l'acceleration
vect acceleration(vect pos) {
	double r = sqrt(pos.x*pos.x + pos.y*pos.y);
	double F = -G*MS/(r*r*r);
	vect acc = {F*pos.x, F*pos.y};
	return acc;
}

//Methode RK4
void RK4(vect *pos, vect *vit, double dt) {
	vect k1_v = acceleration(*pos);
	vect k1_r = *vit;
	
	vect temps_pos = {pos -> x+0.5*dt*k1_r.x, pos -> y+0.5*dt*k1_r.y};
	vect temps_vit = {vit -> x+0.5*dt*k1_v.x, vit -> y+0.5*dt*k1_v.y};
	
	vect k2_v = acceleration(temps_pos);
	vect k2_r = temps_vit;
	
	temps_pos.x = pos->x+0.5*dt*k2_r.x;
	temps_pos.y = pos->y+0.5*dt*k2_r.y;
	temps_vit.x = vit->x+0.5*dt*k2_v.x;
	temps_vit.y = vit->y+0.5*dt*k2_v.y;
	
	vect k3_v = acceleration(temps_pos);
	vect k3_r = temps_vit;
	
	temps_pos.x = pos->x+dt*k3_r.x;
	temps_pos.y = pos->y+dt*k3_r.y;
	temps_vit.x = vit->x+dt*k3_v.x;
	temps_vit.y = vit->y+dt*k3_v.y;
	
	vect k4_v = acceleration(temps_pos);
	vect k4_r = temps_vit;
	
	pos->x += (dt/6.0)*(k1_r.x + 2*k2_r.x + 2*k3_r.x + k4_r.x);
	pos->y += (dt/6.0)*(k1_r.y + 2*k2_r.y + 2*k3_r.y + k4_r.y);
	
	vit->x += (dt/6.0)*(k1_v.x + 2*k2_v.x + 2*k3_v.x + k4_v.x);
	vit->y += (dt/6.0)*(k1_v.y + 2*k2_v.y + 2*k3_v.y + k4_v.y);
}

int main(void)  {
	//positions initiales
	vect pos = {3e8, 0.0}; //30 000 km de Saturne
	vect vit = {0.0, 10000};  //10 m/s
	
	FILE *fichier = fopen("trajectoire.txt", "w");
	if (!fichier) {
		printf("Erreur lors du chargement du fichier");
		return 1;
	}
	
	for (int i = 0; i < N; i++) {
		fprintf(fichier, "%lf %lf\n", pos.x, pos.y);
		RK4(&pos, &vit, DT);
	}
	
	fclose(fichier);
	return 0;
}























	
