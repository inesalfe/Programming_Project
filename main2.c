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
	GtkAdjustment *adjustment_massa_M, *adjustment_massa_m, *adjustment_constante_K, *adjustment_comp_l, *adjustment_theta, *adjustment_x, *adjustment_omega, *adjustment_v;

	GtkWidget * window;
	GtkWidget * main_box;
	GtkWidget * box_graphics;
	GtkWidget * plot;

	GtkWidget * box_image;
	GtkWidget * box_frame_image;
	GtkWidget * frame_image;
	GtkWidget * box_buttons;

	GtkWidget * box_set;
	GtkWidget * toggle_button_start;
	GtkWidget * toggle_button_pause;
	GtkWidget * button_restart;

	GtkWidget * box_choose_plot;
	GtkWidget * toggle_button_plot_1;
	GtkWidget * toggle_button_plot_2;
	GtkWidget * toggle_button_plot_3;

	GtkWidget * box_parameters;
	GtkWidget * box_massa_M;
	GtkWidget * label_massa_M;
	GtkWidget * spin_massa_M;
	GtkWidget * box_massa_m;
	GtkWidget * label_massa_m;
	GtkWidget * spin_massa_m;
	GtkWidget * box_constante_K;
	GtkWidget * label_constante_K;
	GtkWidget * spin_constante_K;
	GtkWidget * box_comp_l;
	GtkWidget * label_comp_l;
	GtkWidget * spin_comp_l;
	GtkWidget * box_theta;
	GtkWidget * label_theta;
	GtkWidget * spin_theta;
	GtkWidget * box_x;
	GtkWidget * label_x;
	GtkWidget * spin_x;
	GtkWidget *box_omega;
  	GtkWidget *label_omega;
  	GtkWidget *spin_omega;
  	GtkWidget *box_v;
  	GtkWidget *label_v;
  	GtkWidget *spin_v;
						
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
	double 	timer;
	int 	discrete_timer;
} Global;

// Reset

// gboolean func_toggle_button_restart (GtkWidget * widget, Global *global)
// {

// 	Data * data = global->data;
// 	global->timer = 0.0;
// 	clearRingBuffer(data->x_rb);
// 	clearRingBuffer(data->theta_rb);
// 	clearRingBuffer(data->t_rb);

// 	return TRUE;
// }

// Change Status

gboolean func_change_status_start (GtkWidget * widget, Global *global)
{
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

	return TRUE;
}

gboolean func_change_status_pause (GtkWidget * widget, Global *global)
{
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

	return TRUE;
}

void draw_plot(	cairo_t *cr, RingBuffer * x_rb, RingBuffer * y_rb, 
				double x_min, double x_max,
				double y_min, double y_max,
				int len,
				double xaxis_pixels, 		double yaxis_pixels,
				double x_start, 			double y_start, double* colour){

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

// Timer Handler

gboolean timeHandler(gpointer gptr){

	Global 	* global 	= (Global *) gptr;
	Data 	* data 		= global->data;
	Widgets * widgets   = global->widgets;
	double t, x, theta;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_start)) == TRUE){
		global->timer += DELTA_T;
		global->discrete_timer ++;

		solver(data->coords, data->consts, DELTA_T);

		t 		= data->coords->t; 	
		x 		= data->coords->x; 	
		theta 	= data->coords->theta; 	
		writeRingBuffer(data->t_rb, &t);
		writeRingBuffer(data->x_rb, &x);
		writeRingBuffer(data->theta_rb, &theta);
		gtk_widget_queue_draw(widgets->plot);
	}



	return TRUE;
}


// Plot Drawing Handler

gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){

	Data 	* data 		= global->data;
	Widgets * Widgets   = global->widgets;

	double t_min, t_max;
	cairo_set_source_rgb(cr, 255.0, 255.0, 255.0) ;
	cairo_paint(cr);

		// Draw X and Y axis - Plot 1 e 2

   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
    cairo_move_to(cr, 50.0, 150.0);
    cairo_line_to(cr, 350.0, 150.0);
    cairo_move_to(cr, 50.0, 250.0);
    cairo_line_to(cr, 50.0, 50.0);
    cairo_stroke(cr);

    	//Labels

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

		// Draw X and Y axis - Plot 3

   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
    cairo_move_to(cr, 50.0, 450.0);
    cairo_line_to(cr, 350.0, 450.0);
    cairo_move_to(cr, 200.0, 550.0);
    cairo_line_to(cr, 200.0, 350.0);
    cairo_stroke(cr);

		// Draw Plot

	t_max 	= MAX(global->timer, DATA_MEMORY * DELTA_T);
	t_min 	= MAX(global->timer - (DATA_MEMORY - 1) * DELTA_T, 0);

	double colour_plot_1[3];
	double colour_plot_2[3];
	double colour_plot_3[3];

		// Plot x(t)

	colour_plot_1[0] = 0.0;
	colour_plot_1[1] = 0.0;
	colour_plot_1[2] = 255.0;

	draw_plot(cr, data->t_rb, data->x_rb, 
				t_min, t_max, 
				-0.5, 0.5,
				data->t_rb->length,
				300.0, 200.0,
				50.0, 50.0, colour_plot_1);

		// Plot theta(t)

	colour_plot_2[0] = 255.0;
	colour_plot_2[1] = 0.0;
	colour_plot_2[2] = 0.0;

	draw_plot(cr, data->t_rb, data->theta_rb, 
				t_min, t_max, 
				-M_PI / 2, M_PI / 2,
				data->t_rb->length,
				300.0, 200.0,
				50.0, 50.0, colour_plot_2);

		// Plot x(theta)

	colour_plot_3[0] = 0.0;
	colour_plot_3[1] = 100.0;
	colour_plot_3[2] = 0.0;

	draw_plot(cr, data->theta_rb, data->x_rb, 
				-0.5, 0.5, 
				-M_PI / 2, M_PI / 2,
				data->t_rb->length,
				150.0, 100.0,
				125.0, 400.0, colour_plot_3);

	return TRUE;

}

