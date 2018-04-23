  #include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#include "fsm.h"
#define PERIOD_TICK 100/portTICK_RATE_MS


//Constantes: temperaturas umbrales de las trampillas y tiempo de timeout
#define th1 20
#define th2 40

#define th1_carga 40
#define th2_carga 70
#define th_out 20
#define th_in1 20
#define th_in2 40

#define tiempo_de_espera 10000/portTICK_RATE_MS

//Variables: temperaturas de las trampillas (izquierda t1 y derecha t2), temperatura en el exterior (temp_fuera), temperatura en el interior (temp_in), carga de t inicio del timeout
int t1;
int t2;

int temp_fuera;
int temp_in;
int carga;

portTickType start_timeout_temperatura;

//Declaración de las funciones de entrada
int frio_frio (fsm_t *this);
int caliente_caliente (fsm_t *this);
int frio_caliente (fsm_t *this);
int caliente_frio (fsm_t *this);

int sin_presion (fsm_t *this);
int presion_baja (fsm_t *this);
int presion_alta (fsm_t *this);

int timeout (fsm_t *this);
//

//Declaración de las funciones de salida
void get_entradas (fsm_t *this);
void abrir_dos (fsm_t *this);
void cerrar_dos (fsm_t *this);
void cerrar_izquierda (fsm_t *this);
void cerrar_derecha (fsm_t *this);
void abrir_izquierda (fsm_t *this);
void abrir_derecha (fsm_t *this);
void switch_abrir_derecha (fsm_t *this);
void switch_abrir_izquierda (fsm_t *this);

void get_tyc (fsm_t *this);
void POT_2 (fsm_t *this);
void POT_0 (fsm_t *this);
void POT_1 (fsm_t *this);


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/

uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

//Estados de las maquinas de estados
enum estados_de_trampillas {
  DOS_CERRADAS,
  DOS_ABIERTAS,
  IZQUIERDA_ABIERTA,
  DERECHA_ABIERTA,
};

enum estados_de_presion {
  P2,
  P1,
  P0,
};

enum estados_de_temperatura {
  LEYENDO,
};

//Funciones de entrada
//Ejemplo frio_caliente: la trampilla izquierda esta fria y la trampilla derecha esta caliente
//Ejemplo caliente_frio: la trampilla izquierda esta caliente y la trampilla derecha esta fria

int frio_frio (fsm_t *this) {
  return (t1<th1 && t2<th2);
}
int caliente_caliente (fsm_t *this) {
  return (t1>th1 && t2>th2);
}
int frio_caliente (fsm_t *this) {
  return (t1<th1 && t2>th2);
}
int caliente_frio (fsm_t *this) {
  return (t1>th1 && t2<th2);
}
int timeout (fsm_t *this) {
  return ( xTaskGetTickCount () > (start_timeout_temperatura+tiempo_de_espera) );
}

//Funciones de entrada de presión

int sin_presion (fsm_t *this) {
  return (temp_fuera<th_out && (temp_in<th_in1||carga<th1_carga));
}
int presion_alta (fsm_t *this) {
  return (temp_fuera>th_out && (temp_in>th_in2 && carga>th2_carga));
}
int presion_baja (fsm_t *this) {
  return ((temp_fuera>th_out) && (((temp_in>th_in1) && (temp_in<=th_in2)) || ((carga>th1_carga) && (carga<=th2_carga))));
}
//Funciones de salida

void get_entradas (fsm_t *this) {
  //iniciamos otra vez el timer
  start_timeout_temperatura = xTaskGetTickCount ();
  //generamos numeros aleatorios ya que no tenemos sensores
  t1 = (rand() % 80) + 1;
  t2 = (rand() % 80) + 1;
  temp_in = (rand() % 80) + 1;
  temp_fuera = (rand() % 80) + 1;
  carga = (rand() % 100) +1;
  printf("La temperatura 1 es: %d\n", t1);
  printf("La temperatura 2 es: %d\n", t2);
  printf("La temperatura dentro es: %d\n", temp_in);
  printf("La temperatura fuera es: %d\n", temp_fuera);
  printf("La carga es: %d\n", carga);
}
void abrir_dos (fsm_t *this) {
  //Aquí irá la función para abrir las dos trampillas
  printf("Abrimos las dos\n");
}

void abrir_izquierda (fsm_t *this) {
  //Aquí irá la función para abrir la trampilla izquierda
  printf("Abrimos izquierda\n");
}

void abrir_derecha (fsm_t *this) {
  //Aquí irá la función para cerrar la trampilla derecha
  printf("Abrimos derecha\n");
}

void cerrar_dos (fsm_t *this) {
  //Aquí irá la función para cerrar las dos trampillas
  printf("Cerramos las dos\n");
}

