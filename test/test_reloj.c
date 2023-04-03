#include "unity.h"
#include "clock.h"

#define TICKS_PER_SECOND 5

clock_t reloj;
bool alarm_state;


void SimulateSeconds( int seconds){
        for (int i = 0; i < seconds * TICKS_PER_SECOND; i++){
        ClockNewTick(reloj);
    }
}

void AlarmEventHandler(clock_t clock, bool state){
    alarm_state = state;
}



/**
 * @brief Funcion que Ceedling ejecuta antes de realizar cada una de las pruebas (Porque se llama setUp)
 * 
 */
void setUp(void){
    static const uint8_t INICIAL[] = {1,2,3,4};
    reloj = ClockCreate(TICKS_PER_SECOND,AlarmEventHandler);
    ClockSetupTime(reloj, INICIAL, sizeof(INICIAL));
    alarm_state = false;
}


// Configurar la libreria, consultar la hora tiene que ser invalida.
void test_start_up(void){
    static const uint8_t ESPERADO[] = {0,0,0,0,0,0};
    uint8_t hora[6];
    uint8_t alarma[4];
    reloj = ClockCreate(TICKS_PER_SECOND, AlarmEventHandler);
    TEST_ASSERT_FALSE( ClockGetTime(reloj, hora, sizeof(hora)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY( ESPERADO, hora, sizeof(ESPERADO));
    TEST_ASSERT_FALSE( ClockGetAlarm(reloj, alarma, sizeof(alarma)));
}


// Configurar la libreria, ajustar la hora (con valores correctos), consultar la hora y tiene que ser valida.
void test_set_up_current_time(void){
    static const uint8_t ESPERADO[] = {1,2,3,4,0,0};
    uint8_t hora[6];
    TEST_ASSERT_TRUE( ClockGetTime(reloj, hora, sizeof(hora)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY( ESPERADO, hora, sizeof(ESPERADO));
}

//Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo un segundo
void test_one_second_elapsed(void){
    static const uint8_t ESPERADO[] = {1,2,3,4,0,1};
    uint8_t hora[6];

    SimulateSeconds(1);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}

// Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo diez segundos 
void test_ten_second_elapsed(void){
    static const uint8_t ESPERADO[] = {1,2,3,4,1,0};
    uint8_t hora[6];
    
    SimulateSeconds(10);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}

// Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo 1 minuto 
void test_one_minute_elapsed(void){
    static const uint8_t ESPERADO[] = {1,2,3,5,0,0};
    uint8_t hora[6];
    
    SimulateSeconds(60);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}

// Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo 10 minutos 
void test_ten_minutes_elapsed(void){
    static const uint8_t ESPERADO[] = {1,2,4,4,0,0};
    uint8_t hora[6];
    
    SimulateSeconds(10 * 60);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}


// Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo 1 hora 
void test_one_hour_elapsed(void){
    static const uint8_t ESPERADO[] = {1,3,3,4,0,0};
    uint8_t hora[6];
    
    SimulateSeconds(60 * 60);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}

// Simular el paso de n ciclos de reloj, consultar la hora y verificar que avanzo 10 horas 
void test_ten_hours_elapsed(void){
    static const uint8_t ESPERADO[] = {2,2,3,4,0,0};
    uint8_t hora[6];
    
    SimulateSeconds(10 * 60 * 60);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}


// Simular el paso de n ciclos de reloj, consultar la hora y verificar se llego a las 24.00.00
void test_one_day_elapsed(void){
    static const uint8_t ESPERADO[] = {1,2,3,4,0,0};
    uint8_t hora[6];
    
    SimulateSeconds((24 * 60) * 60);
    ClockGetTime(reloj, hora, sizeof(hora));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ESPERADO, hora, sizeof(ESPERADO));
}


//Configurar la hora de la alarmar (con valores correctos), revisar si la guarda y si queda activada.
void test_setup_and_get_alarm(void) {
    static const uint8_t ALARMA[] = {1,2,3,5};
    uint8_t hora[4];

    ClockSetupAlarm(reloj, ALARMA, sizeof(ALARMA));
    TEST_ASSERT_TRUE(ClockGetAlarm(reloj, hora, sizeof(hora)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ALARMA, hora, sizeof(ALARMA));    
}


// Si la alarma esta activa y la desactivo, queda desactivada pero no cambia la hora. 
void test_setup_and_disabled_alarm(void) {
    static const uint8_t ALARMA[] = {1,2,3,5};
    uint8_t hora[4];

    ClockSetupAlarm(reloj, ALARMA, sizeof(ALARMA));
    TEST_ASSERT_FALSE(ClockToggleAlarm(reloj));
    TEST_ASSERT_FALSE(ClockGetAlarm(reloj, hora, sizeof(hora)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ALARMA, hora, sizeof(ALARMA));    
}

// Si la alarma esta activa, y la hora del reloj coincide con la hora de la alarma, entonces suena.
void test_setup_and_fired_alarm(void) {
    static const uint8_t ALARMA[] = {1,2,3,5};

    ClockSetupAlarm(reloj, ALARMA, sizeof(ALARMA));
    SimulateSeconds(60);
    TEST_ASSERT_TRUE(alarm_state);
    alarm_state = false;
    SimulateSeconds(10);
    TEST_ASSERT_FALSE(alarm_state);

}

// Si se desactiva la alarma y pasan 60 segundos no suena
void test_setup_and_not_fired_alarm(void) {
    static const uint8_t ALARMA[] = {1,2,3,5};

    ClockSetupAlarm(reloj, ALARMA, sizeof(ALARMA));
    ClockToggleAlarm(reloj);

    SimulateSeconds(60);
    TEST_ASSERT_FALSE(alarm_state);
}

// Si la alarma esta activa, y pasa un dia, entonces vuelve a sonar.
void test_setup_and_fired_alarm_onedayelapsed(void) {
    static const uint8_t ALARMA[] = {1,2,3,5};

    ClockSetupAlarm(reloj, ALARMA, sizeof(ALARMA));
    SimulateSeconds(60);
    TEST_ASSERT_TRUE(alarm_state);
    alarm_state = false;
    SimulateSeconds((23 * 60)*60 + 58);
    TEST_ASSERT_FALSE(alarm_state);
    SimulateSeconds((23 * 60)*60 + 62);
    TEST_ASSERT_TRUE(alarm_state);

    

}

