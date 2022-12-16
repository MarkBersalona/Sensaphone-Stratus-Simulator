/* 
 * File:   main.c
 * Author: mbersalona
 *
 * Created on September 4, 2018, 5:22 PM
 * Rack Mounted Sentinel Pro (RMSP) Simulator
 */

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>          // File Control Definitions
#include <termios.h>        // POSIX Terminal Control definitions
#include <string.h>

#include "gconfig.h"
#include "main.h"
#include "serial.h"
#include "zone_db.h"


///////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define GLADE_LAYOUT ("RMSPSimulator.glade")

///////////////////////////////////////////////////////////////////////////////
//
// Variables and tables
//
///////////////////////////////////////////////////////////////////////////////

// Main display
GtkCssProvider *cssProvider;
GtkWindow *window;
GtkWidget *lblAppTitle;
GtkWidget *lblConnectTitle, *lblConnect;
GtkWidget *lblStatusTitle, *lblStatus;
GtkWidget *evntShutdown, *evntStop, *evntGo;

////////////////////////////////////////////////////////////////////////////
// Name:         button_shutdown
// Description:  Shutdown the application
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
static void
button_shutdown(void)
{
    gtk_main_quit();
}
// end button_shutdown

////////////////////////////////////////////////////////////////////////////
// Name:         button_stop
// Description:  Stop transmitting the JSON display strings
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
static void
button_stop(void)
{
    g_print("%s()\r\n", __FUNCTION__);
    isTransmitEnabled = FALSE;

    // For quicker HMI, change the display now
    gtk_widget_set_name((lblStatus), "ConnectionWarning");
    gtk_label_set_text(GTK_LABEL(lblStatus), "STOPPED");
}
// end button_stop

////////////////////////////////////////////////////////////////////////////
// Name:         button_go
// Description:  Allow the JSON display strings to be transmitted
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
static void
button_go(void)
{
    g_print("%s()\r\n", __FUNCTION__);
    isTransmitEnabled = TRUE;
    
    // For quicker HMI, change the display now
    gtk_widget_set_name((lblStatus), "ConnectionOK");
    gtk_label_set_text(GTK_LABEL(lblStatus), "TRANSMIT");
}
// end button_go

////////////////////////////////////////////////////////////////////////////
// Name:         main_periodic
// Description:  Main periodic code (1 second period)
// Parameters:   None
// Return:       TRUE
////////////////////////////////////////////////////////////////////////////
static gboolean
main_periodic(gpointer data)
{
    static guint32 lulCounter = 0;
    GIOStatus writeStatus;
    gsize lBytesWritten;
    
    ++lulCounter;
    
    //
    // USB reconnect
    //
    if (isUSBConnectionOK)
    {
        // Code when USB connection OK
    }
    else // USB connection not OK
    {
        // Code when USB connection failed
        // Try to open the serial-to-USB port

        g_print("\r\nlulCounter=%d   serial_open returns %d\r\n",
                lulCounter, 
                fd = serial_open("/dev/ttyUSB0",115200));
        //g_print("  fd = %d\r\n", fd);
        if (fd<0)
        {
            g_print("***ERROR*** problem opening ttyUSB0\r\n");
            isUSBConnectionOK = FALSE;
        }
        else
        {
            g_print("ttyUSB0 opened successfully!\r\n");
            isUSBConnectionOK = TRUE;
            gIOPointer = g_io_channel_unix_new(fd);  // creates the correct reference for callback
            // Set encoding
            g_io_channel_set_encoding(gIOPointer, NULL, NULL);
            g_print("\r\n%s() g_io_channel_get_encoding() returned %s\r\n", __FUNCTION__,
                    g_io_channel_get_encoding(gIOPointer));
            // Specify callback routines for serial read and error
            g_io_add_watch(gIOPointer, 
                           G_IO_IN, 
                           serial_read, 
                           NULL);
            g_io_add_watch(gIOPointer, 
                           G_IO_ERR|G_IO_HUP|G_IO_NVAL, 
                           serial_error, 
                           NULL);
        }
    }
    
    //
    // Display connection
    //
    if (isUSBConnectionOK)
    {
        // Connection OK
        gtk_widget_set_name((lblConnect), "ConnectionOK");
        gtk_label_set_text(GTK_LABEL(lblConnect), "OK");
    }
    else
    {
        // Connection disconnected
        gtk_widget_set_name((lblConnect), "ConnectionError");
        gtk_label_set_text(GTK_LABEL(lblConnect), "DISCONNECTED");
    }
    
    //
    // Display transmit
    //
    if (isTransmitEnabled)
    {
        // Transmit enabled
        gtk_widget_set_name((lblStatus), "ConnectionOK");
        gtk_label_set_text(GTK_LABEL(lblStatus), "TRANSMIT");
    }
    else
    {
        // Transmit disabled
        gtk_widget_set_name((lblStatus), "ConnectionWarning");
        gtk_label_set_text(GTK_LABEL(lblStatus), "STOPPED");
    }
        
    
    if (lulCounter%15 == 0 && isTransmitEnabled)
    {

        //////////////////////////////////////////////////
        // TEST MAB 2018.09.12
        // Random walk the zone database
        zone_db_random_walk();
        // Reconstruct zone values JSON
        zone_db_build_zone_values();
        // Randomize alarms
        zone_db_random_alarms();
        // Reconstruct alarm values JSON
        zone_db_build_alarm_values();
        //////////////////////////////////////////////////

    
        //
        // Transmit JSON
        //
        write(fd, strJSONZoneValues,strlen(strJSONZoneValues) );
        write(fd, strJSONAlarmValues,strlen(strJSONAlarmValues) );
        write(fd, strJSONZoneNames,strlen(strJSONZoneNames) );
        write(fd, strJSONZoneTypes,strlen(strJSONZoneTypes) );
        write(fd, strJSONDiagnostics,strlen(strJSONDiagnostics) );
        write(fd, strConnectionErrorMsg,strlen(strConnectionErrorMsg) );
    }
       
    
    return TRUE;
}
// end main_periodic



