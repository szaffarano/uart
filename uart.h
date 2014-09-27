/*
 * uart.h
 *
 *  Created on: 07/06/2014
 *      Author: sebas
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#define USART_BAUD_PRESCALE(BR) ((F_CPU) / ((BR) * 16l) - 1)

#define USART_BUFFER_SIZE			128

/**
 * Buffer para almacenar la información que se envia y recive.
 * Sus índices se usan para agregar y sacar datos del mismo.  Es circular,
 * haciendo un AND lógico entre el indice deseado y el tamaño máximo
 * se tiene la ubicación actual, sea para agregar o para quitar
 * elementos.
 * Si habiendo agregado un elemento los dos índices son iguales, hay
 * overflow.
 * La única restricción es que al hacer bitwise operations con el
 * tamaño, éste debe ser múltiplo de 2.
 */
typedef struct _buffer_t {
	volatile unsigned char* buffer;
	volatile unsigned char push_idx;
	volatile unsigned char pop_idx;
} buffer_t;

/**
 * Inicializa la UART.  Por el momento utiliza registros
 * de Atmega328 aunque los mismos son compatibles con muchos
 * modelos Atmega.  Eventualmente cambiando algunas definiciones
 * se puede portar a más modelos.
 * También configura stdout y stderr para que las librerías
 * estándar de C escriban a la UART (ej, printf).
 */
void uart_init(uint16_t baud);

/**
 * Escribe un byte en la UART.
 * En el caso en que el buffer de escritura esté
 * lleno se bloquea hasta que el mismo se libere.
 */
void uart_putc(uint8_t byte);

/**
 * Lee un byte de la UART o devuelve cero si no hay nada que leer
 */
uint8_t uart_getc(void);

uint8_t uart_available(void);

#endif /* UART_H_ */
