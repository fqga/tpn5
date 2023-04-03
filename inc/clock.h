#include <stdint.h>
#include <stdbool.h>

typedef struct clock_s * clock_t;

//clock_event_t es un puntero a una función que devuelve void y recibe un clock y un state 
typedef void(*clock_event_t)(clock_t clock, bool state); //Puntero a función para notificacion de eventos de reloj


clock_t ClockCreate(uint16_t ticks_per_second, clock_event_t event_handler);

bool ClockGetTime(clock_t clock, uint8_t *time, uint8_t size);

void ClockSetupTime(clock_t clock, uint8_t const * const time, uint8_t size);

void ClockNewTick(clock_t clock);

void ClockSetupAlarm(clock_t clock, uint8_t const * const alarm, uint8_t size);

bool ClockGetAlarm(clock_t clock, uint8_t * alarm, uint8_t size);

bool ClockToggleAlarm(clock_t clock);