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

#define PENDULUM_RADIUS 25
#define MASS_WIDTH 50
#define MASS_HEIGHT 25

#define PLAY 1
#define PAUSE 0

typedef struct
{
	GtkWidget * window;
	GtkWidget * plot;	
	GtkWidget * figure;				
	GtkWidget * toggle_button_plot1;		
	GtkWidget * toggle_button_plot2;			
	GtkWidget * toggle_button_plot3;	
	GtkWidget * toggle_button_start;	
	GtkWidget * toggle_button_pause;	
	GtkWidget * toggle_button_restart;	
	GtkWidget * spin_m;
	GtkWidget * spin_M;
	GtkWidget * spin_k;
	GtkWidget * spin_l;
	GtkWidget * spin_x;
	GtkWidget * spin_v;
	GtkWidget * spin_theta;
	GtkWidget * spin_omega;

} Widgets;

typedef struct 
{
	RingBuffer * x_rb;
	RingBuffer * theta_rb;
	RingBuffer * t_rb;
	Coords * initiaLCoords;
	Coords * currCoords;
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

void draw_figure(	cairo_t *cr, RingBuffer * pos_rb, RingBuffer * ang_rb, 
					double x_min, double x_max,
					double y_min, double y_max,
					double l,
					double xaxis_pixels, 		double yaxis_pixels,
					double x_start, 			double y_start){

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0) ;

	int j, i, idx;
	double pos, ang;
	double x, y;
	double delta_x, delta_y;
	double x_prev, x_next, y_prev, y_next;
	double x0, y0, x1, y1;

		// current coordenates
	idx 	= (pos_rb->tail-1) % pos_rb->capacity; 
	pos		= *((double *) (&pos_rb->buffer[idx]));
	ang 	= *((double *) (&ang_rb->buffer[idx]));
	
	x0 = (pos - x_min) / (x_max - x_min) * 350.0;
	y0 = 50.0;
	x1 = (sin(ang) * l + pos - x_min) / (x_max - x_min) * 350.0;
	y1 = (cos(ang) * l - y_min) / (y_max - y_min) * 350.0 + 50.0;


		// ball
	cairo_arc(cr, x1, y1, PENDULUM_RADIUS, 0, 2 * M_PI);
	cairo_fill(cr);

		// stem
	cairo_move_to(cr, x0, y0);
	cairo_line_to(cr, x1, y1);
	cairo_stroke(cr);

		// mass
	cairo_rectangle(cr, x0 - MASS_WIDTH / 2, y0 - MASS_HEIGHT / 2, MASS_WIDTH, MASS_HEIGHT);
	cairo_fill(cr);

		// spring
	cairo_move_to(cr, 25.0, y0);
	cairo_line_to(cr, 50.0, y0);

	cairo_move_to(cr, 25.0, y0 + 10.0);
	cairo_line_to(cr, 25.0, y0 - 10.0);

	cairo_move_to(cr, x0 - MASS_WIDTH / 2, y0);
	cairo_line_to(cr, x0 - MASS_WIDTH / 2 - 25.0, y0);

	cairo_move_to(cr, 375.0, y0);
	cairo_line_to(cr, 350.0, y0);

	cairo_move_to(cr, 375.0, y0 + 10.0);
	cairo_line_to(cr, 375.0, y0 - 10.0);

	cairo_move_to(cr, x0 + MASS_WIDTH / 2, y0);
	cairo_line_to(cr, x0 + MASS_WIDTH / 2 + 25.0, y0);

	delta_x = (x0 - MASS_WIDTH / 2 - 75.0) / 10; 
	delta_y = 25.0;
	x 	= 50.0;
	for(j = 0, x = 50.0; j < 2 ; j++, x += 300.0){
		if(j == 0) delta_x = (x0 - MASS_WIDTH / 2 - 75.0) / 10; 
		else delta_x = - (325.0 - x0 - MASS_WIDTH / 2) / 10; 
		x_prev = x;
		for(i = 0, x_next = x_prev + delta_x; i < 10 ; i++, x_next += delta_x){
			if(i % 2 == 0){ 
				y_next = y0 + delta_y;
				y_prev = y0 - delta_y;}
			else{
				y_next = y0 - delta_y;
				y_prev = y0 + delta_y;}		  
			if(i == 0) y_prev = y0;
			if(i == 9) y_next = y0;
			cairo_move_to(cr, x_prev, y_prev);
			cairo_line_to(cr, x_next, y_next);
			x_prev = x_next;}}

	cairo_stroke(cr);
}

