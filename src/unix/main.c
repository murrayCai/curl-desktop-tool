#include <stdio.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

#define APP_ID "com.msoft8.cdt"

#define CURL_SETOPT(opt,val) app.curlCode = curl_easy_setopt(app.curl,(opt),(val))
#define CURL_PERFORM() app.curlCode = curl_easy_perform(app.curl)

typedef struct request_s{
    char *url;
    char *tmpFile;
}request_t;

typedef struct app_s{
    GtkApplication *gtkApp;
    int gtkAppStatus;

    GtkBuilder *mainWindowBuilder;
    GtkWidget *mainWindow;
    GtkButton *btnEnter;
    GtkEntry *txtUrl;
    GtkEntry *txtRspContent;

    CURL *curl;
    CURLcode curlCode;
    gchar *bufStart;
}app_t;

#define RSP_CONTENT_BUF_SIZE 65536
gchar rspContentBuf[RSP_CONTENT_BUF_SIZE] = {0};

app_t app = {0};

size_t on_curl_response_data(char *ptr,size_t size,size_t num,void *user_data){
    size_t total = size * num;
    memcpy(app.bufStart,ptr,total);
    app.bufStart += total;
    return total;
}

void on_btn_enter_clicked(GtkButton *btn,gpointer user_data){
    const gchar *url = gtk_entry_get_text(app.txtUrl);
    CURL_SETOPT(CURLOPT_URL,url);
    CURL_SETOPT(CURLOPT_WRITEFUNCTION,on_curl_response_data);
    app.bufStart = rspContentBuf;
    memset(rspContentBuf,0,RSP_CONTENT_BUF_SIZE);
    CURL_PERFORM();
    printf("url : %s\nresponse:\t%s\n",gtk_entry_get_text(app.txtUrl),rspContentBuf);
    gtk_entry_set_text(app.txtRspContent,rspContentBuf);
}

static void ui_init(){

    /* init main window */
    app.mainWindowBuilder = gtk_builder_new_from_file("src/ui/main.glade");
    app.mainWindow = (GtkWidget *)gtk_builder_get_object(app.mainWindowBuilder,"window_main");
    gtk_window_set_title(GTK_WINDOW(app.mainWindow),"cdt");

    /* init enter button */
    app.btnEnter = (GtkButton *)gtk_builder_get_object(app.mainWindowBuilder,"btn_enter");
    g_signal_connect(G_OBJECT(app.btnEnter),"clicked",G_CALLBACK(on_btn_enter_clicked),NULL);

    app.txtUrl = (GtkEntry *)gtk_builder_get_object(app.mainWindowBuilder,"txt_url");
    //g_signal_connect(G_OBJECT(app.txtUrl),"changed",G_CALLBACK(on_btn_enter_clicked),NULL);

    app.txtRspContent = (GtkEntry *)gtk_builder_get_object(app.mainWindowBuilder,"txt_rsp_content");

    gtk_application_add_window(app.gtkApp,GTK_WINDOW(app.mainWindow));
    //gtk_application_prefers_app_menu(app.gtkApp);

    gtk_widget_show_all(app.mainWindow);
}


static void app_init(GtkApplication *gtkApp,gpointer user_data){
    ui_init();

    /* curl init*/
    app.curl = curl_easy_init();
    
}

void on_app_exit(){

    curl_easy_cleanup(app.curl);
    printf("app exit\n");
}

int main(int argc,char *argv[]){


    app.gtkApp = gtk_application_new(APP_ID,G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app.gtkApp,"activate",G_CALLBACK(app_init),NULL);
    app.gtkAppStatus = g_application_run(G_APPLICATION(app.gtkApp),argc,argv);

    atexit(on_app_exit);

    g_object_unref(app.gtkApp);
    return 0;
}
