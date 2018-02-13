#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
const char* morse ( char c);
void str2morse ( const char *buf , int n , const char* str );
void morse_send ( const char* msg );


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

/**void task_blink(void* ignore)
{
  //gpio16_output_conf();
  while(true) {
    GPIO_OUTPUT_SET(2,1);
      vTaskDelay(1000/portTICK_RATE_MS);
    GPIO_OUTPUT_SET(2,0);
      vTaskDelay(1000/portTICK_RATE_MS);
  }

    vTaskDelete(NULL);
}**/
void morsee(void* ignore){
  const char* str = "hola";
  const char* buf;
  int n = 0;
  str2morse (buf, n, str);
}

const char* morse ( char c) {
  static const char* morse_ch [] = {
    ". -   ", /* A */
    "- . . .   ", /* B */
    "- . - .   ", /* C */
    "- . .   ", /* D */
    ".   ", /* E */
    ". . - .   ", /* F */
    "- - .   ", /* G */
    ". . . .   ", /* H */
    ". .   ", /* I */
    ". - - -   ", /* J */
    "- . -   ", /* K */
    ". - . .   ", /*L */
    "- -   ", /* M */
    "- .   ", /* N */
    "- - -   ", /* 0 */
    ". - - .   ", /* P */
    "- - . -   ", /* Q */
    ". - .   ", /* R */
    ". . .   ", /* S */
    "-   ", /* T */
    ". . -   ", /* U */
    ". . . -   ", /* V */
    ". - -   ", /* W */
    "- . . -   ", /* X */
    "- . - -   ", /* Y */
    "- - . .   ", /* Z */
    "- - - - -   ", /* 0 */
    ". - - - -   ", /* 1 */
    ". . - - -   ", /* 2 */
    ". . . - -   ", /* 3 */
    ". . . . -   ", /* 4 */
    ". . . . .   ", /* 5 */
    "- . . . .   ", /* 6 */
    "- - . . .   ", /* 7 */
    "- - - . .   ", /* 8 */
    "- - - -  .   ", /* 9 */
    ". . .  . . .   ", /* . */
    ". - . - . -   " , /* , */
    ". . - - . .   ", /* ? */
    "- - . . - -   "  /* ! */

    } ;
if (c == ' '){
  return "     ";
}else{
  return morse_ch [ c - 'a' ];
}

}

void str2morse ( const char* buf , int n , const char* str ) {

      while (*(str + n) != '\0'){
        n++;
      }

       int i;

       for (i = 0; i < n ; i++){
           buf = morse(*(str + i));
           buf++;
        }

        buf = buf - n;

      for (i = 0; i < n ; i++){
        morse_send(buf + i);
      }
 }

 void morse_send (const char* msg ) {
   switch (*msg ) {
      case '.':
        GPIO_OUTPUT_SET(2,0);
        vTaskDelay(250/portTICK_RATE_MS);
        break ;
      case '-':
        GPIO_OUTPUT_SET(2,0);
        vTaskDelay(750/portTICK_RATE_MS);
        break ;
      case ' ':
        GPIO_OUTPUT_SET(2,1);
        vTaskDelay(250/portTICK_RATE_MS);
        break ;
      case '\0':
        return ;
    }
    morse_send (++msg ) ;
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    xTaskCreate(&morsee, "startup", 2048, NULL, 1, NULL);
}
