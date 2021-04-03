// COPY THIS FILE AND RENAME IT TO "config.h" AND ENTER ALL YOUR CONFIGURATION DATA BELOW

#ifndef CONFIG_H
#define CONFIG_H

#define NTP_SERVER      "pool.ntp.org"
#define NTP_INTERVAL    60000           // In milliseconds
#define AP_NAME         "NixieAP"       // In milliseconds
#define CLOCK_SERIAL    9600            // Baud
#define DEBUG_SERIAL    115200          // Baud

// Configure your timezone rules here; how to is described at https://github.com/JChristensen/Timezone
#define TZ_DST          { "CEST", Last, Sun, Mar, 2, +120 } // Daylight time = UTC + 2 hours
#define TZ_STD          { "CET", Last, Sun, Oct, 2, +60 }   // Standard time = UTC + 1 hours

#endif