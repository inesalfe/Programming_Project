#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "data.h"

#define G 9.8

Consts * newConsts(double m, double M, double k, double l){
	
	Consts * consts = (Consts *) malloc(sizeof(Consts));
	consts->m = m;
	consts->M = M;
	consts->k = k;
	consts->l = l; 
	return consts;
}

Coords* newCoords(double x, double v, double theta, double omega){

	Coords * coords = (Coords *) malloc(sizeof(Coords));
	coords->t = 0.0;
	coords->x = x;
	coords->v = v;
	coords->theta = theta;
	coords->omega = omega;
	return coords;
}

void printCoords(Coords * coords){
	printf("t : %lf, x : %lf, v : %lf, theta : %lf, omega : %lf \n", coords->t, coords->x, coords->v, coords->theta, coords->omega);
}

void solver(Coords* coords, Consts* consts, double delta_t){

	double Z, A, B, C, D, E;
	double f_x, f_theta;

	Z = consts->m / consts->M;
	A = Z * G;
	B = Z * consts->l; 
	C = consts->k / consts->M; 
	D = (1 + Z) * G / consts->l;

	f_x 	= (	A * sin(coords->theta) * cos(coords->theta) 
				+ B * sin(coords->theta) * pow(coords->omega, 2) 
				- C * coords->x) 						
				/ (1 + Z * pow(sin(coords->theta), 2));
	f_theta = (- Z * sin(coords->theta) * cos(coords->theta) * pow(coords->omega, 2) 
				- D * sin(coords->theta) 
				+ C * cos(coords->theta) * coords->x) 	
				/ (1 + Z * pow(sin(coords->theta), 2));

	coords->t 	= coords->t + delta_t;
	coords->v 	= coords->v + f_x * delta_t; 
	coords->x 	= coords->x + coords->v * delta_t; 
	coords->omega = coords->omega + f_theta * delta_t;
	coords->theta = coords->theta + coords->omega * delta_t;

}

// // Testing
// int main(){

// 	Coords * coords; 
// 	Consts * consts;
// 	double t, tf = 100, delta_t = 1e-3;

// 	consts 	= newConsts(0.2, 0.3, 50.0, 0.3); 
// 	coords 	= newCoords(0.1, 0.0, M_PI / 4, 0.0);

// 	for(t = 0; t <= tf; t += delta_t){
// 		solver(coords, consts, delta_t);
// 		printCoords(coords);
// 	}

// 	return 0;
// }