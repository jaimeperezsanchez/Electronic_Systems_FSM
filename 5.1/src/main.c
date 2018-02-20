#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <time.h>
#include "fsm.h"
#define PERIOD_TICK 100/portTICK_RATE_MS
#define ESPERA 60000
int button_pressed_D3 (fsm_t *this);
int timeout (fsm_t *this);
void led_on (fsm_t *this);
void led_off (fsm_t *this);

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
enum fsm_state {
  LED_ON,
  LED_OFF,
};

//time_t start_time, end_time;
//double diff_t;
portTickType start_time, end_time;
long diff;

int button_pressed_D3 (fsm_t *this) {
 return (!GPIO_INPUT_GET(0));
}
int timeout (fsm_t *this) {
  end_time = xTaskGetTickCount() * portTICK_RATE_MS;
  diff = end_time - start_time;
  return (ESPERA < diff);
}
void led_on (fsm_t *this) {
  start_time = xTaskGetTickCount() * portTICK_RATE_MS;
  GPIO_OUTPUT_SET(2, 0);
}
void led_off (fsm_t *this) {
  GPIO_OUTPUT_SET(2, 1);
}

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


void inter(void* ignore)
{
 static fsm_trans_t interruptor[] = {
  { LED_OFF, button_pressed_D3, LED_ON, led_on },
  { LED_ON, button_pressed_D3, LED_ON, led_on},
  { LED_ON, timeout, LED_OFF, led_off},
  {-1, NULL, -1, NULL },
 };

 PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);
 fsm_t* fsm = fsm_new(interruptor);
 led_off(fsm);
 portTickType xLastWakeTime;

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
