/* 
 * File:   zone_db.c
 * Author: mbersalona
 *
 * Created on September 12, 2018
 * RMSP Simulator zone database
 */

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "zone_db.h"
#include "gconfig.h"

///////////////////////////////////////////////////////////////////////////////
//
// Variables and tables
//
///////////////////////////////////////////////////////////////////////////////

//
// The zone database
//
Zone_record ZoneDB[IO_NUMBER_OF_ZONES];
guint8 SortedZoneIndex[IO_NUMBER_OF_ZONES];
guint8 firstDisplayedZoneIndex = 0;
gboolean fAutoscrollEnabled = TRUE;
guint8 ucAlarmCount=0, ucEnabledCount=0, ucDisabledCount=0;
guint32 ulStandbyMode;
guint16 uiStandbyState;
guint32 ulTimestamp;
guint32 ulElapsedTimeSinceDataUpdate_sec = 0;


char strJSONZoneValues[10000];
char strJSONAlarmValues[10000];
char strJSONZoneNames[10000];
char strJSONZoneTypes[10000];
char strJSONDiagnostics[200];
char strConnectionErrorMsg[40];

char lcTempString[100];


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_initialize
// Description:  Initialize the zone database with default values
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_initialize(void)
{
    int zone;
    
    g_print("\r\n%s() Initializing the zone database\r\n", __FUNCTION__);
    
    for (zone = 0; zone < IO_NUMBER_OF_ZONES; ++zone)
    {
        // Initialize the sorted zone index
        SortedZoneIndex[zone] = zone;
        //g_print("SortedZoneIndex[%d] = %d\r\n", zone, SortedZoneIndex[zone]);
        
        // Initialize the power, battery and lithium zones
        if (zone == IO_POWER_OFFSET)
        {
            strcpy(ZoneDB[zone].name, "POWER");
            ZoneDB[zone].value = 8191;
            ZoneDB[zone].minimum = 0;
            ZoneDB[zone].maximum = 0;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            ZoneDB[zone].enabled = 1;
            ZoneDB[zone].type = INTERNAL_POWER;
            strcpy(ZoneDB[zone].units, "V");
            //g_print("zone=%d   POWER\r\n", zone+1);
            //g_print("\t name  = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type  = %d\r\n", ZoneDB[zone].type);
            //g_print("\t value = %d\r\n", ZoneDB[zone].value);
            //g_print("\t units = %s\r\n", ZoneDB[zone].units);
        }
        else if (zone == IO_BATTERY_OFFSET)
        {
            strcpy(ZoneDB[zone].name, "BATTERY");
            ZoneDB[zone].value = 100000;
            ZoneDB[zone].minimum = 0;
            ZoneDB[zone].maximum = 0;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            ZoneDB[zone].enabled = 1;
            ZoneDB[zone].type = INTERNAL_BATTERY;
            strcpy(ZoneDB[zone].units, "%");
            //g_print("zone=%d   BATTERY\r\n", zone+1);
            //g_print("\t name = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type = %d\r\n", ZoneDB[zone].type);
        }
        else if (zone == IO_LITHIUM_OFFSET)
        {
            strcpy(ZoneDB[zone].name, "LITHIUM");
            ZoneDB[zone].value = 100000;
            ZoneDB[zone].minimum = 0;
            ZoneDB[zone].maximum = 0;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            ZoneDB[zone].enabled = 1;
            ZoneDB[zone].type = INTERNAL_BATTERY;
            strcpy(ZoneDB[zone].units, "%");
            //g_print("zone=%d   LITHIUM\r\n", zone+1);
            //g_print("\t name = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type = %d\r\n", ZoneDB[zone].type);
        }
        
        // Initialize the standard input zones
        else if (zone >= IO_STANDARD_OFFSET && 
                 zone < IO_STANDARD_OFFSET+IO_NUMBER_OF_STANDARD_INPUTS)
        {
            sprintf(ZoneDB[zone].name, "RMSP Simulator Zone %d", zone+1-IO_STANDARD_OFFSET);
            ZoneDB[zone].value = 3140;
            ZoneDB[zone].minimum = 700;
            ZoneDB[zone].maximum = 5400;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            ZoneDB[zone].enabled = 1;
            ZoneDB[zone].type = TEMP_28K_C;
            strcpy(ZoneDB[zone].units, "C");
            //g_print("zone=%d   Zone %d\r\n", zone+1, zone+1-IO_STANDARD_OFFSET);
            //g_print("\t name = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type = %d\r\n", ZoneDB[zone].type);
        }
        
        // Initialize the MODBUS zones
        else if (zone >= IO_MODBUS_OFFSET && 
                 zone < IO_MODBUS_OFFSET+IO_NUMBER_OF_MODBUS_INPUTS)
        {
            sprintf(ZoneDB[zone].name, "RMSP Simulator Zone MB%02d", zone+1-IO_MODBUS_OFFSET);
            ZoneDB[zone].value = 40200;
            ZoneDB[zone].minimum = 37100;
            ZoneDB[zone].maximum = 43500;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            //ZoneDB[zone].enabled = 1;
            ZoneDB[zone].enabled = 0;
            ZoneDB[zone].type = MODBUS_INPUT;
            strcpy(ZoneDB[zone].units, "F");
            //g_print("zone=%d   Zone MB%02d\r\n", zone+1, zone+1-IO_MODBUS_OFFSET);
            //g_print("\t name = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type = %d\r\n", ZoneDB[zone].type);
        }
        
        // Initialize the output zones
        else if (zone >= IO_RELAY_OFFSET && 
                 zone < IO_RELAY_OFFSET+IO_NUMBER_OF_PHYSICAL_OUTPUTS)
        {
            strcpy(ZoneDB[zone].name, "Output");
            ZoneDB[zone].value = 0;
            ZoneDB[zone].minimum = 0;
            ZoneDB[zone].maximum = 0;
            ZoneDB[zone].alarm_status = 0;
            ZoneDB[zone].alarm_range = 0;
            ZoneDB[zone].alarm_unack = 0;
            ZoneDB[zone].enabled = 1;
            ZoneDB[zone].type = RELAY_OUTPUT;
            //g_print("zone=%d   Zone O%d\r\n", zone+1, zone+1-IO_RELAY_OFFSET);
            //g_print("\t name = %s\r\n",ZoneDB[zone].name);
            //g_print("\t type = %d\r\n", ZoneDB[zone].type);
        }
    }
    
    ////////////////////////////////////////////////////////
    // MAB 2020.01.17
    // For an HVAC show, modify the database
    // - Power, Battery and Lithium - unchanged
    // - 12 standard zones: customized
    // -  6 MODBUS zones, customized; remaining MB disabled
    // -  2 output zones: named "Output"
    ////////////////////////////////////////////////////////
    zone = IO_STANDARD_OFFSET; // standard Zone 1
    sprintf(ZoneDB[zone].name, "Heater Output");
    ZoneDB[zone].value = 74500;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = TEMP_28K_F;
    strcpy(ZoneDB[zone].units, "F");
    
    zone = IO_STANDARD_OFFSET+1; // standard Zone 2
    sprintf(ZoneDB[zone].name, "Fan Status");
    ZoneDB[zone].value = 1;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = NORMALLY_OPEN;
    strcpy(ZoneDB[zone].units, "Running");
    
    zone = IO_STANDARD_OFFSET+2; // standard Zone 3
    sprintf(ZoneDB[zone].name, "Duct Air Flow");
    ZoneDB[zone].value = 8494;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = FOUR_TO_TWENTY;
    strcpy(ZoneDB[zone].units, "ft/min");
    
    zone = IO_STANDARD_OFFSET+3; // standard Zone 4
    sprintf(ZoneDB[zone].name, "HVAC Differential Pressure");
    ZoneDB[zone].value = 12;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = FOUR_TO_TWENTY;
    strcpy(ZoneDB[zone].units, "inWC");
    
    zone = IO_STANDARD_OFFSET+4; // standard Zone 5
    sprintf(ZoneDB[zone].name, "CO Basement Parking");
    ZoneDB[zone].value = 1616;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = FOUR_TO_TWENTY;
    strcpy(ZoneDB[zone].units, "ppm");
    
    zone = IO_STANDARD_OFFSET+5; // standard Zone 6
    sprintf(ZoneDB[zone].name, "Duct Temperature");
    ZoneDB[zone].value = 45900;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = TEMP_28K_F;
    strcpy(ZoneDB[zone].units, "F");
    
    zone = IO_STANDARD_OFFSET+6; // standard Zone 7
    sprintf(ZoneDB[zone].name, "Blower Vibration");
    ZoneDB[zone].value = 122099;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 15000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = FOUR_TO_TWENTY;
    strcpy(ZoneDB[zone].units, "mm/s");
    
    zone = IO_STANDARD_OFFSET+7; // standard Zone 8
    sprintf(ZoneDB[zone].name, "Humidity 40th Floor");
    ZoneDB[zone].value = 51360;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = FOUR_TO_TWENTY;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_STANDARD_OFFSET+8; // standard Zone 9
    sprintf(ZoneDB[zone].name, "Humidity 20th Floor");
    ZoneDB[zone].value = 0;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = NORMALLY_OPEN;
    strcpy(ZoneDB[zone].units, "OK");
    
    zone = IO_STANDARD_OFFSET+9; // standard Zone 10
    sprintf(ZoneDB[zone].name, "Boiler Room Leak");
    ZoneDB[zone].value = 0;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = NORMALLY_OPEN;
    strcpy(ZoneDB[zone].units, "OK");
    
    zone = IO_STANDARD_OFFSET+10; // standard Zone 11
    sprintf(ZoneDB[zone].name, "MDF CRAC Alarm");
    ZoneDB[zone].value = 0;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = NORMALLY_OPEN;
    strcpy(ZoneDB[zone].units, "OK");
    
    zone = IO_STANDARD_OFFSET+11; // standard Zone 12
    sprintf(ZoneDB[zone].name, "Data Center Ups Alarm");
    ZoneDB[zone].value = 0;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 10000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = NORMALLY_OPEN;
    strcpy(ZoneDB[zone].units, "OK");
    
    zone = IO_MODBUS_OFFSET; // MODBUS Zone 1
    sprintf(ZoneDB[zone].name, "Total kWH");
    ZoneDB[zone].value = 24246000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "KWH");
    
    zone = IO_MODBUS_OFFSET+1; // MODBUS Zone 2
    sprintf(ZoneDB[zone].name, "Voltage, L-N 3 Phase");
    ZoneDB[zone].value = 120142;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "VAC");
    
    zone = IO_MODBUS_OFFSET+2; // MODBUS Zone 3
    sprintf(ZoneDB[zone].name, "Current, 3 Phase");
    ZoneDB[zone].value = 2806;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "Amps");
    
    zone = IO_MODBUS_OFFSET+5; // MODBUS Zone 6
    sprintf(ZoneDB[zone].name, "Current, Phase A");
    ZoneDB[zone].value = 2804;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "Amps");
    
    zone = IO_MODBUS_OFFSET+6; // MODBUS Zone 7
    sprintf(ZoneDB[zone].name, "Total Power Factor");
    ZoneDB[zone].value = 892;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "");
    
    zone = IO_MODBUS_OFFSET+7; // MODBUS Zone 8
    sprintf(ZoneDB[zone].name, "Real Power, Phase A");
    ZoneDB[zone].value = 8985374;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 34246000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "KW");
    
    
    ////////////////////////////////////////////////////////
    // MAB 2022.12.12
    // For testing Stratus Display Mk.2, add to the database
    // - Power, Battery and Lithium - unchanged
    // - 12 standard zones: customized
    // -  6 MODBUS zones, customized; remaining MB disabled
    // -  n MODBUS zones, USS Enterprise NCC-1701-E
    // -  2 output zones: named "Output"
    ////////////////////////////////////////////////////////
    zone = IO_MODBUS_OFFSET+12; // MODBUS Zone 13
    sprintf(ZoneDB[zone].name, "Photon magazine 1, fore port");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+13; // MODBUS Zone 14
    sprintf(ZoneDB[zone].name, "Photon magazine 2, fore port");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+14; // MODBUS Zone 15
    sprintf(ZoneDB[zone].name, "Photon magazine 3, fore port");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+15; // MODBUS Zone 16
    sprintf(ZoneDB[zone].name, "Photon magazine 4, fore port");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+16; // MODBUS Zone 17
    sprintf(ZoneDB[zone].name, "Photon magazine 5, fore port");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+17; // MODBUS Zone 18
    sprintf(ZoneDB[zone].name, "Photon magazine 1, fore starboard");
    ZoneDB[zone].value = 5000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = VAL_RANGE_LOW;
    ZoneDB[zone].alarm_range = VAL_RANGE_LOW;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+18; // MODBUS Zone 19
    sprintf(ZoneDB[zone].name, "Photon magazine 2, fore starboard");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+19; // MODBUS Zone 20
    sprintf(ZoneDB[zone].name, "Photon magazine 3, fore starboard");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+20; // MODBUS Zone 21
    sprintf(ZoneDB[zone].name, "Photon magazine 4, fore starboard");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+21; // MODBUS Zone 22
    sprintf(ZoneDB[zone].name, "Photon magazine 5, fore starboard");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+22; // MODBUS Zone 23
    sprintf(ZoneDB[zone].name, "Photon magazine 1, aft");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+23; // MODBUS Zone 24
    sprintf(ZoneDB[zone].name, "Photon magazine 2, aft");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+24; // MODBUS Zone 25
    sprintf(ZoneDB[zone].name, "Quantum magazine 1, fore");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+25; // MODBUS Zone 26
    sprintf(ZoneDB[zone].name, "Quantum magazine 2, fore");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+26; // MODBUS Zone 27
    sprintf(ZoneDB[zone].name, "Quantum magazine 3, fore");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+27; // MODBUS Zone 28
    sprintf(ZoneDB[zone].name, "Quantum magazine 4, fore");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+28; // MODBUS Zone 29
    sprintf(ZoneDB[zone].name, "Quantum magazine 5, fore");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+29; // MODBUS Zone 30
    sprintf(ZoneDB[zone].name, "Quantum magazine, aft");
    ZoneDB[zone].value = 30000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 30000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "torp");
    
    zone = IO_MODBUS_OFFSET+30; // MODBUS Zone 31
    sprintf(ZoneDB[zone].name, "Warp plasma, total");
    ZoneDB[zone].value = 12000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 12500000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+31; // MODBUS Zone 32
    sprintf(ZoneDB[zone].name, "Warp plasma, in use");
    ZoneDB[zone].value = 9500000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 12500000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+32; // MODBUS Zone 33
    sprintf(ZoneDB[zone].name, "Deuterium, port");
    ZoneDB[zone].value = 100000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 120000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+33; // MODBUS Zone 34
    sprintf(ZoneDB[zone].name, "Deuterium, starboard");
    ZoneDB[zone].value = 100000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 120000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+34; // MODBUS Zone 35
    sprintf(ZoneDB[zone].name, "Shields, fore port");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+35; // MODBUS Zone 36
    sprintf(ZoneDB[zone].name, "Shields, fore starboard");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+36; // MODBUS Zone 37
    sprintf(ZoneDB[zone].name, "Shields, port");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+37; // MODBUS Zone 38
    sprintf(ZoneDB[zone].name, "Shields, starboard");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+38; // MODBUS Zone 39
    sprintf(ZoneDB[zone].name, "Shields, upper");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+39; // MODBUS Zone 40
    sprintf(ZoneDB[zone].name, "Shields, lower");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+40; // MODBUS Zone 41
    sprintf(ZoneDB[zone].name, "Shields, aft port");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+41; // MODBUS Zone 42
    sprintf(ZoneDB[zone].name, "Shields, aft starboard");
    ZoneDB[zone].value = 100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 100000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "%");
    
    zone = IO_MODBUS_OFFSET+42; // MODBUS Zone 43
    sprintf(ZoneDB[zone].name, "EPS flow, life support");
    ZoneDB[zone].value = 45800000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 80000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kW/sec");
    
    zone = IO_MODBUS_OFFSET+43; // MODBUS Zone 44
    sprintf(ZoneDB[zone].name, "EPS flow, grav");
    ZoneDB[zone].value = 2400000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 80000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kW/sec");
    
    zone = IO_MODBUS_OFFSET+44; // MODBUS Zone 45
    sprintf(ZoneDB[zone].name, "EPS flow, SIF");
    ZoneDB[zone].value = 76200000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 80000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kW/sec");
    
    zone = IO_MODBUS_OFFSET+45; // MODBUS Zone 46
    sprintf(ZoneDB[zone].name, "EPS flow, shields");
    ZoneDB[zone].value = 8400000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 80000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kW/sec");
    
    zone = IO_MODBUS_OFFSET+46; // MODBUS Zone 47
    sprintf(ZoneDB[zone].name, "EPS flow, deflector");
    ZoneDB[zone].value = 703100;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 80000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kW/sec");
    
    zone = IO_MODBUS_OFFSET+47; // MODBUS Zone 48
    sprintf(ZoneDB[zone].name, "Ship stores, non-replicated");
    ZoneDB[zone].value = 428000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 500000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kg");
    
    zone = IO_MODBUS_OFFSET+48; // MODBUS Zone 49
    sprintf(ZoneDB[zone].name, "Ship stores, undifferentiated");
    ZoneDB[zone].value = 150030000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 200000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "kg");
    
    zone = IO_MODBUS_OFFSET+49; // MODBUS Zone 50
    sprintf(ZoneDB[zone].name, "Shuttlecraft tylium, Galileo");
    ZoneDB[zone].value = 20000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 20000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+50; // MODBUS Zone 51
    sprintf(ZoneDB[zone].name, "Shuttlecraft tylium, Columbus");
    ZoneDB[zone].value = 20000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 20000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+51; // MODBUS Zone 52
    sprintf(ZoneDB[zone].name, "Shuttlecraft tylium, El Baz");
    ZoneDB[zone].value = 20000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 20000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+52; // MODBUS Zone 53
    sprintf(ZoneDB[zone].name, "Shuttlecraft tylium, Picard");
    ZoneDB[zone].value = 20000000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 20000000;
    ZoneDB[zone].alarm_status = 0;
    ZoneDB[zone].alarm_range = 0;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    zone = IO_MODBUS_OFFSET+53; // MODBUS Zone 54
    sprintf(ZoneDB[zone].name, "Shuttlecraft tylium, Green Bug");
    ZoneDB[zone].value = 1100000;
    ZoneDB[zone].minimum = 00;
    ZoneDB[zone].maximum = 20000000;
    ZoneDB[zone].alarm_status = VAL_RANGE_LOW;
    ZoneDB[zone].alarm_range = VAL_RANGE_LOW;
    ZoneDB[zone].alarm_unack = 0;
    ZoneDB[zone].enabled = 1;
    ZoneDB[zone].type = MODBUS_INPUT;
    strcpy(ZoneDB[zone].units, "l");
    
    
    
}
// end zone_db_initialize

