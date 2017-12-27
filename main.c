#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "data.h"
#include "circularbuffer.h"

// Helpers
#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

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

void draw_plot(	cairo_t *cr, double * x, double * y, 
				double x_min, double x_max,
				double y_min, double y_max,
				int len,
				double xaxis_pixels, 	double yaxis_pixels,
				double x_start, 			double y_start){

	int i;
	for(i = 0; i < len; i++){
		x[i] = (x[i] - x_min) / (x_max - x_min) * xaxis_pixels + x_start;
		y[i] = (y[i] - y_min) / (y_max - y_min) * yaxis_pixels + y_start;
		x[i] = (x[i] - x_min) / (x_max - x_min) * xaxis_pixels + x_start;
		y[i+1] = (y[i] - y_min) / (y_max - y_min) * yaxis_pixels + y_start;
		cairo_move_to(cr, x[i], y[i]);
		cairo_line_to(cr, x[i+1], y[i+1]);
	}
	cairo_stroke(cr);
}

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
		writeRingBuffer(data->x_rb, &x);
		writeRingBuffer(data->theta_rb, &theta);
		printf("-------------- t = %lf -------------- \n", global->timer);
		printRingBuffer(data->x_rb, printDouble);
	}

	gtk_widget_queue_draw(widgets->plot);

	return TRUE;
}

// Plot Drawing Handler
gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){

	cairo_set_source_rgb() ;
	cairo_paint();

	// Plot 1 - t : t-min, t_max ; theta : -pi/2, pi/2
	// Plot 2 - x : t-min, t_max ; theta : -1, 1
	// Plot 3 - theta : -pi/, pi/2, x : -1, 1 ;

	// (3.) Draw Plot
	t_max = MAX(global->timer, DATA_MEMORY * DELTA_T);
	t_min = MAX(global->timer - DATA_MEMORY * DELTA_T, 0);
	len = 

		// Plot x(t)
	draw_plot(cr, data->t_rb->buffer, data->x_rb->buffer, 
				t_min, t_max, 
				-M_PI / 2, M_PI / 2,
				len,
				400.0, 400.0,
				100.0, 300.0);

	// 	// Plot omega(t)
	// draw_plot(cr, data->t_rb->buffer, data->x_rb->buffer, 
	// 			t_min, t_max, 
	// 			-M_PI / 2, M_PI / 2,
	// 			len,
	// 			400.0, 400.0,
	// 			100.0, 300.0);

	// 	// Plot x(theta)
	// draw_plot(cr, data->t_rb->buffer, data->x_rb->buffer, 
	// 			t_min, t_max, 
	// 			-M_PI / 2, M_PI / 2,
	// 			len,
	// 			400.0, 400.0,
	// 			100.0, 300.0);

void draw_plot(	cairo_t *cr, double * x, double * y, 
				double x_min, double x_max,
				double y_min, double y_max,
				int len,
				double xaxis_pixels, 	double yaxis_pixels,
				double x_start, 		double y_start)
}

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