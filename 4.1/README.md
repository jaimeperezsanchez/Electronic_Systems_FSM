### Ejercicio 4 
Se desea implementar el control de una lámpara que tiene múltiples pulsadores, con
una plataforma WeMos D1 mini. Para simplificar, utilizaremos el LED integrado en la placa
como lámpara, que corresponde con el pin D2 cuando está configurado como salida. La única
diferencia con el sistema real es que a esta salida iría conectado un relé.
1. Supondremos que los pulsadores están conectados a GPIOs del mismo
dispositivo, configurados como entradas. Debe implementarse un antirrebotes software.