////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_zone_values
// Description:  Build the Zone Values JSON, including prefix
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_zone_values(void)
{
    memset (strJSONZoneValues, 0, sizeof(strJSONZoneValues));
    // Header and opening JSON bracket
    strcat(strJSONZoneValues, "DISPLAY ZONE VALUES >>> {");

    // "time":1536694842,
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, "time");
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, ":");
    //// TEST MAB 2022.12.15 strcat(strJSONZoneValues, "1579707275");
    sprintf(lcTempString, "%u", (unsigned int)(g_get_real_time()/1000000));
    strcat(strJSONZoneValues, lcTempString);
    strcat(strJSONZoneValues, ",");
    
    // "value":{"1":8191,"2":100000, ... "81":0},
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, "value");
    strcat(strJSONZoneValues, "\":{");
    for (int zone=0; zone<IO_NUMBER_OF_ZONES; ++zone)
    {
        strcat(strJSONZoneValues, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONZoneValues, lcTempString);
        strcat(strJSONZoneValues, "\":");
        sprintf(lcTempString, "%d", ZoneDB[zone].value);
        strcat(strJSONZoneValues, lcTempString);
        if (zone < IO_NUMBER_OF_ZONES-1)
            strcat(strJSONZoneValues, ",");
    }
    strcat(strJSONZoneValues, "},");
    
    // "min":{"1":8191,"2":100000, ... "79":26310},
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, "min");
    strcat(strJSONZoneValues, "\":{");
    for (int zone=0; zone<IO_NUMBER_OF_INPUTS; ++zone)
    {
        strcat(strJSONZoneValues, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONZoneValues, lcTempString);
        strcat(strJSONZoneValues, "\":");
        sprintf(lcTempString, "%d", ZoneDB[zone].minimum);
        strcat(strJSONZoneValues, lcTempString);
        if (zone < IO_NUMBER_OF_INPUTS-1)
            strcat(strJSONZoneValues, ",");
    }
    strcat(strJSONZoneValues, "},");
    
    // "max":{"1":8191,"2":100000, ... "79":30740},
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, "max");
    strcat(strJSONZoneValues, "\":{");
    for (int zone=0; zone<IO_NUMBER_OF_INPUTS; ++zone)
    {
        strcat(strJSONZoneValues, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONZoneValues, lcTempString);
        strcat(strJSONZoneValues, "\":");
        sprintf(lcTempString, "%d", ZoneDB[zone].maximum);
        strcat(strJSONZoneValues, lcTempString);
        if (zone < IO_NUMBER_OF_INPUTS-1)
            strcat(strJSONZoneValues, ",");
    }
    strcat(strJSONZoneValues, "},");
    
    // "standby":{"state":0,"mode":0}
    strcat(strJSONZoneValues, "\"");
    strcat(strJSONZoneValues, "standby");
    strcat(strJSONZoneValues, "\":{");
    strcat(strJSONZoneValues, "\"state\":0,");
    strcat(strJSONZoneValues, "\"mode\":0");
    strcat(strJSONZoneValues, "}");

    // Closing JSON bracket
    strcat(strJSONZoneValues, "}\r\n");
    
    //g_print("%s\r\n", strJSONZoneValues);
}
// end zone_db_build_zone_values


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_alarm_values
// Description:  Build the Alarm Values JSON, including prefix
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_alarm_values(void)
{
    memset (strJSONAlarmValues, 0, sizeof(strJSONAlarmValues));
    // Header and opening JSON bracket
    strcat(strJSONAlarmValues, "DISPLAY ALARM VALUES >>> {");

    for (int zone=0; zone<IO_NUMBER_OF_ZONES; ++zone)
    {
        // "1":{"status":0,"range":0,"unack":0},
        // ...
        // "81":{"status":0,"range":89,"unack":0}
        strcat(strJSONAlarmValues, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONAlarmValues, lcTempString);
        strcat(strJSONAlarmValues, "\":{");
        
        strcat(strJSONAlarmValues, "\"status\":");
        sprintf(lcTempString, "%d,", ZoneDB[zone].alarm_status);
        strcat(strJSONAlarmValues, lcTempString);
        
        strcat(strJSONAlarmValues, "\"range\":");
        sprintf(lcTempString, "%d,", ZoneDB[zone].alarm_range);
        strcat(strJSONAlarmValues, lcTempString);
        
        strcat(strJSONAlarmValues, "\"unack\":");
        sprintf(lcTempString, "%d", ZoneDB[zone].alarm_unack);
        strcat(strJSONAlarmValues, lcTempString);
        
        strcat(strJSONAlarmValues, "}");
        if (zone < IO_NUMBER_OF_ZONES-1)
            strcat(strJSONAlarmValues, ",");
    }

    // Closing JSON bracket
    strcat(strJSONAlarmValues, "}\r\n");
    
    //g_print("%s\r\n", strJSONAlarmValues);
}
// end zone_db_build_alarm_values


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_zone_names
// Description:  Build the Zone Names JSON, including prefix
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_zone_names(void)
{
    memset (strJSONZoneNames, 0, sizeof(strJSONZoneNames));
    // Header and opening JSON bracket
    strcat(strJSONZoneNames, "DISPLAY ZONE NAMES >>> {");

    for (int zone=0; zone<IO_NUMBER_OF_ZONES; ++zone)
    {
        // "1":{"name":"Power"},
        // ...
        // "81":{"name":"Output 2"}
        strcat(strJSONZoneNames, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONZoneNames, lcTempString);
        strcat(strJSONZoneNames, "\":{");
        
        strcat(strJSONZoneNames, "\"name\":");
        strcat(strJSONZoneNames, "\"");
        sprintf(lcTempString, "%s", ZoneDB[zone].name);
        strcat(strJSONZoneNames, lcTempString);
        strcat(strJSONZoneNames, "\"");
        
        strcat(strJSONZoneNames, "}");
        if (zone < IO_NUMBER_OF_ZONES-1)
            strcat(strJSONZoneNames, ",");
    }

    // Closing JSON bracket
    strcat(strJSONZoneNames, "}\r\n");
    
    //g_print("%s\r\n", strJSONZoneNames);
}
// end zone_db_build_zone_names


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_zone_types
// Description:  Build the Zone Types JSON, including prefix
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_zone_types(void)
{
    memset (strJSONZoneTypes, 0, sizeof(strJSONZoneTypes));
    // Header and opening JSON bracket
    strcat(strJSONZoneTypes, "DISPLAY ZONE TYPES >>> {");

    for (int zone=0; zone<IO_NUMBER_OF_ZONES; ++zone)
    {
        // "1":{"type":13,"units":"V","enabled":1},
        // ...
        // "81:{"type":35,"units":"","enabled":1}
        strcat(strJSONZoneTypes, "\"");
        sprintf(lcTempString, "%d", zone+1);
        strcat(strJSONZoneTypes, lcTempString);
        strcat(strJSONZoneTypes, "\":{");
        
        strcat(strJSONZoneTypes, "\"type\":");
        sprintf(lcTempString, "%d,", ZoneDB[zone].type);
        strcat(strJSONZoneTypes, lcTempString);
        
        strcat(strJSONZoneTypes, "\"units\":");
        strcat(strJSONZoneTypes, "\"");
        sprintf(lcTempString, "%s", ZoneDB[zone].units);
        strcat(strJSONZoneTypes, lcTempString);
        strcat(strJSONZoneTypes, "\",");
        
        strcat(strJSONZoneTypes, "\"enabled\":");
        sprintf(lcTempString, "%d", ZoneDB[zone].enabled);
        strcat(strJSONZoneTypes, lcTempString);
        
        strcat(strJSONZoneTypes, "}");
        if (zone < IO_NUMBER_OF_ZONES-1)
            strcat(strJSONZoneTypes, ",");
    }

    // Closing JSON bracket
    strcat(strJSONZoneTypes, "}\r\n");
    
    //g_print("%s\r\n", strJSONZoneTypes);
}
// end zone_db_build_zone_types


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_diagnostics
// Description:  Build the Diagnostics JSON, including prefix
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_diagnostics(void)
{
    memset (strJSONDiagnostics, 0, sizeof(strJSONDiagnostics));
    // Header and opening JSON bracket
    strcat(strJSONDiagnostics, "DISPLAY DIAGNOSTICS >>> {");
    // IP address
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "ip");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "10.1.2.265");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ",");
    // DNS address
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "dns");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "10.1.2.355");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ",");
    // Gateway address
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "gateway");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "10.1.2.405");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ",");
    // Mask address
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "mask");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "265.265.265.0");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ",");
    // MAC address
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "mac");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "00-07-F9-00-xx-xx");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ",");
    // RMSP version
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, "rmsp");
    strcat(strJSONDiagnostics, "\"");
    strcat(strJSONDiagnostics, ":");
    strcat(strJSONDiagnostics, "\"");
    sprintf(lcTempString, "%s.%s.%s", VERSION_A,VERSION_B,VERSION_C);
    strcat(strJSONDiagnostics, lcTempString);
    strcat(strJSONDiagnostics, "\"");
    // Closing JSON bracket
    strcat(strJSONDiagnostics, "}\r\n");
    
    //g_print("%s\r\n", strJSONDiagnostics);
}
// end zone_db_build_diagnostics


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_build_error_code
// Description:  Build the Connection Error Code message, including prefix
// Parameters:   aucConnectionErrorCode - connection error (blink) code
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_build_error_code(guint8 aucConnectionErrorCode)
{
    memset (strConnectionErrorMsg, 0, sizeof(strConnectionErrorMsg));
    sprintf(strConnectionErrorMsg, 
            "DISPLAY ERROR CODE >>> %d\r\n", 
            aucConnectionErrorCode);
    
    //g_print("%s\r\n", strConnectionErrorMsg);
}
// end zone_db_build_error_code


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_random_walk
// Description:  Randomly adjust zone values
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_random_walk(void)
{
    guint32 lulRandomValue;
    gint8 lcSign;
    guint8 lucAdjustment;
    
    for (int zone = IO_STANDARD_OFFSET; zone < IO_NUMBER_OF_INPUTS; ++zone)
    {
        // Get random sign (+1 or -1)
        lulRandomValue = rand();
        lcSign = lulRandomValue > RAND_MAX/2 ? 1 : -1;
        
        // Get random adjustment
        lucAdjustment = rand() % 100;
        
        // Adjust zone value
        ZoneDB[zone].value += lcSign * lucAdjustment;
    }
}
// end zone_db_random_walk