void draw_plot_axis(cairo_t * cr){

    // Axis for Plot x(t), theta(t)

	cairo_move_to(cr, 350.0, 150.0);
    cairo_line_to(cr, 345.0, 144.0);
    cairo_move_to(cr, 350.0, 150.0);
    cairo_line_to(cr, 345.0, 155.0);

    cairo_move_to(cr, 50.0, 50.0);
    cairo_line_to(cr, 45.0, 55.0);
    cairo_move_to(cr, 50.0, 50.0);
    cairo_line_to(cr, 55.0, 55.0);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
    cairo_move_to(cr, 50.0, 150.0);
    cairo_line_to(cr, 350.0, 150.0);
    cairo_move_to(cr, 50.0, 250.0);
    cairo_line_to(cr, 50.0, 50.0);
    cairo_stroke(cr);

    // Axis for Plot theta(x)
	cairo_move_to(cr, 350.0, 450.0);
    cairo_line_to(cr, 345.0, 445.0);
    cairo_move_to(cr, 350.0, 450.0);
    cairo_line_to(cr, 345.0, 455.0);

    cairo_move_to(cr, 200.0, 350.0);
    cairo_line_to(cr, 195.0, 355.0);
    cairo_move_to(cr, 200.0, 350.0);
    cairo_line_to(cr, 205.0, 355.0);

   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
    cairo_move_to(cr, 50.0, 450.0);
    cairo_line_to(cr, 350.0, 450.0);
    cairo_move_to(cr, 200.0, 550.0);
    cairo_line_to(cr, 200.0, 350.0);
    cairo_stroke(cr);

}

void draw_plot_labels(cairo_t * cr){

    cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10);

	cairo_move_to(cr, 330, 170);  
	cairo_show_text(cr, "t (s)");

	cairo_set_source_rgb (cr, 255.0, 0.0, 0.0);
	cairo_move_to(cr, 60, 60);  
	cairo_show_text(cr, "theta (rad)"); 

	cairo_set_source_rgb (cr, 0.0, 0.0, 250.0);
	cairo_move_to(cr, 10, 60);  
	cairo_show_text(cr, "x (m)");

   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); 
	cairo_move_to(cr, 310, 470);  
	cairo_show_text(cr, "theta (rad)");

	cairo_move_to(cr, 160, 360);  
	cairo_show_text(cr, "x (m)");

}

