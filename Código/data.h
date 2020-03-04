#ifndef DATA_H_
#define DATA_H_

typedef struct 
{
	double x;
	double v;
	double theta;
	double omega;
} Coords;

typedef struct 
{
	double m;
	double M;
	double k;
	double l;
} Consts;

Consts * newConsts(double, double, double, double);
Coords * newCoords(double, double, double, double);

void printCoords(Coords * coords);

void solver(Coords*, Consts* , double);

#endif