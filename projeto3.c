#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct{

	GtkAdjustment *adjustment_massa_M, *adjustment_massa_m, *adjustment_constante_K, *adjustment_comp_l, *adjustment_theta, *adjustment_x, *adjustment_omega, *adjustment_v;

	GtkWidget * window;
	GtkWidget * main_box;
	GtkWidget * box_graphics;
	GtkWidget * box_frame_graphics;
	GtkWidget * frame_graphics;
	GtkWidget * drawarea1;

	GtkWidget * box_image;
	GtkWidget * box_frame_image;
	GtkWidget * frame_image;
	GtkWidget * box_buttons;

	GtkWidget * box_set;
	GtkWidget * toggle_button_start;
	GtkWidget * toggle_button_pause;
	GtkWidget * button_restart;

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

} ProgWidgets;

typedef struct{

	double delta_t;
	double t_curr;
	double t_max;

} ProgInfo;

typedef struct{

	double M;
	double m;
	double K;
	double l;
	double theta;
	double x;
	double omega;
	double v;

} ProgData;

typedef struct 
{

	ProgWidgets * widgets;
	ProgInfo * info;
	ProgData * data;

} Prog;

//Restart

gboolean func_toggle_button_restart (GtkWidget * widget, Prog *prog)
{

	prog->info->t_curr 	= 0.0;

	// if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause)) == TRUE)
	// {

	// gtk_widget_queue_draw(prog->widgets->drawarea1);

	// 		// Set Background  
	// cairo_set_source_rgb (cr, 0.0, 100.0, 0.0);
	// cairo_paint (cr);

	// 		// Draw X and Y axis
 //   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
 //    cairo_move_to(cr, 50.0, 300.0);
 //    cairo_line_to(cr, 300.0, 300.0);
 //    cairo_move_to(cr, 50.0, 200.0);
 //    cairo_line_to(cr, 50.0, 400.0);
 //    cairo_stroke(cr);

	// }

	return TRUE;
}

// Change Status

gboolean func_change_status_start (GtkWidget * widget, Prog *prog)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start)) == TRUE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause)) == TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start), TRUE);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start)) == FALSE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause)) == FALSE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause), TRUE);
	}

	return TRUE;
}

gboolean func_change_status_pause (GtkWidget * widget, Prog *prog)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start)) == TRUE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause)) == TRUE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start), FALSE);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start)) == FALSE && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause)) == FALSE)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause), FALSE);
	}

	return TRUE;
}

//Update Values

gboolean update_values (GtkWidget * widget, Prog *prog)
{
	prog->data->M = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_massa_M));
	prog->data->m = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_massa_m));
	prog->data->K = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_constante_K));
	prog->data->l = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_comp_l));
	prog->data->theta = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_theta));
	prog->data->x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_x));
	prog->data->omega = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_omega));
	prog->data->v = gtk_spin_button_get_value(GTK_SPIN_BUTTON(prog->widgets->spin_v));

	// printf ("Massa M = %lf\n", prog->data->M);
	// printf ("Massa m = %lf\n", prog->data->m);
	// printf ("Constante K = %lf\n", prog->data->K);
	// printf ("Comp l = %lf\n", prog->data->l);
	// printf ("Theta i = %lf\n", prog->data->theta);
	// printf ("x inicial = %lf\n", prog->data->x);
	// printf ("omega i = %lf\n", prog->data->omega);
	// printf ("v i = %lf\n", prog->data->v);

    return TRUE;
}

//Gráficos

gboolean draw_plot (GtkWidget * widget, cairo_t *cr, Prog *prog)
{
	double t, t_start, t_end;
	double x_start = 50.0;
	double y_start = 300.0;
	double x0, y0, x1, y1;
	double xpixels_per_sec = 50;
	double ypixels_per_sec = 100;

		// Set Background  
	cairo_set_source_rgb (cr, 0.0, 100.0, 0.0);
	cairo_paint (cr);

			// Draw X and Y axis
   	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);    
    cairo_move_to(cr, 50.0, 300.0);
    cairo_line_to(cr, 300.0, 300.0);
    cairo_move_to(cr, 50.0, 200.0);
    cairo_line_to(cr, 50.0, 400.0);
    cairo_stroke(cr);

        	// Draw Plots

    if(prog->info->t_curr < prog->info->t_max){
		t_start = 0; 
		t_end = prog->info->t_curr;}
	else{
		t_start = prog->info->t_curr - prog->info->t_max; 
		t_end = prog->info->t_curr;}

	for(t = t_start; t <= t_end; t = t + prog->info->delta_t){
	{
		x0 = (t - t_start) * xpixels_per_sec + x_start;
		y0 = sin(t) * ypixels_per_sec + y_start;
		x1 = (t + prog->info->delta_t - t_start) * xpixels_per_sec + x_start;
		y1 = sin(t + prog->info->delta_t) * ypixels_per_sec + y_start;
		cairo_move_to(cr, x0, y0);
		cairo_line_to(cr, x1, y1);
	}

	cairo_stroke(cr);

	}

    return TRUE;
}