////////////////////////////////////////////////////////////////////////////
// Name:         main
// Description:  Main routine for RMSP Simulator
// Parameters:   Standard main arguments, unused
// Return:       0 on conventional exit; error otherwise
////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    GtkBuilder *builder;
    GError *error = NULL;

    gtk_init(&argc, &argv);
    
    //g_usleep(1000000);
    g_print("\r\n");
    g_print("=================================<=>=================================\r\n");
    g_print("                            RMSP Simulator                    \r\n");
    g_print("                               v%s.%s.%s \r\n", VERSION_A,VERSION_B,VERSION_C);
    g_print("                             2022.12.15                              \r\n");
    g_print("=================================<=>=================================\r\n");

    //
    // Initialize the zone database
    //
    zone_db_initialize();
    
    //
    // Initialize the random number generator
    //
    srand(time(NULL));
    
    //
    // Build the output JSON messages
    //
    zone_db_build_zone_values();
    zone_db_build_alarm_values();
    zone_db_build_zone_names();
    zone_db_build_zone_types();
    zone_db_build_diagnostics();
    zone_db_build_error_code(ONLINE_CONNECTION_OK);
    
    //
    // Enable CSS styling
    ///
    //GtkCssProvider *cssProvider = gtk_css_provider_new();
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "theme.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                 GTK_STYLE_PROVIDER(cssProvider),
                                 GTK_STYLE_PROVIDER_PRIORITY_USER);

    //
    // Construct a GtkBuilder instance and load our UI description
    //
    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, GLADE_LAYOUT, &error) == 0)
    {
        g_printerr ("Error loading file: %s\r\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    
    //
    // Connect signal handlers to the constructed widgets
    //
    window = gtk_builder_get_object(builder, "window1");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    evntShutdown = gtk_builder_get_object(builder, "evntShutdown");
    g_signal_connect(evntShutdown, "button-press-event", G_CALLBACK(button_shutdown), NULL);
    evntStop = gtk_builder_get_object(builder, "evntStop");
    g_signal_connect(evntStop, "button-press-event", G_CALLBACK(button_stop), NULL);
    evntGo = gtk_builder_get_object(builder, "evntGo");
    g_signal_connect(evntGo, "button-press-event", G_CALLBACK(button_go), NULL);

    //
    // Connect labels
    //
    lblAppTitle     = gtk_builder_get_object(builder, "lblAppTitle");
    lblConnectTitle = gtk_builder_get_object(builder, "lblConnectTitle");
    lblConnect      = gtk_builder_get_object(builder, "lblConnect");
    lblStatusTitle  = gtk_builder_get_object(builder, "lblStatusTitle");
    lblStatus       = gtk_builder_get_object(builder, "lblStatus");

    //
    // Disable window decoration
    //
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    // 
    // Initialize stylings
    //
    gtk_widget_set_name(lblAppTitle, "AppTitle");
    gtk_widget_set_name(lblConnectTitle, "MainTitle");
    gtk_widget_set_name(lblStatusTitle,  "MainTitle");
    
    gtk_widget_set_name(lblConnect, "ConnectionOK");
    gtk_widget_set_name(lblStatus,  "ConnectionWarning");

    // Start the timeout periodic function
    g_timeout_add(MAIN_PERIODIC_INTERVAL_MSEC, main_periodic, NULL);

    gtk_main();

    

    return (0);
}

