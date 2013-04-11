#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

static void do_drawing(cairo_t *);
static GtkWidget *window;
struct {
  int count, savecount;
  int coordx[100];
  int coordy[100];
  bool isclicked;//when a point is clicked isclicked becomes TRUE
  int change;//contains the number of the rectangle that is clicked
}

glob;

static void on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	g_print("on_draw_event");
  cr = gdk_cairo_create(gtk_widget_get_window(widget));
  do_drawing(cr);
  cairo_destroy(cr);
}

//function to draw the lines with cairo
static void do_drawing(cairo_t *cr)
{
	int rectanglesize = 5;
	g_print("do_drawing");
  	cairo_set_source_rgb(cr, 22, 22, 0);
  	cairo_set_line_width(cr, 5);
  	int i;
  	for (i = 0; i <= glob.count - 1; i++){
		cairo_rectangle(cr, glob.coordx[i]-rectanglesize/2, glob.coordy[i]-rectanglesize, rectanglesize, rectanglesize);
  	}
  	for (i = 0; i <= glob.count - 2; i++ ) {
          cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
          cairo_line_to(cr, glob.coordx[i+1], glob.coordy[i+1]);
      }
  cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
  cairo_line_to(cr, glob.coordx[0], glob.coordy[0]);
  glob.savecount = glob.count;
  //glob.count = 0;
  cairo_stroke(cr);
}

//open the Data
static void open (GtkWidget *widget, gpointer data){
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open",
										GTK_WINDOW(window),
										GTK_FILE_CHOOSER_ACTION_OPEN,
										GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
										NULL);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){
		char ch, number;
		char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_print("%s", path);
		FILE *file;
		file = fopen(path, "r");
		char *line = NULL;
		int i = 0;
		int buffer;
		//buffer = getc(file);
		while(fscanf(file, "%1d|%3d|%3d", &buffer, &glob.coordx[i], &glob.coordy[i]) != EOF){
		 	g_printf("%i|%i|%i\n", buffer, glob.coordx[i], glob.coordy[i]);
		 	i++;
		 }
		 glob.count = i;

    gtk_widget_queue_draw(window);
		fclose(file);

		}
	gtk_widget_destroy(dialog);
}

//save the Data
static void saving (GtkWidget *widget, gpointer data){
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Save", 
										GTK_WINDOW(window),
										GTK_FILE_CHOOSER_ACTION_SAVE,
										GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
										NULL);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_print("%s", path);
		FILE *file = fopen (path, "a+");
		g_print("TEST");
		int i;
		for(i = 0; i < glob.count; i++){
			if(i == 0 || i == glob.count-1){
				fprintf(file, "0|");
			}
				else{
					fprintf(file, "1|");
			}
			if(glob.coordx[i] < 100){
				if(glob.coordx[i] < 10){
					fprintf(file, "0");
				}
				fprintf(file, "0");
			}
			fprintf(file, "%i", glob.coordx[i]);
			fprintf(file, "%s", "|");
			if(glob.coordy[i] < 100){
				if(glob.coordy[i] < 10){
					fprintf(file, "0");
				}
				fprintf(file, "0");
			}
			fprintf(file, "%i", glob.coordy[i]);
			fprintf(file, "%s", "\n");
		}
		fclose(file);
	}
	gtk_widget_destroy(dialog);
}

static void draw (GtkWidget *widget, GtkWidget *data){
	g_print("draw");
	glob.count = 0;
    gtk_widget_queue_draw(data);
}

//Save the position of the clik in glob.coordx
static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
	g_print ("clicked");
	int x = event ->x;
	int y = event ->y;
	int i;
	bool onrectangle = FALSE;
	if(!glob.isclicked){
		for(i = 0; i < glob.count; i++){
			if(x > glob.coordx[i] - 10 && x < glob.coordx[i] + 10 && y < glob.coordy[i] + 10 && y > glob.coordy[i] - 10){
				onrectangle = TRUE;
				glob.isclicked = TRUE;
				glob.change = i;
			}
		}
		g_printf("%i", glob.change);
	}
	if(!onrectangle && !glob.isclicked){
		glob.coordx[glob.count] = event->x;
		glob.coordy[glob.count] = event->y;
		glob.count++;
		gtk_widget_queue_draw(widget);
	}
	if(glob.isclicked && !onrectangle){
		g_printf("istgeklickt");
		glob.coordx[glob.change] = event->x;
		glob.coordy[glob.change] = event->y;
		gtk_widget_queue_draw(widget);
		glob.isclicked = FALSE;
	}
	return TRUE;
}

int main (int   argc,
      char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *darea;
  GObject *button;

  gtk_init (&argc, &argv);

  // loading the xml-File for Gui
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "builder.ui", NULL);

  //connecting Gui with Widgets and connect the signals
  window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);


  button = gtk_builder_get_object (builder, "buttonsave");
  g_signal_connect (button, "clicked", G_CALLBACK(saving), NULL);
  
  button = gtk_builder_get_object (builder, "buttonopen");
  g_signal_connect (button, "clicked", G_CALLBACK(open), NULL);

  button = gtk_builder_get_object (builder, "buttondraw");
  g_signal_connect (button, "clicked", G_CALLBACK (draw), window);

  darea = GTK_WIDGET (gtk_builder_get_object (builder, "darea"));
  gtk_widget_add_events(darea, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT (darea), "draw", G_CALLBACK(on_draw_event), NULL);
  gtk_widget_set_size_request (darea, 500, 300);

  g_signal_connect(window, "button-press-event", G_CALLBACK (clicked), NULL);

  //set a default size for the window
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  gtk_main();

  return 0;
}