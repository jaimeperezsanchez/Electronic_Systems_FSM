#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#include "fsm.h"

#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS
#define TIMEOUT_TICK 5000/portTICK_RATE_MS
#define APAGADO_TICK 15000/portTICK_RATE_MS
#define ETS_GPIO_INTR_ENABLE() \
_xt_isr_unmask(1 << ETS_GPIO_INUM)
#define ETS_GPIO_INTR_DISABLE() \
_xt_isr_mask(1 << ETS_GPIO_INUM)

int mirar_flags (fsm_t *this);
int button_pressed_D8 (fsm_t *this);
void led_on (fsm_t *this);
void led_off (fsm_t *this);
int timeout (fsm_t *this);
int codigo_correcto (fsm_t *this);
void sumar (fsm_t *this);
void next_number (fsm_t *this);
void limpiar_flag (fsm_t *this) ;
int timeout_2 (fsm_t *this);
void led_off_timeout (fsm_t *this);


portTickType start_timeout;
portTickType start_timeout_apagado_de_luz;
volatile int done0=0;
volatile int done15=0;
int codigo_valido=0;
int indice=0;
volatile int codigos[2];
int primera_pulsacion=0;



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

enum fsm_state {
  APAGADO,
  ENCENDIDO,
};

int mirar_flags (fsm_t *this) {
  if(indice>2){
    return 0;
  }
  if(codigos[indice]>=10){
    return 0;
  }
  return (done0);
}
int button_pressed_D8 (fsm_t *this) {
 return (done15);
}
int timeout (fsm_t *this) {
  if (primera_pulsacion==0){
    return 0;
  }
  long diff_t = xTaskGetTickCount () - start_timeout;
  return (TIMEOUT_TICK < diff_t);
}
int timeout_2 (fsm_t *this) {
  long diff_t = xTaskGetTickCount () - start_timeout_apagado_de_luz;
  return (APAGADO_TICK < diff_t);
}
int codigo_correcto (fsm_t *this) {
  return (codigo_valido);
}

void led_on (fsm_t *this) {
  done15=0;
  start_timeout_apagado_de_luz = xTaskGetTickCount ();
  //printf("ENCENDEMOS LED\n");
  GPIO_OUTPUT_SET(2, 0);
}
void sumar (fsm_t *this) {
  done0=0;
  if (primera_pulsacion==1){
      if (codigos[indice]==9){
        codigos[indice]=0;
      }else{
        codigos[indice]++;
      }
  }else{
    primera_pulsacion=1;
  }
  start_timeout = xTaskGetTickCount ();
  //printf("PULSADO D3, codigo = %d\n", codigos[indice]);
}

void next_number (fsm_t *this) {
  if(indice<2){
    indice++;
    start_timeout = xTaskGetTickCount ();
    //printf("Timeout, indice = %d\n", indice);
  }else{
    if (codigos[0]==1 && codigos[1]==2 && codigos[2]==3){
      codigo_valido = 1;
      //printf("codigoooooooooo acertaaaaadooo");
    }else{
      indice=0;
      primera_pulsacion=0;
      int i;
      for(i=0; i<=2; i++){
        codigos[i]=0;
      }
      codigo_valido=0;
      //printf("AAAA JULIOOOOOOOOOOOOOOOOO");
    }

  }

}


void limpiar_flag (fsm_t *this) {
  indice=0;
  int i;
  for(i=0; i<=2; i++){
    codigos[i]=0;
  }
  done0=0;
  done15=0;
  codigo_valido=0;
  primera_pulsacion=0;
}

void led_off_timeout (fsm_t *this) {
  //printf("LED APAGADO\n");

  GPIO_OUTPUT_SET(2, 1);
}
void led_off (fsm_t *this) {
  //printf("LED APAGADO\n");
  codigo_valido=0;
  indice=0;
  int i;
  for(i=0; i<=2; i++){
    codigos[i]=0;
  }
  done0=0;
  done15=0;
  codigo_valido=0;
  primera_pulsacion=0;
  GPIO_OUTPUT_SET(2, 1);
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

void inter(void* ignore)
{
 static fsm_trans_t interruptor[] = {
  { APAGADO, codigo_correcto, ENCENDIDO, limpiar_flag },
  { ENCENDIDO, codigo_correcto, APAGADO, led_off},

  { APAGADO, mirar_flags, APAGADO, sumar},
  { APAGADO, timeout, APAGADO, next_number },

  { ENCENDIDO, mirar_flags, ENCENDIDO, sumar},
  { ENCENDIDO, timeout, ENCENDIDO, next_number },

  { ENCENDIDO, button_pressed_D8, ENCENDIDO, led_on },

  { ENCENDIDO, timeout_2, ENCENDIDO, led_off_timeout },

  {-1, NULL, -1, NULL },
 };

 PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);

 gpio_intr_handler_register((void*)isr_gpio, NULL);
 gpio_pin_intr_state_set(0, GPIO_PIN_INTR_NEGEDGE);
 gpio_pin_intr_state_set(15, GPIO_PIN_INTR_POSEDGE);
 ETS_GPIO_INTR_ENABLE();

 fsm_t* fsm = fsm_new(interruptor);
 led_off(fsm);
 portTickType xLastWakeTime;

 //printf("AL BUCLEEE INFINITOOOOO\n");


 while(true) {
 xLastWakeTime = xTaskGetTickCount ();
 fsm_fire(fsm);
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
    xTaskCreate(&inter, "startup", 2048, NULL, 1, NULL);
}
