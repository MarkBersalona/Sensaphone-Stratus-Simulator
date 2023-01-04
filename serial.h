/* 
 * File:   serial.h
 * Author: mbersalona
 *
 * Created on September 12, 2018, 8:32 AM
 * RMSP Simulator serial
 */

#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Public variables
//
///////////////////////////////////////////////////////////////////////////////
extern gboolean isUSBConnectionOK;
extern gboolean isTransmitEnabled;
extern gboolean isFirstSerialFail;
extern int fd;
/* IO channel variable for file */
extern GIOChannel *gIOPointer;

///////////////////////////////////////////////////////////////////////////////
//
// Public routines
//
///////////////////////////////////////////////////////////////////////////////

int serial_open(char *name, int baud);
gboolean serial_read(GIOChannel *gio, GIOCondition condition, gpointer data); // GdkInputCondition condition )
gboolean serial_error(GIOChannel *gio, GIOCondition condition, gpointer data);


#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H */

