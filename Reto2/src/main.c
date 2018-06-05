#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#include "fsm.h"
#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS
#define TIMEOUT_PANTALLA_TICK 100/portTICK_RATE_MS
#define TIMEOUT_PELOTA_TICK 400/portTICK_RATE_MS
#define ETS_GPIO_INTR_ENABLE() \
_xt_isr_unmask(1 << ETS_GPIO_INUM)
#define ETS_GPIO_INTR_DISABLE() \
_xt_isr_mask(1 << ETS_GPIO_INUM)
int button_pressed_D3 (fsm_t *this);
int button_pressed_D8 (fsm_t *this);
void raqueta_izquierda (fsm_t *this);
void raqueta_derecha (fsm_t *this);
void refresco_pantalla (fsm_t *this);

portTickType start_timeout_pantalla;
portTickType start_timeout_pelota;

// FLAGS botones
int done0=0;
int done15=0;

// Matriz PANTALLA
int matriz[20][20];

// Direcciones PELOTA
int eje_y=0;
int eje_x=1;

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


/***************************************
 ***************************************
              enums FSMs
 ***************************************
 ***************************************
*/
enum fsm_pantalla {
  PANTALLA
};

enum fsm_raqueta {
  RAQUETA
};

enum fsm_pelota {
  PELOTA
};


/***************************************
 ***************************************
          Funciones RAQUETA
 ***************************************
 ***************************************
*/
int button_pressed_D3 (fsm_t *this) {
   return (done0);
}
int button_pressed_D8 (fsm_t *this) {
 return (done15);
}
void raqueta_izquierda (fsm_t *this) {
  done15=0;
  //MOVER RAQUETA A LA IZQUIERDA
}
void raqueta_derecha (fsm_t *this) {
  done0=0;
  //MOVER RAQUETA A LA DERECHA
}
void isr_gpio(void* arg) {
 static portTickType xLastISRTick0 = 0;
 uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
 portTickType now = xTaskGetTickCount ();
 //printf("ENTRAMOS INTERRUPCION\n");
 if (status & BIT(0)) {
   if (now > xLastISRTick0) {
     //printf("INTERRUPCION 0\n");
    xLastISRTick0 = now + REBOUND_TICK;
    done0 = 1;
  }
  //xLastISRTick0 = now + REBOUND_TICK;
  }
  static portTickType xLastISRTick15 = 0;
  if (status & BIT(15)) {
    if (now > xLastISRTick15) {
     xLastISRTick15 = now + REBOUND_TICK;
     //printf("INTERRUPCION 15\n");
     done15 = 1;
   }
   //xLastISRTick0 = now + REBOUND_TICK;
   }
   GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,	status);
}


/***************************************
 ***************************************
          Funciones PANTALLA
 ***************************************
 ***************************************
*/
int timeout_pantalla (fsm_t *this) {
  long diff_t = xTaskGetTickCount () - start_timeout_pantalla;
  return (TIMEOUT_PANTALLA_TICK < diff_t);
}
void refresco_pantalla (fsm_t *this) {
  start_timeout_pantalla= xTaskGetTickCount ();
  for(int i=0; i<20; i++){
    for(int j=0; j<20; j++){
      printf("%d", matriz[i][j]);
    }
      printf("\n");
  }
}


/***************************************
 ***************************************
          Funciones PELOTA
 ***************************************
 ***************************************
*/
int timeout_pelota (fsm_t *this) {
  long diff_t = xTaskGetTickCount () - start_timeout_pelota;
  return (TIMEOUT_PELOTA_TICK < diff_t);
}
int pelota_choca_arriba (fsm_t *this) {
  // Comprueba si en la siguiente posición hay un obstaculo
  return 0;
}
int pelota_choca_abajo (fsm_t *this) {
  // Comprueba si en la siguiente posición hay un obstaculo
  return 0;
}
int pelota_choca_derecha (fsm_t *this) {
  // Comprueba si en la siguiente posición hay un obstaculo
  return 0;
}
int pelota_choca_izquierda (fsm_t *this) {
  // Comprueba si en la siguiente posición hay un obstaculo
  return 0;
}
void pelota_cambia_abajo (fsm_t *this){
  eje_y=-1;
}
void pelota_cambia_arriba (fsm_t *this){
  eje_y=1;
}
void pelota_cambia_derecha (fsm_t *this){
  eje_x=1;
}
void pelota_cambia_izquierda (fsm_t *this){
  eje_x=-1;
}
void mover_pelota (fsm_t *this) {
  start_timeout_pelota= xTaskGetTickCount ();
  if (eje_y==1 && eje_x==0){ // ARRIBA

  }else if(eje_y==1 && eje_x==1){ // ARRIBA DERECHA

  }else if(eje_y==0 && eje_x==1){ // DERECHA

  }else if(eje_y==-1 && eje_x==1){ // ABAJO DERECHA

  }else if(eje_y==-1 && eje_x==0){ // ABAJO

  }else if(eje_y==-1 && eje_x==-1){ // ABAJO IZQUIERDA

  }else if(eje_y==0 && eje_x==-1){ // IZQUIERDA

  }else if(eje_y==1 && eje_x==-1){ // ARRIBA IZQUIERDA

  }
}


/***************************************
 ***************************************
                SET UP
 ***************************************
 ***************************************
*/
void inter(void* ignore)
{
 static fsm_trans_t pantalla[] = {
  { PANTALLA, timeout_pantalla, PANTALLA, refresco_pantalla },
  {-1, NULL, -1, NULL },
 };

 static fsm_trans_t raqueta[] = {
  { RAQUETA, button_pressed_D8, RAQUETA, raqueta_izquierda },
  { RAQUETA, button_pressed_D3, RAQUETA, raqueta_derecha },
  {-1, NULL, -1, NULL },
 };

 static fsm_trans_t pelota[] = {
  { PELOTA, timeout_pelota, PELOTA, mover_pelota },
  { PELOTA, pelota_choca_abajo, PELOTA, pelota_cambia_arriba },
  { PELOTA, pelota_choca_arriba, PELOTA, pelota_cambia_abajo },
  { PELOTA, pelota_choca_izquierda, PELOTA, pelota_cambia_derecha },
  { PELOTA, pelota_choca_derecha, PELOTA, pelota_cambia_izquierda },
  {-1, NULL, -1, NULL },
 };

 PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
 gpio_intr_handler_register((void*)isr_gpio, NULL);
 gpio_pin_intr_state_set(0, GPIO_PIN_INTR_NEGEDGE);
 gpio_pin_intr_state_set(15, GPIO_PIN_INTR_POSEDGE);
 ETS_GPIO_INTR_ENABLE();

 fsm_t* fsm_pantalla = fsm_new(pantalla);
 fsm_t* fsm_raqueta = fsm_new(raqueta);
 fsm_t* fsm_pelota = fsm_new(pelota);
 portTickType xLastWakeTime;


 // Inicializar PANTALLA
 start_timeout_pantalla = xTaskGetTickCount ();
 for(int i=0; i<20; i++){
   for(int j=0; j<20; j++){
     matriz[i][j]=0;
   }
 }

 // Inicialzar RAQUETA


 // Inicializar PELOTA


 // LOOP infinito
 while(true) {
 xLastWakeTime = xTaskGetTickCount ();
 fsm_fire(fsm_pantalla);
 fsm_fire(fsm_raqueta);
 fsm_fire(fsm_pelota);
 vTaskDelayUntil(&xLastWakeTime, PERIOD_TICK);
 }
}

void user_init(void)
{
    xTaskCreate(&inter, "startup", 2048, NULL, 1, NULL);
}
