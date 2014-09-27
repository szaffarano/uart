/*
 * uart.c
 *
 *  Created on: 07/06/2014
 *      Author: sebas
 */

#include <avr/io.h>
#include <uart/uart.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#define USART_BUFFER_MASK			(USART_BUFFER_SIZE - 1)
#define	USART_NEXT_BUFFER_IDX(idx)	((idx+1) & USART_BUFFER_MASK)

buffer_t tx = { .push_idx = 0, .pop_idx = 0 };
buffer_t rx = { .push_idx = 0, .pop_idx = 0 };
FILE file;

int send_stdout(char c, FILE *f);

/**
 * Inicializa la USART para enviar y recibir.
 * Formato: frame de 8 bits con 1 de parada
 */
void uart_init(uint16_t baud) {
	UBRR0H = (USART_BAUD_PRESCALE(baud) >> 8);
	UBRR0L = (0x00FF & USART_BAUD_PRESCALE(baud));

	UCSR0B |= _BV(RXCIE0); // habilitar interrupcion de recepcion de datos
	UCSR0B |= _BV(RXEN0) | _BV(TXEN0); // habilitar tx y rx
	UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01); // frame de 8 bits

	// buffers de transmisión / recepción
	tx.buffer = malloc(USART_BUFFER_SIZE);
	rx.buffer = malloc(USART_BUFFER_SIZE);

	// sobreescribo stdout/stderr para wrapear en printf
	fdev_setup_stream(&file, send_stdout, NULL, _FDEV_SETUP_WRITE);
	stdout = &file;
	stderr = &file;
}

void uart_putc(uint8_t byte) {
	unsigned char next_idx = USART_NEXT_BUFFER_IDX(tx.push_idx);

	while (next_idx == tx.pop_idx) {
		// @TODO: está mal, hay que devolver error y no bloquear!
		// no hay espacio en el buffer
	}

	tx.push_idx = next_idx;
	tx.buffer[next_idx] = byte;

	// habilito interrupción para enviar el dato.
	UCSR0B |= _BV(UDRIE0);
}

uint8_t uart_getc(void) {
	uint8_t next_idx;
	uint8_t r;

	if (rx.pop_idx == rx.push_idx) {
		return 0;
	}
	next_idx = USART_NEXT_BUFFER_IDX(rx.pop_idx);
	r = rx.buffer[next_idx];
	rx.pop_idx = next_idx;

	return r;
}

uint8_t uart_available(void) {
	return (rx.push_idx > rx.pop_idx) ? (rx.push_idx - rx.pop_idx) : 0;
}

ISR(USART_RX_vect) {
	uint8_t next_idx;
	uint8_t data;

//@TODO manejar estado / errores

	data = UDR0;

	next_idx = USART_NEXT_BUFFER_IDX(rx.push_idx);

	if (next_idx == rx.pop_idx) {
		// overflow
	} else {
		rx.push_idx = next_idx;
		rx.buffer[next_idx] = data;
	}
}

ISR(USART_UDRE_vect) {
	if (tx.push_idx != tx.pop_idx) {
		tx.pop_idx = USART_NEXT_BUFFER_IDX(tx.pop_idx);
		UDR0 = tx.buffer[tx.pop_idx];
	} else {
		// no hay mas datos para enviar, deshabilito la interrupción hasta tener más.
		UCSR0B &= ~_BV(UDRIE0);
	}
}

int send_stdout(char c, FILE *f) {
	uart_putc(c);
	return 0;
}
