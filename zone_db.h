/* 
 * File:   zone_db.h
 * Author: mbersalona
 *
 * Created on September 12, 2018, 8:32 AM
 * RMSP Simulator zone database
 */

#ifndef ZONE_DB_H
#define ZONE_DB_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define MAX_NAME_LENGTH (80)
#define MAX_UNIT_LENGTH (10)
typedef struct
{
    char    name[MAX_NAME_LENGTH];
    gint32 value;
    gint32 minimum;
    gint32 maximum;
    guint8  alarm_status;
    guint8  alarm_range;
    guint8  alarm_unack;
    guint8  type;
    char    units[MAX_UNIT_LENGTH];
    guint8  enabled;
} Zone_record;

///////////////////////////////////////////////////////////////////////////////
//
// Public variables
//
///////////////////////////////////////////////////////////////////////////////
extern char strJSONZoneValues[];
extern char strJSONAlarmValues[];
extern char strJSONZoneNames[];
extern char strJSONZoneTypes[];
extern char strJSONDiagnostics[];
extern char strConnectionErrorMsg[];

///////////////////////////////////////////////////////////////////////////////
//
// Public routines
//
///////////////////////////////////////////////////////////////////////////////

void zone_db_initialize(void);
void zone_db_build_zone_values(void);
void zone_db_build_alarm_values(void);
void zone_db_build_zone_names(void);
void zone_db_build_zone_types(void);
void zone_db_build_diagnostics(void);
void zone_db_build_error_code(guint8 aucConnectionErrorCode);
void zone_db_random_walk(void);
void zone_db_random_alarms(void);





#ifdef __cplusplus
}
#endif

#endif /* ZONE_DB_H */