gboolean timehandle(gpointer data){

	Prog * prog = (Prog *) data;

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_start)) == TRUE){
		gtk_widget_queue_draw(prog->widgets->drawarea1);
		prog->info->t_curr = prog->info->t_curr + prog->info->delta_t;
	}

	return TRUE;
}


int main (int argc, char** argv)
{
	Prog * prog 	= (Prog*)		malloc(sizeof(Prog));
	prog->widgets 	= (ProgWidgets *) malloc (sizeof (ProgWidgets));
	prog->info 		= (ProgInfo *) malloc (sizeof (ProgInfo));
	prog->data 		= (ProgData *) malloc (sizeof (ProgData));

	gtk_init (&argc, &argv);

	prog->widgets->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(prog->widgets->window), 1200, 600);
	gtk_window_set_title (GTK_WINDOW(prog->widgets->window), "Sistema de duas molas, duas roldanas e uma massa");
	gtk_window_set_position (GTK_WINDOW(prog->widgets->window), GTK_WIN_POS_CENTER);

	g_signal_connect (G_OBJECT(prog->widgets->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	//Main Box

	prog->widgets->main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_add (GTK_CONTAINER(prog->widgets->window), prog->widgets->main_box);
	gtk_box_set_homogeneous (GTK_BOX(prog->widgets->main_box), TRUE);

	//Box dos gráficos

	prog->widgets->box_graphics = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(prog->widgets->main_box), prog->widgets->box_graphics);
	gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_graphics), TRUE);

	prog->widgets->drawarea1 = gtk_drawing_area_new ();
    gtk_container_add(GTK_CONTAINER (prog->widgets->box_graphics), prog->widgets->drawarea1);

	//Box da imagem

	prog->widgets->box_image = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(prog->widgets->main_box), prog->widgets->box_image);
	gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_image), TRUE);

	// prog->widgets->box_frame_image = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_image), prog->widgets->box_frame_image);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_frame_image), TRUE);

	// prog->widgets->frame_image = gtk_frame_new("Representação Gráfica");
	// gtk_box_pack_start(GTK_BOX(prog->widgets->box_frame_image), prog->widgets->frame_image, TRUE, TRUE, 10);
	// gtk_frame_set_label_align( GTK_FRAME(prog->widgets->frame_image), 0.25, 0.75);

	//Box dos botões

	prog->widgets->box_buttons = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER(prog->widgets->main_box), prog->widgets->box_buttons);
	gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_buttons), FALSE);

	// //Box set - start, pause, restart

	// prog->widgets->box_set = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_buttons), prog->widgets->box_set);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_set), TRUE);

	// prog->widgets->toggle_button_start = gtk_toggle_button_new_with_label ("Start");
	// gtk_widget_set_size_request (prog->widgets->toggle_button_start, 150, 35);
	// g_signal_connect (prog->widgets->toggle_button_start, "toggled", G_CALLBACK (func_change_status_start), prog);
	// gtk_box_pack_start (GTK_BOX (prog->widgets->box_set), prog->widgets->toggle_button_start, TRUE, FALSE, 10);

	// prog->widgets->toggle_button_pause = gtk_toggle_button_new_with_label ("Pause");
	// gtk_widget_set_size_request (prog->widgets->toggle_button_pause, 150, 35);
	// g_signal_connect (prog->widgets->toggle_button_pause, "toggled", G_CALLBACK (func_change_status_pause), prog);
	// gtk_box_pack_start (GTK_BOX (prog->widgets->box_set), prog->widgets->toggle_button_pause, TRUE, FALSE, 10);
	// gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prog->widgets->toggle_button_pause), TRUE);

	// prog->widgets->button_restart = gtk_button_new_with_label ("Restart");
	// gtk_widget_set_size_request (prog->widgets->button_restart, 150, 35);
	// g_signal_connect (prog->widgets->button_restart, "clicked", G_CALLBACK (func_toggle_button_restart), prog);
	// gtk_box_pack_start (GTK_BOX (prog->widgets->box_set), prog->widgets->button_restart, TRUE, FALSE, 10);

	// //Box Parameters and Initial Conditions

	// prog->widgets->box_parameters = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_buttons), prog->widgets->box_parameters);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_parameters), TRUE);

	// prog->widgets->adjustment_massa_M = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_massa_m = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_constante_K = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_comp_l = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_theta = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_x = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_omega = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);
	// prog->widgets->adjustment_v = (GtkAdjustment *) gtk_adjustment_new (1.000, 0.001, 5.000, 0.100, 1, 0.0);

	// //Massa M - Box, Label e Spin Button

	// prog->widgets->box_massa_M = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_massa_M);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_massa_M), TRUE);

	// prog->widgets->label_massa_M = gtk_label_new ("Massa M");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_massa_M), prog->widgets->label_massa_M, FALSE, TRUE, 20);

	// prog->widgets->spin_massa_M = gtk_spin_button_new (prog->widgets->adjustment_massa_M, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_massa_M), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_massa_M), prog->widgets->spin_massa_M, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_massa_M, "value-changed", G_CALLBACK (update_values), prog);


 //  	//Massa m - Box, Label e Spin Button

	// prog->widgets->box_massa_m = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_massa_m);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_massa_m), TRUE);

	// prog->widgets->label_massa_m = gtk_label_new ("Massa m");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_massa_m), prog->widgets->label_massa_m, FALSE, TRUE, 20);

	// prog->widgets->spin_massa_m = gtk_spin_button_new (prog->widgets->adjustment_massa_m, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_massa_m), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_massa_m), prog->widgets->spin_massa_m, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_massa_m, "value-changed", G_CALLBACK (update_values), prog);

 //  	//Constante K - Box, Label e Spin Button

 //  	prog->widgets->box_constante_K = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_constante_K);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_constante_K), TRUE);

 //  	prog->widgets->label_constante_K = gtk_label_new ("Constante K");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_constante_K), prog->widgets->label_constante_K, FALSE, TRUE, 20);

	// prog->widgets->spin_constante_K = gtk_spin_button_new (prog->widgets->adjustment_constante_K, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_constante_K), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_constante_K), prog->widgets->spin_constante_K, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_constante_K, "value-changed", G_CALLBACK (update_values), prog);

 //  	//Comprimento l - Box, Label e Spin Button

 //  	prog->widgets->box_comp_l = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_comp_l);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_comp_l), TRUE);

 //  	prog->widgets->label_comp_l = gtk_label_new ("Comprimento l");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_comp_l), prog->widgets->label_comp_l, FALSE, TRUE, 20);

	// prog->widgets->spin_comp_l = gtk_spin_button_new (prog->widgets->adjustment_comp_l, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_comp_l), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_comp_l), prog->widgets->spin_comp_l, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_comp_l, "value-changed", G_CALLBACK (update_values), prog);

 // 	//Theta inicial - Box, Label e Spin Button

 //  	prog->widgets->box_theta = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_theta);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_theta), TRUE);

 //  	prog->widgets->label_theta = gtk_label_new ("Theta inicial");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_theta), prog->widgets->label_theta, FALSE, TRUE, 20);

	// prog->widgets->spin_theta = gtk_spin_button_new (prog->widgets->adjustment_theta, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_theta), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_theta), prog->widgets->spin_theta, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_theta, "value-changed", G_CALLBACK (update_values), prog);

 //  	//X inicial - Box, Label e Spin Button

 //  	prog->widgets->box_x = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_x);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_x), TRUE);

 //  	prog->widgets->label_x = gtk_label_new ("x inicial");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_x), prog->widgets->label_x, FALSE, TRUE, 20);

	// prog->widgets->spin_x = gtk_spin_button_new (prog->widgets->adjustment_x, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_x), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_x), prog->widgets->spin_x, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_x, "value-changed", G_CALLBACK (update_values), prog);

 //  	//omega inicial - Box, Label e Spin Button

 //  	prog->widgets->box_omega = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_omega);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_omega), TRUE);

 //  	prog->widgets->label_omega = gtk_label_new ("omega inicial");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_omega), prog->widgets->label_omega, FALSE, TRUE, 20);

	// prog->widgets->spin_omega = gtk_spin_button_new (prog->widgets->adjustment_omega, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_omega), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_omega), prog->widgets->spin_omega, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_omega, "value-changed", G_CALLBACK (update_values), prog);

 //  	//v inicial - Box, Label e Spin Button

 //  	prog->widgets->box_v = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
	// gtk_container_add (GTK_CONTAINER(prog->widgets->box_parameters), prog->widgets->box_v);
	// gtk_box_set_homogeneous (GTK_BOX(prog->widgets->box_v), TRUE);

 //  	prog->widgets->label_v = gtk_label_new ("v inicial");
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_v), prog->widgets->label_v, FALSE, TRUE, 20);

	// prog->widgets->spin_v = gtk_spin_button_new (prog->widgets->adjustment_v, 0.100, 3);
 //  	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(prog->widgets->spin_v), TRUE);
 //  	gtk_box_pack_start (GTK_BOX (prog->widgets->box_v), prog->widgets->spin_v, TRUE, FALSE, 10);
 //  	g_signal_connect (prog->widgets->spin_v, "value-changed", G_CALLBACK (update_values), prog);

  	// //Graphic

    prog->info->t_curr 		= 0.0;
    prog->info->delta_t 	= 1e-2; 
    prog->info->t_max		= 5;

    g_timeout_add(1000 * prog->info->delta_t, timehandle, prog);
    g_signal_connect (G_OBJECT(prog->widgets->drawarea1), "draw", G_CALLBACK(draw_plot), prog);


	//Main

	gtk_widget_show_all (prog->widgets->window);
	gtk_main ();

	return 0;
}