////////////////////////////////////////////////////////////////////////////
// Name:         zone_db_random_alarms
// Description:  Randomly generate alarms
// Parameters:   None
// Return:       None
////////////////////////////////////////////////////////////////////////////
void
zone_db_random_alarms(void)
{
    guint32 lulRandomValue;
    guint8  lucAlarmStatus, lucAlarmRange, lucAlarmUnack;
    
    for (int zone = IO_STANDARD_OFFSET; zone < IO_NUMBER_OF_INPUTS; ++zone)
    {
        // Get current zone alarms
        lucAlarmStatus = ZoneDB[zone].alarm_status;
        lucAlarmRange  = ZoneDB[zone].alarm_range;
        lucAlarmUnack  = ZoneDB[zone].alarm_unack;
        
        // IF zone is an unacked alarm
        if (lucAlarmUnack != VAL_ALARM_NONE)
        {
            // 75% it gets acked
            lulRandomValue = rand() % 100;
            if (lulRandomValue<75)
                ZoneDB[zone].alarm_unack = VAL_ALARM_NONE;
        }
        // ELSE IF zone is an acked alarm
        else if (lucAlarmStatus != VAL_ALARM_NONE)
        {
            // 75% alarm goes away
            lulRandomValue = rand() % 100;
            if (lulRandomValue<75)
            {
                ZoneDB[zone].alarm_status = VAL_ALARM_NONE;
                ZoneDB[zone].alarm_range  = VAL_ALARM_NONE;
            }
        }
        // ELSE 0.4% zone goes into alarm (50% HI, 50% LO)
        else
        {
            lulRandomValue = rand() % 1000;
            if (lulRandomValue<2)
            {
                ZoneDB[zone].alarm_status = VAL_RANGE_HIGH;
                ZoneDB[zone].alarm_range  = VAL_RANGE_HIGH;
                ZoneDB[zone].alarm_unack  = VAL_RANGE_ALARM;
            }
            else if (lulRandomValue<4)
            {
                ZoneDB[zone].alarm_status = VAL_RANGE_LOW;
                ZoneDB[zone].alarm_range  = VAL_RANGE_LOW;
                ZoneDB[zone].alarm_unack  = VAL_RANGE_ALARM;
            }
        }
    }
}
// end zone_db_random_alarms