void draw_plot(	cairo_t *cr, RingBuffer * x_rb, RingBuffer * y_rb, 
				double x_min, double x_max,
				double y_min, double y_max,
				int len,
				double xaxis_pixels, 		double yaxis_pixels,
				double x_start, 			double y_start,
				double * colour){

	int i;
	int idx, idx0, idx1; 
	double x0, x1, y0, y1;

	cairo_set_source_rgb(cr, colour[0], colour[1], colour[2]) ;
	
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

// Plot Drawing Handler
gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){

	Data 		* data 		= global->data;
	GtkWidget 	* toggle_button_plot1 = global->widgets->toggle_button_plot1, 
				* toggle_button_plot2 = global->widgets->toggle_button_plot2,
				* toggle_button_plot3 = global->widgets->toggle_button_plot3;

	double colour_plot1[3] = {0.0, 0.0, 255.0};
	double colour_plot2[3] = {255.0, 0.0, 0.0};
	double colour_plot3[3] = {0, 100.0, 0.0};

	double t_min, t_max;

	cairo_set_source_rgb(cr, 255.0, 255.0, 255.0) ;
	cairo_paint(cr);

	// (1.) Draw Axis
	draw_plot_axis(cr);

	// (2.) Draw Labels
	draw_plot_labels(cr);

	// (3.) Draw Plots
	t_max 	= MAX(global->timer, DATA_MEMORY * DELTA_T);
	t_min 	= MAX(global->timer - (DATA_MEMORY - 1) * DELTA_T, 0);

	// (3.1) x(t)
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button_plot1)) == TRUE)
		draw_plot(cr, data->t_rb, data->x_rb, 
				t_min, t_max, 
				-0.5, 0.5,
				data->t_rb->length,
				300.0, 200.0,
				50.0, 50.0, 
				colour_plot1);


	// (3.2) theta(t)
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button_plot2)) == TRUE)
	draw_plot(cr, data->t_rb, data->theta_rb, 
				t_min, t_max, 
				-M_PI / 2, M_PI / 2,
				data->t_rb->length,
				300.0, 200.0,
				50.0, 50.0, 
				colour_plot2);

	// (3.3) x(theta)
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button_plot3)) == TRUE)
	draw_plot(cr, data->theta_rb, data->x_rb,
				-M_PI / 2, M_PI / 2,
				-data->consts->l, data->consts->l,
				data->t_rb->length,
				200.0, 200.0,
				100.0, 350.0,
				colour_plot3);

	return TRUE;

}

// Figure Drawing Handler
gboolean drawfigureHandler(GtkWidget * widget, cairo_t * cr, Global * global){

	Data * data = global->data;

	double t_min, t_max;

	cairo_set_source_rgb(cr, 255.0, 255.0, 255.0) ;
	cairo_paint(cr);

	// (3.) Draw Plot
	draw_figure(cr, data->x_rb, data->theta_rb,
				-data->consts->l, data->consts->l,
				0, data->consts->l,
				data->consts->l,
				275.0, 300.0,
				27.5, 200.0);

	return TRUE;

}

gboolean spinConstsHandler(GtkWidget * widget, Global * global){

	Data * data = global->data;
	double a;

	a = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_m));
	exit(0);
	data->consts->M = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_M));
	data->consts->k = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_k));
	data->consts->l = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_l));
	return TRUE;
}

gboolean spinInitiaLCoordsHandler(GtkWidget * widget, Global * global){

	Data * data = global->data;

	data->initiaLCoords->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_x));
	data->initiaLCoords->v = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_v));
	data->initiaLCoords->theta = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_theta));
	data->initiaLCoords->omega = gtk_spin_button_get_value(GTK_SPIN_BUTTON(global->widgets->spin_omega));
	return TRUE;
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

		solver(data->currCoords, data->consts, DELTA_T);

		t 		= global->timer; 	
		x 		= data->currCoords->x; 	
		theta 	= data->currCoords->theta; 	
		writeRingBuffer(data->t_rb, &t);
		writeRingBuffer(data->x_rb, &x);
		writeRingBuffer(data->theta_rb, &theta);
	}

	gtk_widget_queue_draw(widgets->plot);
	gtk_widget_queue_draw(widgets->figure);

	return TRUE;
}

gboolean buttonStartHandler(GtkWidget * widget, Global *global){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start)) == TRUE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause)) == TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start), TRUE);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start)) == FALSE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause)) == FALSE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause), TRUE);
	}
	global->status = PLAY;
	return TRUE;

}

gboolean buttonPauseHandler(GtkWidget * widget, Global *global){

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start)) == TRUE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause)) == TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start), FALSE);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start)) == FALSE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause)) == FALSE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause), FALSE);
	}
	global->status = PAUSE;
	return TRUE;
}

gboolean buttonRestartHandler(GtkWidget * widget, Global *global){

	Data * data = global->data;

	global->timer = 0.0;
	global->discrete_timer = 0;
	clearRingBuffer(data->t_rb);
	clearRingBuffer(data->x_rb);
	clearRingBuffer(data->theta_rb);
	data->currCoords = data->initiaLCoords;

	return TRUE;
}
 