int main(int argc, char** argv){

	Global * global 		= (Global *) malloc(sizeof(Global));
	Data   * data   		= (Data *) malloc(sizeof(Data));
	Widgets 	* widgets 	= (Widgets *)   malloc(sizeof(Widgets));
	GtkWidget 	* window;
	GtkWidget 	* plot;

	// Setup Data

	data->x_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->theta_rb 	= newRingBuffer(DATA_MEMORY, sizeof(double));
	data->t_rb 		= newRingBuffer(DATA_MEMORY, sizeof(double));

	data->consts 	= newConsts(0.2, 0.3, 50.0, 0.3); 
	data->coords 	= newCoords(0.1, 0.0, M_PI / 4, 0.0);

	global->timer 			= 0.0;
	global->discrete_timer 	= 0;
	global->data = data;
		
	// Setup GTK Widgets

	gtk_init (&argc, &argv);

	global->widgets = widgets;

	// Main Window

	global->widgets->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(global->widgets->window), 1200, 600);
	gtk_window_set_title (GTK_WINDOW(global->widgets->window), " Sistema massa, molas e pêndulo ");
	gtk_window_set_position (GTK_WINDOW(global->widgets->window), GTK_WIN_POS_CENTER);

	//Main Box

	global->widgets->main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_add (GTK_CONTAINER(global->widgets->window), global->widgets->main_box);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->main_box), TRUE);

	//Box dos gráficos

	global->widgets->box_graphics = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->main_box), global->widgets->box_graphics);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_graphics), TRUE);

	global->widgets->plot = gtk_drawing_area_new ();
    gtk_container_add(GTK_CONTAINER (global->widgets->box_graphics), global->widgets->plot);

	//Box da imagem

	global->widgets->box_image = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->main_box), global->widgets->box_image);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_image), TRUE);

	global->widgets->box_frame_image = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_image), global->widgets->box_frame_image);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_frame_image), TRUE);

	global->widgets->frame_image = gtk_frame_new("Representação Gráfica");
	gtk_box_pack_start(GTK_BOX(global->widgets->box_frame_image), global->widgets->frame_image, TRUE, TRUE, 10);
	gtk_frame_set_label_align( GTK_FRAME(global->widgets->frame_image), 0.25, 0.75);

	//Box dos botões

	global->widgets->box_buttons = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER(global->widgets->main_box), global->widgets->box_buttons);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_buttons), FALSE);

	//Box set - start, pause, restart

	global->widgets->box_set = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_buttons), global->widgets->box_set);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_set), TRUE);

	global->widgets->toggle_button_start = gtk_toggle_button_new_with_label ("Start");
	gtk_widget_set_size_request (global->widgets->toggle_button_start, 150, 35);
	g_signal_connect (global->widgets->toggle_button_start, "toggled", G_CALLBACK (func_change_status_start), global);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_set), global->widgets->toggle_button_start, TRUE, FALSE, 10);

	global->widgets->toggle_button_pause = gtk_toggle_button_new_with_label ("Pause");
	gtk_widget_set_size_request (global->widgets->toggle_button_pause, 150, 35);
	g_signal_connect (global->widgets->toggle_button_pause, "toggled", G_CALLBACK (func_change_status_pause), global);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_set), global->widgets->toggle_button_pause, TRUE, FALSE, 10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(global->widgets->toggle_button_pause), TRUE);

	global->widgets->button_restart = gtk_button_new_with_label ("Restart");
	gtk_widget_set_size_request (global->widgets->button_restart, 150, 35);
	//g_signal_connect (global->widgets->button_restart, "clicked", G_CALLBACK (func_toggle_button_restart), global);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_set), global->widgets->button_restart, TRUE, FALSE, 10);

	// Box Choose Plot

	global->widgets->box_choose_plot = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_buttons), global->widgets->box_choose_plot);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_choose_plot), TRUE);

	global->widgets->toggle_button_plot_1 = gtk_toggle_button_new_with_label ("Plot 1");
	gtk_widget_set_size_request (global->widgets->toggle_button_plot_1, 100, 35);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_choose_plot), global->widgets->toggle_button_plot_1, TRUE, FALSE, 10);

	global->widgets->toggle_button_plot_2 = gtk_toggle_button_new_with_label ("Plot 2");
	gtk_widget_set_size_request (global->widgets->toggle_button_plot_2, 100, 35);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_choose_plot), global->widgets->toggle_button_plot_2, TRUE, FALSE, 10);

	global->widgets->toggle_button_plot_3 = gtk_toggle_button_new_with_label ("Plot 3");
	gtk_widget_set_size_request (global->widgets->toggle_button_plot_3, 100, 35);
	gtk_box_pack_start (GTK_BOX (global->widgets->box_choose_plot), global->widgets->toggle_button_plot_3, TRUE, FALSE, 10);

	//Box Parameters and Initial Conditions

	global->widgets->box_parameters = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_buttons), global->widgets->box_parameters);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_parameters), TRUE);

	global->widgets->adjustment_massa_M = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_massa_m = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_constante_K = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_comp_l = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_theta = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_x = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_omega = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	global->widgets->adjustment_v = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);

	//Massa M - Box, Label e Spin Button

	global->widgets->box_massa_M = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_massa_M);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_massa_M), TRUE);

	global->widgets->label_massa_M = gtk_label_new ("Massa M");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_massa_M), global->widgets->label_massa_M, FALSE, TRUE, 20);

	global->widgets->spin_massa_M = gtk_spin_button_new (global->widgets->adjustment_massa_M, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_massa_M), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_massa_M), global->widgets->spin_massa_M, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_massa_M, "value-changed", G_CALLBACK (update_values), global);

  	//Massa m - Box, Label e Spin Button

	global->widgets->box_massa_m = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_massa_m);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_massa_m), TRUE);

	global->widgets->label_massa_m = gtk_label_new ("Massa m");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_massa_m), global->widgets->label_massa_m, FALSE, TRUE, 20);

	global->widgets->spin_massa_m = gtk_spin_button_new (global->widgets->adjustment_massa_m, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_massa_m), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_massa_m), global->widgets->spin_massa_m, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_massa_m, "value-changed", G_CALLBACK (update_values), global);

  	//Constante K - Box, Label e Spin Button

  	global->widgets->box_constante_K = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_constante_K);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_constante_K), TRUE);

  	global->widgets->label_constante_K = gtk_label_new ("Constante K");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_constante_K), global->widgets->label_constante_K, FALSE, TRUE, 20);

	global->widgets->spin_constante_K = gtk_spin_button_new (global->widgets->adjustment_constante_K, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_constante_K), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_constante_K), global->widgets->spin_constante_K, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_constante_K, "value-changed", G_CALLBACK (update_values), global);

  	//Comprimento l - Box, Label e Spin Button

  	global->widgets->box_comp_l = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_comp_l);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_comp_l), TRUE);

  	global->widgets->label_comp_l = gtk_label_new ("Comprimento l");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_comp_l), global->widgets->label_comp_l, FALSE, TRUE, 20);

	global->widgets->spin_comp_l = gtk_spin_button_new (global->widgets->adjustment_comp_l, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_comp_l), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_comp_l), global->widgets->spin_comp_l, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_comp_l, "value-changed", G_CALLBACK (update_values), global);

 	//Theta inicial - Box, Label e Spin Button

  	global->widgets->box_theta = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_theta);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_theta), TRUE);

  	global->widgets->label_theta = gtk_label_new ("Theta inicial");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_theta), global->widgets->label_theta, FALSE, TRUE, 20);

	global->widgets->spin_theta = gtk_spin_button_new (global->widgets->adjustment_theta, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_theta), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_theta), global->widgets->spin_theta, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_theta, "value-changed", G_CALLBACK (update_values), global);

  	//X inicial - Box, Label e Spin Button

  	global->widgets->box_x = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_x);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_x), TRUE);

  	global->widgets->label_x = gtk_label_new ("x inicial");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_x), global->widgets->label_x, FALSE, TRUE, 20);

	global->widgets->spin_x = gtk_spin_button_new (global->widgets->adjustment_x, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_x), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_x), global->widgets->spin_x, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_x, "value-changed", G_CALLBACK (update_values), global);

  	//omega inicial - Box, Label e Spin Button

  	global->widgets->box_omega = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_omega);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_omega), TRUE);

  	global->widgets->label_omega = gtk_label_new ("omega inicial");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_omega), global->widgets->label_omega, FALSE, TRUE, 20);

	global->widgets->spin_omega = gtk_spin_button_new (global->widgets->adjustment_omega, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_omega), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_omega), global->widgets->spin_omega, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_omega, "value-changed", G_CALLBACK (update_values), global);

  	//v inicial - Box, Label e Spin Button

  	global->widgets->box_v = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_container_add (GTK_CONTAINER(global->widgets->box_parameters), global->widgets->box_v);
	gtk_box_set_homogeneous (GTK_BOX(global->widgets->box_v), TRUE);

  	global->widgets->label_v = gtk_label_new ("v inicial");
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_v), global->widgets->label_v, FALSE, TRUE, 20);

	global->widgets->spin_v = gtk_spin_button_new (global->widgets->adjustment_v, 0.100, 3);
  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(global->widgets->spin_v), TRUE);
  	gtk_box_pack_start (GTK_BOX (global->widgets->box_v), global->widgets->spin_v, TRUE, FALSE, 10);
  	//g_signal_connect (global->widgets->spin_v, "value-changed", G_CALLBACK (update_values), global);

	// Set Timer

	g_timeout_add(1000 * DELTA_T, timeHandler, global);

	// Set Handlers

	g_signal_connect (G_OBJECT(global->widgets->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT(global->widgets->plot), "draw", G_CALLBACK(drawplotHandler), global);
	
	gtk_widget_show_all (global->widgets->window);
	gtk_main ();

	return 0;
}