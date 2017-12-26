
// Global Variables
#define DATA_MEMORY = 100
#define DELTA_T		= 1e-3

#define PAUSED = 0;

typedef struct $
{
	Widgets * widgets;
	Data    * data;
	int 	status;
	double 	timer;
	int 	discrete_timer;
} Global;

typedef struct
{
	GtkWidget * window;
	GtkWidget * plot;			
} Widgets;

typedef struct 
{
	RingBuffer * x;
	RingBuffer * theta;
	double * t;
} Data;

// Timer Handler
gboolean timeHandler(gpointer data){

	Global * global = (Global *) data;
	double x;
	double theta;

	if (global->status == PLAY){
		global->timer += DELTA_T;
		global->discrete_timer ++;

		solver(coords, consts, DELTA_T);

		x 		= coords->x;
		theta 	= coords->theta; 	
		pushRingBuffer(global->x, &x);
		pushRingBuffer(global->theta, &theta);
	}
}

// // Plot Drawing Handler
// gboolean drawplotHandler(GtkWidget * widget, cairo_t * cr, Global * global){



// }

int main(int argc, char** argv){

	// Variable Initialization
	Global * global = (Global *) malloc(sizeof(Global));
	Data   * data   = (Data *)   malloc(sizeof(Data));
	GtkWidget * window;
	GtkWidget * plot;

		// (1.) Setup Status Vars
	global->status = PAUSED;

		// (2.) Setup Data

	data->x 	= (double *) malloc(DATA_MEMORY * sizeof(double));
	data->theta = (double *) malloc(DATA_MEMORY * sizeof(double));

	for(i = 0; i < DATA_MEMORY;i ++){
		data->x[i] 		= 0.0;
		data->theta[i] 	= 0.0;
	}

	consts = newConst();
	coords = newCoords();

	global->timer 			= 0.0;
	global->discrete_timer 	= 0;
	global->data = data;
		
		// (3.) Setup GTK Widgets

	gtk_init (&argc, &argv);

		// Main Window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(window), 1200, 600);
	gtk_window_set_title (GTK_WINDOW(window), " Programming - Final Project ");
	gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	global->widgets->window = window;

		// Plots
	plot = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), plot);
	global->widgets->window = plot;

		// Set Timer
	g_timeout_add(1000 * DELTA_T, timeHandler, global);

		// Set Handlers
	g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	// g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK(drawplotHandler), global);

	gtk_widget_show_all (window);
	gtk_main ();

	return 0;
}