int main(int argc, char** argv){

	// Variable Initialization
	int i;

	Global * global 		= (Global *) malloc(sizeof(Global));
	Data   * data   		= (Data *) malloc(sizeof(Data));
	Widgets 	* widgets 	= (Widgets *)   malloc(sizeof(Widgets));
	GtkWidget 	* window;
	GtkWidget 	* box_main, * box_plots, * box_figure, * box_buttons;
	GtkWidget 	* box_select_plot, * toggle_button_plot1, * toggle_button_plot2, * toggle_button_plot3;
	GtkWidget 	* box_action_buttons, * toggle_button_start, * toggle_button_pause, * toggle_button_restart;
	GtkWidget 	* box_parameters;
	GtkAdjustment   * adjustment_m, * adjustment_M, * adjustment_k, * adjustment_l;
	GtkAdjustment   * adjustment_x, * adjustment_v, * adjustment_theta, * adjustment_omega;	 
	GtkWidget 	* box_m, * label_m, * spin_m;
	GtkWidget 	* box_M, * label_M, * spin_M;
	GtkWidget 	* box_l, * label_l, * spin_l;
	GtkWidget 	* box_k, * label_k, * spin_k;
	GtkWidget 	* box_x, * label_x, * spin_x;
	GtkWidget 	* box_v, * label_v, * spin_v;
	GtkWidget 	* box_omega, * label_omega, * spin_omega;
	GtkWidget 	* box_theta, * label_theta, * spin_theta;
	GtkWidget 	* plot, * figure;

		// (1.) Setup Status Vars
	global->status = PAUSE;

		// (2.) Setup Data

	data->x_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->theta_rb 	= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->t_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));

	data->consts 		= newConsts(0.2, 0.3, 50.0, 0.3); 
	data->initiaLCoords = newCoords(0.1, 0.0, M_PI / 4, 0.0);
	data->currCoords 	= data->initiaLCoords;

	global->timer 			= 0.0;
	global->discrete_timer 	= 0;
	global->data = data;
		
		// (3.) Setup GTK Widgets

	gtk_init (&argc, &argv);

	global->widgets = (Widgets *)   malloc(sizeof(Widgets));

		// (1.) Main Window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(window), 1200, 600);
	gtk_window_set_title (GTK_WINDOW(window), " Programming - Final Project ");
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	widgets->window = window;

		// (2.) Main Box
	box_main = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_add (GTK_CONTAINER(window), box_main);
	gtk_box_set_homogeneous (GTK_BOX(box_main), TRUE);

		// (2.1) Box Plots
	box_plots = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(box_main), box_plots);
	gtk_box_set_homogeneous (GTK_BOX(box_plots), TRUE);

		// (2.2) Box Figure
	box_figure = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(box_main), box_figure);
	gtk_box_set_homogeneous (GTK_BOX(box_figure), TRUE);

		// (2.3) Box Buttons
	box_buttons = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(box_main), box_buttons);
	gtk_box_set_homogeneous (GTK_BOX(box_buttons), FALSE);

			// (2.3.1) Box Action Buttons 
	box_action_buttons = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(box_buttons), box_action_buttons);
	gtk_box_set_homogeneous (GTK_BOX(box_action_buttons), TRUE);

	toggle_button_start = gtk_toggle_button_new_with_label ("Start");
	gtk_widget_set_size_request (toggle_button_start, 150, 35);
	gtk_box_pack_start (GTK_BOX (box_action_buttons), toggle_button_start, TRUE, FALSE, 10);
	widgets->toggle_button_start = toggle_button_start;

	toggle_button_pause = gtk_toggle_button_new_with_label ("Pause");
	gtk_widget_set_size_request (toggle_button_pause, 150, 35);
	gtk_box_pack_start (GTK_BOX (box_action_buttons), toggle_button_pause, TRUE, FALSE, 10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button_pause), TRUE);	
	widgets->toggle_button_pause = toggle_button_pause;

	toggle_button_restart = gtk_button_new_with_label ("Restart");
	gtk_widget_set_size_request (toggle_button_restart, 150, 35);
	gtk_box_pack_start (GTK_BOX (box_action_buttons), toggle_button_restart, TRUE, FALSE, 10);
	widgets->toggle_button_restart = toggle_button_restart;

			// (2.3.2) Box Select Plots 
	box_select_plot = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_buttons), box_select_plot);
	gtk_box_set_homogeneous (GTK_BOX(box_select_plot), TRUE);

	toggle_button_plot1 = gtk_toggle_button_new_with_label ("x(t)");
	gtk_widget_set_size_request (toggle_button_plot1, 100, 35);
	gtk_box_pack_start (GTK_BOX (box_select_plot), toggle_button_plot1, TRUE, FALSE, 10);
	widgets->toggle_button_plot1 = toggle_button_plot1;

	toggle_button_plot2 = gtk_toggle_button_new_with_label ("θ(t)");
	gtk_widget_set_size_request (toggle_button_plot2, 100, 35);
	gtk_box_pack_start (GTK_BOX (box_select_plot), toggle_button_plot2, TRUE, FALSE, 10);
	widgets->toggle_button_plot2 = toggle_button_plot2;

	toggle_button_plot3 = gtk_toggle_button_new_with_label ("x(θ)");
	gtk_widget_set_size_request (toggle_button_plot3, 100, 35);
	gtk_box_pack_start (GTK_BOX (box_select_plot), toggle_button_plot3, TRUE, FALSE, 10);
	widgets->toggle_button_plot3 = toggle_button_plot3;

		// (2.3.3) Box System Parameters 

	box_parameters = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(box_buttons), box_parameters);
	gtk_box_set_homogeneous (GTK_BOX(box_parameters), TRUE);

	adjustment_M 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_m 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_k 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_l 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_theta 		= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_x 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_omega 		= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	adjustment_v 			= (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);

			// (2.3.3.1) Massa M - Box, Label e Spin Button

	box_M = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_M);
	gtk_box_set_homogeneous (GTK_BOX(box_M), TRUE);

	label_M = gtk_label_new ("Massa M");
  	gtk_box_pack_start (GTK_BOX (box_M), label_M, FALSE, TRUE, 20);

	spin_M = gtk_spin_button_new (adjustment_M, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_M), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_M), spin_M, TRUE, FALSE, 10);
  	global->widgets->spin_M = spin_M;

  			// (2.3.3.2) Massa m - Box, Label e Spin Button

	box_m = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_m);
	gtk_box_set_homogeneous (GTK_BOX(box_m), TRUE);

	label_m = gtk_label_new ("Massa m");
  	gtk_box_pack_start (GTK_BOX (box_m), label_m, FALSE, TRUE, 20);

	spin_m = gtk_spin_button_new (adjustment_m, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_m), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_m), spin_m, TRUE, FALSE, 10);
  	global->widgets->spin_m = spin_m;

  			// (2.3.3.3) Massa k - Box, Label e Spin Button

  	box_k = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_k);
	gtk_box_set_homogeneous (GTK_BOX(box_k), TRUE);

  	label_k = gtk_label_new ("Constante K");
  	gtk_box_pack_start (GTK_BOX (box_k), label_k, FALSE, TRUE, 20);

	spin_k = gtk_spin_button_new (adjustment_k, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_k), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_k), spin_k, TRUE, FALSE, 10);
  	global->widgets->spin_k = spin_k;

  			// (2.3.3.4) Massa l - Box, Label e Spin Button

  	box_l = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_l);
	gtk_box_set_homogeneous (GTK_BOX(box_l), TRUE);

  	label_l = gtk_label_new ("Comprimento l");
  	gtk_box_pack_start (GTK_BOX (box_l), label_l, FALSE, TRUE, 20);

	spin_l = gtk_spin_button_new (adjustment_l, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_l), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_l), spin_l, TRUE, FALSE, 10);
  	global->widgets->spin_l = spin_l;

  			// (2.3.3.5) Theta - Box, Label e Spin Button

  	box_theta = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_theta);
	gtk_box_set_homogeneous (GTK_BOX(box_theta), TRUE);

  	label_theta = gtk_label_new ("Theta inicial");
  	gtk_box_pack_start (GTK_BOX (box_theta), label_theta, FALSE, TRUE, 20);

	spin_theta = gtk_spin_button_new (adjustment_theta, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_theta), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_theta), spin_theta, TRUE, FALSE, 10);
  	global->widgets->spin_theta = spin_theta;

  			// (2.3.3.6) x - Box, Label e Spin Button

  	box_x = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_x);
	gtk_box_set_homogeneous (GTK_BOX(box_x), TRUE);

  	label_x = gtk_label_new ("x inicial");
  	gtk_box_pack_start (GTK_BOX (box_x), label_x, FALSE, TRUE, 20);

	spin_x = gtk_spin_button_new (adjustment_x, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_x), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_x), spin_x, TRUE, FALSE, 10);
  	global->widgets->spin_x = spin_x;

  			// (2.3.3.7) omega - Box, Label e Spin Button

  	box_omega = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_omega);
	gtk_box_set_homogeneous (GTK_BOX(box_omega), TRUE);

  	label_omega = gtk_label_new ("omega inicial");
  	gtk_box_pack_start (GTK_BOX (box_omega), label_omega, FALSE, TRUE, 20);

	spin_omega = gtk_spin_button_new (adjustment_omega, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_omega), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_omega), spin_omega, TRUE, FALSE, 10);
  	global->widgets->spin_omega = spin_omega;

  			// (2.3.3.7) v - Box, Label e Spin Button

  	box_v = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(box_parameters), box_v);
	gtk_box_set_homogeneous (GTK_BOX(box_v), TRUE);

  	label_v = gtk_label_new ("v inicial");
  	gtk_box_pack_start (GTK_BOX (box_v), label_v, FALSE, TRUE, 20);

	spin_v = gtk_spin_button_new (adjustment_v, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_v), TRUE);
  	gtk_box_pack_start (GTK_BOX (box_v), spin_v, TRUE, FALSE, 10);
  	global->widgets->spin_v = spin_v;

		// Plots
	plot = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(box_plots), plot);
	widgets->plot = plot;

		// Figure
	figure = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(box_figure), figure);
	widgets->figure = figure;

		// Set Timer
	g_timeout_add(1000 * DELTA_T, timeHandler, global);

		// Set Handlers
	g_signal_connect (G_OBJECT(window), "destroy", 		G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT(plot), "draw", 			G_CALLBACK(drawplotHandler), global);
	g_signal_connect (G_OBJECT(figure), "draw", 		G_CALLBACK(drawfigureHandler), global);
	g_signal_connect (toggle_button_start, "toggled", 	G_CALLBACK (buttonStartHandler), global);
	g_signal_connect (toggle_button_pause, "toggled", 	G_CALLBACK (buttonPauseHandler), global);
	g_signal_connect (toggle_button_restart, "clicked", G_CALLBACK (buttonRestartHandler), global);
  	g_signal_connect (G_OBJECT(spin_M), "value-changed", 	G_CALLBACK (spinConstsHandler), global);
  	g_signal_connect (G_OBJECT(spin_m), "value-changed", 	G_CALLBACK (spinConstsHandler), global);
  	g_signal_connect (G_OBJECT(spin_l), "value-changed", 	G_CALLBACK (spinConstsHandler), global);
  	g_signal_connect (G_OBJECT(spin_k), "value-changed",G_CALLBACK (spinConstsHandler), global);
	g_signal_connect (G_OBJECT(spin_v), "value-changed", 			G_CALLBACK (spinInitiaLCoordsHandler), global);
  	g_signal_connect (G_OBJECT(spin_omega), "value-changed", 		G_CALLBACK (spinInitiaLCoordsHandler), global);
  	g_signal_connect (G_OBJECT(spin_x), "value-changed", 			G_CALLBACK (spinInitiaLCoordsHandler), global);
  	g_signal_connect (G_OBJECT(spin_theta), "value-changed", 		G_CALLBACK (spinInitiaLCoordsHandler), global);

	global->widgets = widgets;

	gtk_widget_show_all (window);
	gtk_main ();

	return 0;
}