void cerrar_izquierda (fsm_t *this) {
  //Aquí irá la función para cerrar la trampilla izquierda
  printf("Cerramos las izquierda\n");
}

void cerrar_derecha (fsm_t *this) {
  //Aquí irá la función para cerrar la trampilla derecha
  printf("Cerramos la derecha\n");
}

void switch_abrir_izquierda (fsm_t *this) {
  //Aquí irá la función para abrir la trampilla izquierda y cerrar la derecha
  printf("Abrimos izquierda y cerramos derecha\n");
}

void switch_abrir_derecha (fsm_t *this) {
  //Aquí irá la función para cerrar la trampilla derecha y cerrar la izquierda
  printf("Abrimos derecha y cerramos la izquierda\n");
}

void POT_2 (fsm_t *this) {
  //Aquí irá la función para potencia al maximo
  printf("Aumentamos la presión del aire al máximo\n");
}

void POT_1 (fsm_t *this) {
  //Aquí irá la función para poner potencia media
  printf("Fijamos una presión media\n");
}

void POT_0 (fsm_t *this) {
  //Aquí irá la función para poner potencia al minimo
  printf("Disminuimos la presión al mínimo\n");
}

//Funcion principal del sistema: "main"

void main_del_sistema(void* ignore)
{
  //Tabla de transiciones de trampillas
 static fsm_trans_t trampillas[] = {
   //Transiciones del estado: DOS_CERRADAS
  { DOS_CERRADAS, caliente_caliente, DOS_ABIERTAS, abrir_dos },
  { DOS_CERRADAS, frio_caliente, DERECHA_ABIERTA, abrir_derecha },
  { DOS_CERRADAS, caliente_frio, IZQUIERDA_ABIERTA, abrir_izquierda },
   //Transiciones del estado: DOS_ABIERTAS
   { DOS_ABIERTAS, frio_frio, DOS_CERRADAS, cerrar_dos },
   { DOS_ABIERTAS, frio_caliente, DERECHA_ABIERTA, cerrar_izquierda },
   { DOS_ABIERTAS, caliente_frio, IZQUIERDA_ABIERTA, cerrar_derecha },
   //Transiciones del estado: DERECHA_ABIERTA
   { DERECHA_ABIERTA, frio_frio, DOS_CERRADAS, cerrar_derecha },
   { DERECHA_ABIERTA, caliente_caliente, DOS_ABIERTAS, abrir_izquierda },
   { DERECHA_ABIERTA, caliente_frio, IZQUIERDA_ABIERTA, switch_abrir_izquierda },
   //Transiciones del estado: IZQUIERDA_ABIERTA
   { IZQUIERDA_ABIERTA, frio_frio, DOS_CERRADAS, cerrar_izquierda },
   { IZQUIERDA_ABIERTA, caliente_caliente, DOS_ABIERTAS, abrir_derecha },
   { IZQUIERDA_ABIERTA, frio_caliente, DERECHA_ABIERTA, switch_abrir_derecha },

   {-1, NULL, -1, NULL },
 };

 //Tabla de transiciones de presion
static fsm_trans_t presion[] = {
  //Transiciones del estado: P2
  { P2, sin_presion, P0, POT_0 },
  { P2, presion_baja, P1, POT_1 },
  //Transiciones del estado: P0
  { P0, presion_baja, P1, POT_1 },
  { P0, presion_alta, P2, POT_2 },
  //Transiciones del estado: P2
  { P1, sin_presion, P0, POT_0 },
  { P1, presion_alta, P2, POT_2 },

  {-1, NULL, -1, NULL },
};

 //Tabla de transiciones de lecturas
 static fsm_trans_t entradas[] = {
  { LEYENDO, timeout, LEYENDO, get_entradas },
  {-1, NULL, -1, NULL },
 };

//Iniciamos fsm de trampillas
 fsm_t* fsm_trampillas = fsm_new(trampillas);
 cerrar_dos(fsm_trampillas);

 //Iniciamos fsm de presion
 fsm_t* fsm_presion = fsm_new(presion);
 sin_presion(fsm_presion);

 //Iniciamos fsm de las entradas
 fsm_t* fsm_entradas = fsm_new(entradas);

//tiempo de reposo de las maquinas de estado y generador de numeros aleatorios
 portTickType xLastWakeTime;
 srand ( time(NULL) );

//Bucle para las transiciones
 while(true) {
 xLastWakeTime = xTaskGetTickCount ();
 fsm_fire(fsm_entradas);
 fsm_fire(fsm_presion);
 fsm_fire(fsm_trampillas);
 vTaskDelayUntil(&xLastWakeTime, PERIOD_TICK);
 }
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    xTaskCreate(&main_del_sistema, "startup", 2048, NULL, 1, NULL);
}
