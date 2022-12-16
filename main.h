/* 
 * File:   main.h
 * Author: mbersalona
 *
 * Created on September 12, 2018, 8:32 AM
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////////////


// These are from Windchime (i.e. Sentinel/Sentinel Pro) inputs.h
/*
 * Sentinel Pro
 *
 * 0                1         2 3           14 15           78 79          80
 * /-------------------------------------------------------------------------\
 * | Power | Battery | Lithium | Wired Inputs | Modbus Inputs | Relay Output |
 * |       |         |         |              |---------------|              |
 * |       |         |         |              | IP Alarms     |              |
 * |       |         |         |--------------|---------------|              |
 * |       |         |         |         M2M Zones            |              | 
 * \-------------------------------------------------------------------------/
 * 0                1         2 3                           78 79          80
 *   [1]      [1]        [1]         [12]            [64]           [2]
 */
#define IO_INVALID_INDEX                   (-1)
# define IO_NUMBER_OF_STANDARD_INPUTS      (12)
# define IO_NUMBER_OF_MODBUS_INPUTS        (64)
# define IO_NUMBER_OF_RELAY_OUTPUTS        (2)
# define IO_NUMBER_OF_ANALOG_OUTPUTS       (0)
# define IO_NUMBER_OF_PHYSICAL_OUTPUTS     (IO_NUMBER_OF_RELAY_OUTPUTS + IO_NUMBER_OF_ANALOG_OUTPUTS)
# define IO_EXTERNAL_OFFSET                (3)
# define IO_DIGITAL_OFFSET                 (IO_EXTERNAL_OFFSET)
# define IO_ANALOG_OFFSET                  (0)
# define IO_STANDARD_OFFSET                (IO_EXTERNAL_OFFSET)
# define IO_MODBUS_OFFSET                  (IO_EXTERNAL_OFFSET + IO_NUMBER_OF_STANDARD_INPUTS)
# define IO_RELAY_OFFSET                   (IO_MODBUS_OFFSET + IO_NUMBER_OF_MODBUS_INPUTS)
# define IO_INTERNAL_OFFSET                (0)
# define IO_SOUND_OFFSET                   (IO_INVALID_INDEX)
# define IO_POWER_OFFSET                   (IO_INTERNAL_OFFSET + 0)
# define IO_BATTERY_OFFSET                 (IO_INTERNAL_OFFSET + 1)
# define IO_LITHIUM_OFFSET                 (IO_INTERNAL_OFFSET + 2)
# define IO_NUMBER_OF_EXTERNAL_INPUTS      (IO_NUMBER_OF_STANDARD_INPUTS + IO_NUMBER_OF_MODBUS_INPUTS)
# define IO_NUMBER_OF_INTERNAL_INPUTS      ((IO_SOUND_OFFSET         != IO_INVALID_INDEX) + \
                                            (IO_POWER_OFFSET         != IO_INVALID_INDEX) + \
                                            (IO_BATTERY_OFFSET       != IO_INVALID_INDEX) + \
                                            (IO_LITHIUM_OFFSET       != IO_INVALID_INDEX))

# define IO_EXTERNAL_INPUTS                (IO_NUMBER_OF_EXTERNAL_INPUTS)
# define IO_NUMBER_OF_ANALOG_INPUTS        (IO_NUMBER_OF_EXTERNAL_INPUTS + IO_NUMBER_OF_INTERNAL_INPUTS)
# define IO_NUMBER_OF_DIGITAL_INPUTS       (IO_NUMBER_OF_EXTERNAL_INPUTS)
# define IO_NUMBER_OF_INPUTS               (IO_NUMBER_OF_EXTERNAL_INPUTS + IO_NUMBER_OF_INTERNAL_INPUTS)
# define IO_NUMBER_OF_ZONES                (IO_NUMBER_OF_INPUTS + IO_NUMBER_OF_PHYSICAL_OUTPUTS)
# define NUM_CHANS_DEFAULT                 (12)		/* A/D channels */


//
// From Windchime inputs.h
//

#define ENABLE_28K_THERMISTOR_SUPPORT (1)
#define ENABLE_10K_THERMISTOR_SUPPORT (1)
#define ENABLE_ZERO_TO_FIVE_SUPPORT   (0)
#define ENABLE_IMS_TYPE_WIRED_SENSORS (0)

