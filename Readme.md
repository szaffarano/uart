## Librería UART

Una librería muy simple para utilzar el periférico UART de (por ahora) AVR ATmega328.

Se porta muy simple a otros UCs (en especial avr) modificando los registros.

### Limitaciones

Si la cola circular de escritura se satura (se pretende escribir más rápido de lo que se lee), el driver queda en 
un bucle infinito, pues la ISR tiene las interrupciones deshabilitadas.

La solución sería devolver un error con el efecto colateral de que quien usa el driver si no chequea el error va
a perder parte de la información que está intentando enviar.

### Ejemplo de uso

```c
#include <uart/uart.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef BAUD
#define BAUD	9600
#endif

/**
 * MCU: Atmega328
 * Fuses: Oscilador interno a 8 Mhz (sin dividir por 8)
 * 		-U lfuse:w:0xe2:m -U hfuse:w:0xd1:m -U efuse:w:0x07:m
 */
int main(void) {
	uart_init(BAUD);

	sei();

	while (1) {
		printf(".");
		_delay_ms(1000);
	}

	return (0);
}
```
