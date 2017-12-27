#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "data.h"
#include "circularbuffer.h"

// Global Variables
#define DATA_MEMORY 500
#define DELTA_T 	1e-2

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

void draw_plot(	cairo_t *cr, RingBuffer * x_rb, RingBuffer * y_rb, 
				double x_min, double x_max,
				double y_min, double y_max,
				int len,
				double xaxis_pixels, 		double yaxis_pixels,
				double x_start, 			double y_start){

	int i;
	int idx, idx0, idx1; 
	double x0, x1, y0, y1;

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0) ;
	
	for(i=0, idx = x_rb->head; i < x_rb->length - 1; i++, idx = (idx + 1) % x_rb->capacity){
		idx0 = idx; idx1 = (idx + 1) % x_rb->capacity;
		x0 = *((double *) (&x_rb->buffer[idx0]));
		y0 = *((double *) (&y_rb->buffer[idx0]));
		x1 = *((double *) (&x_rb->buffer[idx1]));
		y1 = *((double *) (&y_rb->buffer[idx1]));
		x0 = (x0 - x_min) / (x_max - x_min) * xaxis_pixels + x_start;
		y0 = (y0 - y_min) / (y_max - y_min) * yaxis_pixels + y_start;
		x1 = (x1 - x_min) / (x_max - x_min) * xaxis_pixels + x_start;
		y1 = (y1 - y_min) / (y_max - y_min) * yaxis_pixels + y_start;
		cairo_move_to(cr, x0, y0);
		cairo_line_to(cr, x1, y1);
	}
	cairo_stroke(cr);
}

// Timer Handler
gboolean timeHandler(gpointer gptr){

	Global 	* global 	= (Global *) gptr;
	Data 	* data 		= global->data;
	Widgets * widgets   = global->widgets;
	double t, x, theta;

	if (global->status == PLAY){
		global->timer += DELTA_T;
		global->discrete_timer ++;

		solver(data->coords, data->consts, DELTA_T);

		t 		= data->coords->t; 	
		x 		= data->coords->x; 	
		theta 	= data->coords->theta; 	
		writeRingBuffer(data->t_rb, &t);
		writeRingBuffer(data->x_rb, &x);
		writeRingBuffer(data->theta_rb, &theta);
	}

	gtk_widget_queue_draw(widgets->plot);

	return TRUE;
}

// Plot Drawing Handler
gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){

	Data 	* data 		= global->data;
	Widgets * Widgets   = global->widgets;

	double t_min, t_max;
	cairo_set_source_rgb(cr, 255.0, 255.0, 0.0) ;
	cairo_paint(cr);

	// Plot 1 - t : t-min, t_max ; theta : -pi/2, pi/2
	// Plot 2 - x : t-min, t_max ; theta : -1, 1
	// Plot 3 - theta : -pi/, pi/2, x : -1, 1 ;

	// (3.) Draw Plot
	t_max 	= MAX(global->timer, DATA_MEMORY * DELTA_T);
	t_min 	= MAX(global->timer - (DATA_MEMORY - 1) * DELTA_T, 0);

		// Plot x(t)
	draw_plot(cr, data->t_rb, data->x_rb, 
				t_min, t_max, 
				-0.5, 0.5,
				data->t_rb->length,
				400.0, 400.0,
				100.0, 100.0);

		// Plot omega(t)
	draw_plot(cr, data->t_rb, data->theta_rb, 
				t_min, t_max, 
				-M_PI / 2, M_PI / 2,
				data->t_rb->length,
				400.0, 400.0,
				100.0, 100.0);

	// 	// Plot x(theta)
	// draw_plot(cr, data->t_rb->buffer, data->x_rb->buffer, 
	// 			t_min, t_max, 
	// 			-M_PI / 2, M_PI / 2,
	// 			len,
	// 			400.0, 400.0,
	// 			100.0, 300.0);

	return TRUE;

}

int main(int argc, char** argv){

	// Variable Initialization
	int i;

	Global * global 		= (Global *) malloc(sizeof(Global));
	Data   * data   		= (Data *) malloc(sizeof(Data));
	Widgets 	* widgets 	= (Widgets *)   malloc(sizeof(Widgets));
	GtkWidget 	* window;
	GtkWidget 	* plot;

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

	global->widgets = widgets;

	// 	// Main Window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(window), 1200, 600);
	gtk_window_set_title (GTK_WINDOW(window), " Programming - Final Project ");
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	global->widgets->window = window;

		// Plots
	plot = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), plot);
	global->widgets->plot = plot;

		// Set Timer
	g_timeout_add(1000 * DELTA_T, timeHandler, global);

		// Set Handlers
	g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK(drawplotHandler), global);

	gtk_widget_show_all (window);
	gtk_main ();

	return 0;
}