typedef enum INPUT_TYPE_T{
  NORMALLY_OPEN     =  0,	 // NO                       / IMS NO
  NORMALLY_CLOSED   =  1,	 // NC                       / IMS NC
  #if ENABLE_28K_THERMISTOR_SUPPORT != 0
  TEMP_28K_F        =  2,	 // 2.8k Thermistor ºF       / IMS Temperature F
  TEMP_28K_C        =  3,	 // 2.8k Thermistor ºC       / IMS Temperature C
  #endif
  #if ENABLE_10K_THERMISTOR_SUPPORT != 0
  TEMP_10K_F        =  4,	 // 10k Thermistor ºF        / IMS Temperature F w/ Display
  TEMP_10K_C        =  5,	 // 10k Thermistor ºC        / IMS Temperature C w/ Display
  #endif
  FOUR_TO_TWENTY    =  6,	 // 4-20mA
  #if ENABLE_ZERO_TO_FIVE_SUPPORT != 0
  ZERO2FIVE         =  7,	 // 0-5 volts
  #endif
  #if ENABLE_IMS_TYPE_WIRED_SENSORS != 0 || IO_NUMBER_OF_WIRELESS_INPUTS != 0
  WATER             =  8,	 // Wireless Water           / IMS Water
  MOTION            =  9,	 // Wireless Motion          / IMS IR
  HUMIDITY          = 10,	 // Wireless Humidity        / IMS Humidity
  #endif
  #if IO_NUMBER_OF_WIRELESS_INPUTS != 0
//UNKNOWN_ANALOG    = 11,	 // - Wireless Reserved - ==> Search for "UNKNOWN_ANALOG" to enable
  #endif
  #if IO_BATTERY_OFFSET != IO_INVALID_INDEX
  INTERNAL_BATTERY  = 12,	 // Internal Battery         / IMS *Node* Battery
  #endif
  #if IO_POWER_OFFSET != IO_INVALID_INDEX
  INTERNAL_POWER    = 13,	 // Internal Power           / IMS *Node* Power
  #endif
  #if IO_SOUND_OFFSET != IO_INVALID_INDEX
  INTERNAL_SOUND    = 14, 	 // Internal Sound           / IMS Sound
  #endif
  RESERVED_0A       = 15,  		
  RESERVED_0B       = 16,
  RESERVED_0C       = 17,
  INPUT_UP          = 18,	 // Pull-up Raw ADC
  INPUT_DOWN        = 19,	 // Pull-down Raw ADC
  #if IO_NUMBER_OF_WIRELESS_INPUTS != 0
  EXTERNAL_POWER    = 20,        // Wireless External Power
  #endif
  #if ENABLE_IMS_TYPE_WIRED_SENSORS != 0
  HUMIDITY_DISPLAY  = 21,        // Reserved                 / IMS Humidity w/ Display
  SMOKE             = 22,        // Reserved                 / IMS Smoke Detector
  EXTERNAL_AC_POWER = 23,        // Reserved                 / IMS External AC Power
  #endif
  #if IO_NUMBER_OF_MODBUS_INPUTS != 0
  MODBUS_INPUT      = 24,        // MODBUS Input (Normal [Polled] Data Flow)
  MODBUS_INPUT_REV  = 25,        // MODBUS Input (Reversed [Pushed] Data Flow)
  #endif
  #if IO_NUMBER_OF_IP_ALARMS != 0
  IP_ALARM          = 26,        // IP Alarm
  #endif
  #if ENABLE_PULSE_COUNT_TYPE == true
  PULSE_COUNT_NO    = 27,        // Pulse Count (Normally Open Polarity)
  PULSE_COUNT_NC    = 28,        // Pulse Count (Normally Closed Polarity)
  #endif
  #if ENABLE_RUNTIME_TYPE == true
  RUNTIME_NO        = 29,        // Runtime (Normally Open Polarity)
  RUNTIME_NC        = 30,        // Runtime (Normally Closed Polarity)
  #endif
  RESERVED_1        = 31,
  RESERVED_2        = 32,
  RESERVED_3        = 33,
  RESERVED_4        = 34,  
  #if IO_NUMBER_OF_RELAY_OUTPUTS != 0 || IO_NUMBER_OF_M2M_OUTPUTS != 0
  RELAY_OUTPUT      = 35,        // Relay Output (used for control applications)
  #endif
  #if IO_NUMBER_OF_ANALOG_OUTPUTS != 0 || IO_NUMBER_OF_M2M_OUTPUTS != 0
  ANALOG_OUTPUT     = 36,        // Analog Output (e.g. 4-20mA output)
  #endif
  #if IO_NUMBER_OF_M2M_OUTPUTS != 0
  M2M               = 37,        // Machine-To-Machine (Normal [Polled] Data Flow)
  M2M_REV           = 38,        // Machine-To-Machine (Reversed [Pushed] Data Flow)
  #endif
  INPUT_NONE        = 39,  	 // Disabled Input -> MUST BE THE LAST NUMBER FOR SANITY CHECKS TO WORK
                                 //                -> MUST UPDATE inputs.c IF THIS CHANGES
                                 //                -> MUST UPDATE http.c IF THIS IS GREATER THAN AN 8-BIT VALUE
                                 //                -> MUST UPDATE video environmental monitor (input_monitor)
                                 //                -> MUST UPDATE MSP430 build to change version
} INPUT_TYPE_T;


