#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>

static void do_drawing(cairo_t *);

struct {
  int count;
  double coordx[100];
  double coordy[100];
}

glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
    gpointer user_data)
{
	g_print("on_draw_event");
  cr = gdk_cairo_create(gtk_widget_get_window(widget));
  do_drawing(cr);
  cairo_destroy(cr);

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
	g_print("do_drawing");
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 5);

  int i, j;
  for (i = 0; i <= glob.count - 2; i++ ) {
          cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
          cairo_line_to(cr, glob.coordx[i+1], glob.coordy[i+1]);
      }
  cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
  cairo_line_to(cr, glob.coordx[0], glob.coordy[0]);
  glob.count = 0;
  cairo_stroke(cr);
}

static void saving (GtkWidget *widget, gpointer   data)
{
	g_print ("Saving");
	FILE *file;
		file = fopen("savings.txt", "a+");
		int i = 0;
		for(i = 0; i < glob.count; i++){
			fprintf(file, "%f", glob.coordx[i]);
			fprintf(file, "%s", "|");
			fprintf(file, "%f", glob.coordy[i]);
			fprintf(file, "%s", "\n");
		}
}

static void draw (GtkWidget *widget, GtkWidget *data){
	g_print("draw");
    gtk_widget_queue_draw(data);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
	g_print ("clicked");
	glob.coordx[glob.count] = event->x;
	glob.coordy[glob.count] = event->y;
	glob.count++;
	return TRUE;
}

int main (int   argc,
      char *argv[])
{
  GtkBuilder *builder;
  GtkWidget *window, *darea;
  GObject *button;

  gtk_init (&argc, &argv);

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  button = gtk_builder_get_object (builder, "buttonsave");
  g_signal_connect (button, "clicked", G_CALLBACK (saving), NULL);

  button = gtk_builder_get_object (builder, "buttondraw");
  g_signal_connect (button, "clicked", G_CALLBACK (draw), window);

  darea = GTK_WIDGET (gtk_builder_get_object (builder, "darea"));
  g_signal_connect(G_OBJECT (darea), "draw", G_CALLBACK(on_draw_event), NULL);
  gtk_widget_set_size_request (darea, 500, 300);

  g_signal_connect(window, "button-press-event", G_CALLBACK (clicked), NULL);


  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  gtk_main();

  return 0;
}

