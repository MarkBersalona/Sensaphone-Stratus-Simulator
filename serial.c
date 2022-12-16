/* 
 * File:   serial.c
 * Author: mbersalona
 *
 * Created on September 12, 2018
 * RMSP Simulator serial
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

///////////////////////////////////////////////////////////////////////////////
//
// Variables and tables
//
///////////////////////////////////////////////////////////////////////////////

//
// serial-to-USB
//
gboolean isUSBConnectionOK = FALSE;
gboolean isTransmitEnabled = TRUE;
int fd;
/* IO channel variable for file */
GIOChannel *gIOPointer;



////////////////////////////////////////////////////////////////////////////
// Name:         serial_open
// Description:  open a named IO device (file) with a variable baud rate
// Parameters:   char *name - name of port to open
//               int baud - baud rate
// Return:       
////////////////////////////////////////////////////////////////////////////
int 
serial_open(char *name, int baud)
{
    int fd; /* file descriptor of the port */

    int b_baud;
    struct termios t; /* struct for port settings */

    switch (baud) {
        case 4800:
            b_baud = B4800; break;
        case 9600:
            b_baud = B9600; break;
        case 19200:
            b_baud = B19200; break;
        case 38400:
            b_baud = B38400; break;
        case 57600:
            b_baud = B57600; break;
        case 115200:
            b_baud = B115200; break;
        default:
            b_baud = B9600; break;
        }

    fd = open (name, O_RDWR | O_NOCTTY );
    //fd = open (name, O_WRONLY );
    if (fd < 0) { return (-1); } /* open failed */

    if (tcgetattr (fd, &t) != 0) { return (-3); }
/* do settings */
    t.c_cc[VMIN] = 1;  // minimum characters received, should probably be smaller than 32
    t.c_cc[VTIME] = 1;

/* &= ~ disables bits, |= enables bits */
    t.c_iflag &= ~(BRKINT | IUTF8 | IGNBRK | IGNPAR | PARMRK | ISTRIP | INPCK | INLCR | IGNCR | ICRNL | IUCLC | IXON | IXANY | IXOFF | IMAXBEL);
    //t.c_oflag &= ~(OPOST);
    t.c_oflag |= (OPOST);
/* non-canonical */
    t.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON | ISIG | NOFLSH | TOSTOP);
    t.c_cflag &= ~(CSIZE | CSTOPB | HUPCL | PARENB);

    t.c_cflag |= CLOCAL | CREAD | CS8;
/* copy data rates into termios struct */
    if (cfsetispeed (&t, b_baud) == -1) { return (-3); }
    if (cfsetospeed (&t, b_baud) == -1) { return (-3); }
/* throw away any input data / noise */
    if (tcflush (fd, TCIFLUSH) == -1) { return (-3); }
/* Now, set the terminal attributes */
    if (tcsetattr (fd, TCSANOW, &t) == -1) { return (-3); }
    return fd;
}
// end serial_open

////////////////////////////////////////////////////////////////////////////
// Name:         serial_read
// Description:  Read serial port
// Parameters:   
// Return:       
////////////////////////////////////////////////////////////////////////////
gboolean 
serial_read(GIOChannel *gio, GIOCondition condition, gpointer data) // GdkInputCondition condition )
{
    gsize n = 1;
    static char msg[10000] = ""; // application dependent - convert back to static when processing works
    static int  count = 0;
    gchar buf;
    GIOStatus readStatus;
    gboolean lfReturnValue = TRUE;
    GError *lgerror;
   

    while (n > 0)
    {
        readStatus = g_io_channel_read_chars (gio, &buf,1, &n, NULL);  // force to read 1 char at a time?
        switch (readStatus)
        {
        case G_IO_STATUS_NORMAL:
            break;
        case G_IO_STATUS_ERROR:
            isUSBConnectionOK = FALSE;
            g_print("\r\n *** G_IO_STATUS_ERROR ***\r\n");
            lfReturnValue = FALSE;
            break;
        case G_IO_STATUS_EOF:
            isUSBConnectionOK = FALSE;
            g_print("\r\n *** G_IO_STATUS_EOF *** \r\n");
            lfReturnValue = FALSE;
            break;
        case G_IO_STATUS_AGAIN:
            isUSBConnectionOK = FALSE;
            g_print("\r\n *** G_IO_STATUS_AGAIN *** \r\n");
            lfReturnValue = FALSE;
            break;
        default:
            g_print("\r\n readStatus = %d\r\n", readStatus);
            lfReturnValue = FALSE;
            break;
        }
        if (n > 0)
        {
	    msg[count++] = buf;
	}
	if (buf == '\n')
	{
            if (count<2) count=2; // just in case \n received too early in msg
	    msg[count-2] = '\0';  // overwrite \r\n
            //g_print("%s\r\n",msg);
             
            count = 0;
	    msg[0] = '\0';  // finish up and restart
	    n = 0; // drop out after every complete message since ...read_chars seams to always block
	}
    } // while (n>0)
    return lfReturnValue;
}
// end serial_read


////////////////////////////////////////////////////////////////////////////
// Name:         serial_error
// Description:  Handle serial port errors
// Parameters:   
// Return:       
////////////////////////////////////////////////////////////////////////////
gboolean 
serial_error(GIOChannel *gio, GIOCondition condition, gpointer data)
{
    if (condition&G_IO_ERR)
    {
        g_print("\r\n ***** G_IO_ERR *****\r\n");
    }
    if (condition&G_IO_HUP)
    {
        g_print("\r\n ***** G_IO_HUP *****\r\n");
    }
    if (condition&G_IO_NVAL)
    {
        g_print("\r\n ***** G_IO_NVAL *****\r\n");
    }
    g_io_channel_shutdown(gio, FALSE, NULL);
    isUSBConnectionOK = FALSE;
    return FALSE;
}
// end serial_error

