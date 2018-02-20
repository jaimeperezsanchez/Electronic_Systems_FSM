#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#include "fsm.h"
#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS
#define TIMEOUT_TICK 10000/portTICK_RATE_MS
#define ETS_GPIO_INTR_ENABLE() \
_xt_isr_unmask(1 << ETS_GPIO_INUM)
#define ETS_GPIO_INTR_DISABLE() \
_xt_isr_mask(1 << ETS_GPIO_INUM)
int button_pressed_D3 (fsm_t *this);
int button_pressed_D8 (fsm_t *this);
void led_on (fsm_t *this);
void led_off (fsm_t *this);
void done0_0 (fsm_t *this);
void prueba_timeout (fsm_t *this);

portTickType start_timeout;

int done0=0;
int done15=0;


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
  APAGADO_0,
  ENCENDIDO_0,
  APAGADO_1,
  ENCENDIDO_1,
  APAGADO_2,
  ENCENDIDO_2,
  APAGADO_3,
  ENCENDIDO_3,
};

int button_pressed_D3 (fsm_t *this) {
   return (done0);
}
int button_pressed_D8 (fsm_t *this) {
 return (done15);
}
int timeout (fsm_t *this) {
  long diff_t = xTaskGetTickCount () - start_timeout;
  return (TIMEOUT_TICK < diff_t);
}
void led_on (fsm_t *this) {
  done15=0;
  printf("ENCENDEMOS LED\n");
  GPIO_OUTPUT_SET(2, 0);
}
void done0_0 (fsm_t *this) {
  done0=0;
  start_timeout = xTaskGetTickCount ();
  printf("PULSADO D3\n");
}

void prueba_timeout (fsm_t *this) {
  printf("TIMEOUT AGOTADO\n");
}

void led_off (fsm_t *this) {
  printf("LED APAGADO\n");
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
  { APAGADO_0, button_pressed_D3, APAGADO_1, done0_0 },
  { APAGADO_1, button_pressed_D3, APAGADO_2, done0_0},
  { APAGADO_2, button_pressed_D3, APAGADO_3, done0_0},
  { APAGADO_3, timeout, ENCENDIDO_0, prueba_timeout },

  { APAGADO_1, timeout, APAGADO_0, prueba_timeout},
  { APAGADO_2, timeout, APAGADO_0, prueba_timeout},
  { APAGADO_3, button_pressed_D3, APAGADO_0, done0_0 },

  { ENCENDIDO_0, button_pressed_D8, ENCENDIDO_0, led_on},

  { ENCENDIDO_0, button_pressed_D3, ENCENDIDO_1, done0_0},
  { ENCENDIDO_1, button_pressed_D3, ENCENDIDO_2, done0_0 },
  { ENCENDIDO_2, button_pressed_D3, ENCENDIDO_3, done0_0},
  { ENCENDIDO_3, timeout, APAGADO_0, led_off},

  { ENCENDIDO_1, timeout, ENCENDIDO_0, prueba_timeout },
  { ENCENDIDO_2, timeout, ENCENDIDO_0, prueba_timeout},
  { ENCENDIDO_3, button_pressed_D3, ENCENDIDO_0, done0_0},
  {-1, NULL, -1, NULL },
 };

 PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
/*
 GPIO_ConfigTypeDef io_conf_0;
 io_conf_0.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
 io_conf_0.GPIO_Mode = GPIO_Mode_Input;
 io_conf_0.GPIO_Pin = BIT(0);
 io_conf_0.GPIO_Pullup = GPIO_PullUp_EN;
 gpio_config(&io_conf_0);

 GPIO_ConfigTypeDef io_conf_15;
 io_conf_15.GPIO_IntrType = GPIO_PIN_INTR_POSEDGE;
 io_conf_15.GPIO_Mode = GPIO_Mode_Input;
 io_conf_15.GPIO_Pin = BIT(15);
 io_conf_15.GPIO_Pullup = GPIO_PullUp_EN;
 gpio_config(&io_conf_15);
*/


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
