#include <string.h>
#include "clock.h"

#define START_VALUE 0

#define TIME_SIZE 6

#define ALARM_SIZE 4

#define SECONDS_UNITS 5

#define SECONDS_TENS 4

#define MINUTE_UNITS 3

#define MINUTE_TENS 2

#define HOURS_UNITS 1

#define HOURS_TENS 0

#define MAX_HOURS_UNITS_VALUE 4

#define MAX_HOURS_TENS_VALUE 2

struct clock_s{
    bool valid;
    bool enabled;
    uint16_t ticks_per_second;
    uint16_t ticks_count;
    uint8_t time[TIME_SIZE];
    uint8_t alarm[ALARM_SIZE];
    clock_event_t event_handler;
};

static struct clock_s instances;

clock_t ClockCreate( uint16_t ticks_per_second, clock_event_t event_handler){
    instances.valid = false;
    instances.enabled = false;
    instances.event_handler = event_handler;
    instances.ticks_count = START_VALUE;
    instances.ticks_per_second = ticks_per_second;
    memset(instances.time, START_VALUE, TIME_SIZE);
    return &instances;
}

bool ClockGetTime( clock_t clock, uint8_t * time, uint8_t size){
    memcpy( time, clock->time, size);
    return clock->valid;
}

void ClockSetupTime(clock_t clock, uint8_t const * const time, uint8_t size){
    memcpy( clock->time, time, size);
    clock->valid = true;
}

void ClockNewTick(clock_t clock){
    clock->ticks_count++;
    if (clock->ticks_count == clock->ticks_per_second){
        clock->ticks_count = START_VALUE;
        clock->time[SECONDS_UNITS]++;
        if (clock->time[SECONDS_UNITS] == 10){
            clock->time[SECONDS_UNITS] = 0;
            clock->time[SECONDS_TENS]++;
            if (clock->time[SECONDS_TENS] == 6){
                clock->time[SECONDS_TENS] = 0;
                clock->time[MINUTE_UNITS]++;
                if (clock->time[MINUTE_UNITS] == 10){
                    clock->time[MINUTE_UNITS] = 0;
                    clock->time[MINUTE_TENS]++;
                    if (clock->time[MINUTE_TENS] == 6){
                        clock->time[MINUTE_TENS] = 0;
                        clock->time[HOURS_UNITS]++;
                        if (clock->time[HOURS_UNITS] == 10){
                            clock->time[HOURS_UNITS] = 0;
                            clock->time[HOURS_TENS]++;
                        }
                    }
                }
            }
        }

        bool activate = (clock->time[SECONDS_TENS] == 0) && (clock->time[SECONDS_UNITS] == 0);

        for(int index = 0; index < ALARM_SIZE; index++){
            if(clock->alarm[index] != clock->time[index]){
                activate = false;
                break;
            }
        }

        if(activate && clock->enabled){ 
            clock->event_handler(clock,true); 
        }

        if(clock->time[HOURS_TENS] == MAX_HOURS_TENS_VALUE){
            if(clock->time[HOURS_UNITS] == MAX_HOURS_UNITS_VALUE){
                clock->time[HOURS_TENS] = START_VALUE;
                clock->time[HOURS_UNITS] = START_VALUE;
            }
        }
    }
}

void ClockSetupAlarm(clock_t clock, uint8_t const * const alarm, uint8_t size){
    memcpy(clock->alarm, alarm, size);
    clock->enabled = true;
}

bool ClockGetAlarm(clock_t clock, uint8_t * alarm, uint8_t size){
    memcpy(alarm, clock->alarm, size);
    return clock->enabled;
}

bool ClockToggleAlarm(clock_t clock){
    clock->enabled = !clock->enabled;
    return clock->enabled;
}