// Alarms, from Windchime inputs.h
#define VAL_ALARM_NONE       0x00
#define MSK_ALARM_LOW        0x01
#define MSK_ALARM_HIGH       0x02
#define MSK_ALARM_BATTERY    0x04 // Wireless
#define MSK_ALARM_RESPONSE   0x08 // IMS and Wireless
#define VAL_RANGE_OK                  0x00
#define VAL_RANGE_OFF                 0x00 // Outputs: Digital, Analog, M2M
#define VAL_RANGE_ALARM			          0x01
#define VAL_RANGE_CLOSED              0x01
#define VAL_RANGE_LOW                 0x01
#define VAL_RANGE_ON                  0x01 // Outputs: Digital, Analog, M2M
#define VAL_RANGE_OPEN                0x02
#define VAL_RANGE_HIGH                0x02
#define VAL_RANGE_CYCLE               0x02 // Outputs: Digital, Analog, M2M
#define VAL_RANGE_CONNECTION_PENDING  0x03 // Wireless TODO convert to bitmask-compatible value, e.g. 0x08
#define VAL_RANGE_NOT_RESPONDING      0x04 // IMS and Wireless
#define VAL_RANGE_ROUTE_DOWN          0x05 // IP       TODO convert to bitmask-compatible value, e.g. 0x10



// Standby mode
#define STANDBY_MODE_OFF          (0)
#define STANDBY_MODE_CONFIG       (1)
#define STANDBY_MODE_NO_COUNTDOWN (2)

// Standby state
#define STANDBY_STATE_NO_COUNTDOWN (0xFFFF)


/* Online secure connection errors - from mss_ssl_client.h */
typedef enum
{
  ONLINE_CONNECTION_OK = 0,                                     // 00 = secure connection established
  
  // from rtcs_init.c, rtcs_init()
  ONLINE_CONNECTION_ERROR_RTCS_INITIALIZE_IN_PROGRESS,          // 01 = initialize error code to this value before rtcs_init() starts
  ONLINE_CONNECTION_ERROR_RTCS_INITIALIZE_FAILED,               // 02
  ONLINE_CONNECTION_ERROR_ETHERNET_DEVICE_INITIALIZE_FAILED,    // 03
  ONLINE_CONNECTION_ERROR_ETHERNET_CABLE_FAILED,                // 04
  ONLINE_CONNECTION_ERROR_DHCP_FAILED,                          // 05
  
  // from mss_ssl_client.c, mss_ssl_client()
  ONLINE_CONNECTION_ERROR_SECURE_NEGOTIATION_IN_PROGRESS,       // 06 = rtcs_init() completed OK, starting secure negotiation
  ONLINE_CONNECTION_ERROR_DNS_LINK_FAILED,                      // 07
  ONLINE_CONNECTION_ERROR_IP_ADDRESS_FAILED,                    // 08
  ONLINE_CONNECTION_ERROR_TCP_SOCKET_FAILED,                    // 09
  ONLINE_CONNECTION_ERROR_SSL_CONNECT_FAILED,                   // 10
  ONLINE_CONNECTION_ERROR_SSL_HANDSHAKE_FAILED,                 // 11
  
  // server dropped the secure connection
  ONLINE_CONNECTION_ERROR_SERVER_DROPPED_CONNECTION,            // 12 = generic "server dropped connection" error
  ONLINE_CONNECTION_ERROR_SSL_INVALID_MAC,                      // 13 = merror.h error -7509 ERR_SSL_INVALID_MAC
  ONLINE_CONNECTION_ERROR_TCP_READ_ERROR,												// 14 = merror.h error -5902 ERR_TCP_READ_ERROR
  
  // other connection problems
  ONLINE_CONNECTION_ERROR_ETHERNET_CABLE_DISCONNECTED,          // 15
  ONLINE_CONNECTION_ERROR_DISCONNECT_ON_NEW_ALARM,              // 16
  ONLINE_CONNECTION_ERROR_TCP_WRITE_ERROR,										  // 17 = merrorS.h error -5905 ERR_TCP_WRITE_ERROR
  ONLINE_CONNECTION_ERROR_TCP_SOCKET_CLOSED,                    // 18 = merrors.h error -5901 ERR_TCP_SOCKET_CLOSED
  ONLINE_CONNECTION_ERROR_HTTP_PARSE_ERROR,                     // 19 = HTTP JSON parse error
          
  ONLINE_CONNECTION_ERROR_COUNT
} eMssSslOnlineErrors;

///////////////////////////////////////////////////////////////////////////////
//
// Public variables
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Public routines
//
///////////////////////////////////////////////////////////////////////////////




#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */

