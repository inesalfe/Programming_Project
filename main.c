#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "data.h"
#include "circularbuffer.h"

// Global Variables
#define DATA_MEMORY 10
#define DELTA_T 	1e-1

#define PLAY 1
#define PAUSE 0

typedef struct
{
	GtkWidget * window;
	GtkWidget * plot;			
} Widgets;

typedef struct 
{
	RingBuffer * x_rb;
	RingBuffer * theta_rb;
	RingBuffer * t_rb;
	Coords * coords;
	Consts * consts;
} Data;

typedef struct
{
	Widgets * widgets;
	Data    * data;
	int 	status;
	double 	timer;
	int 	discrete_timer;
} Global;

// Timer Handler
gboolean timeHandler(gpointer gptr){

	Global 	* global 	= (Global *) gptr;
	Data 	* data 		= global->data;
	double x;
	double theta;

	if (global->status == PLAY){
		global->timer += DELTA_T;
		global->discrete_timer ++;

		solver(data->coords, data->consts, DELTA_T);

		x 		= data->coords->x;
		theta 	= data->coords->theta; 	
		if(isfullRingBuffer(data->x_rb) == 1)
			popRingBuffer(data->x_rb);
		pushRingBuffer(data->x_rb, &x);
		pushRingBuffer(data->theta_rb, &theta);
		printf("-------------- t = %lf -------------- \n", global->timer);
		printRingBuffer(data->x_rb, printDouble);
	}

	return TRUE;
}

// // Plot Drawing Handler
// gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){



// }

int main(int argc, char** argv){

	// Variable Initialization
	int i;

	Global * global = (Global *) malloc(sizeof(Global));
	Data   * data   = (Data *)   malloc(sizeof(Data));
	GtkWidget * window;
	GtkWidget * plot;

		// (1.) Setup Status Vars
	global->status = PLAY;

		// (2.) Setup Data

	data->x_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->theta_rb 	= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->t_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));

	data->consts 	= newConsts(0.2, 0.3, 50.0, 0.3); 
	data->coords 	= newCoords(0.1, 0.0, M_PI / 4, 0.0);

	global->timer 			= 0.0;
	global->discrete_timer 	= 0;
	global->data = data;
		
		// (3.) Setup GTK Widgets

	gtk_init (&argc, &argv);

		// Main Window
	// window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	// gtk_window_set_default_size (GTK_WINDOW(window), 1200, 600);
	// gtk_window_set_title (GTK_WINDOW(window), " Programming - Final Project ");
	// gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	// global->widgets->window = window;

		// Plots
	// plot = gtk_drawing_area_new();
	// gtk_container_add(GTK_CONTAINER(window), plot);
	// global->widgets->window = plot;

		// Set Timer
	g_timeout_add(1000 * DELTA_T, timeHandler, global);

		// Set Handlers
	// g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	// g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK(drawplotHandler), global);

	// gtk_widget_show_all (window);
	gtk_main ();

	return 0;
}