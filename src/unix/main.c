#include <stdio.h>
#include <gtk/gtk.h>

static void activate(GtkApplication *app,gpointer user_data){

    GtkWidget *window = NULL;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),"cdt");
    gtk_window_set_default_size(GTK_WINDOW(window),1024,668);
    gtk_widget_show_all(window);


}

int main(int argc,char *argv[]){
    GtkApplication *app = NULL;
    int status = 0;


    app = gtk_application_new("cdt",G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app,"activate",G_CALLBACK(activate),NULL);
    status = g_application_run(G_APPLICATION(app),argc,argv);


    g_object_unref(app);
    return 0;
}
