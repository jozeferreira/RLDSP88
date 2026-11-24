/*=======================================================================================================================
MOD_SOFT_SERIAL.H
Cabeçalho para comunicação serial via software em 1MHz
=======================================================================================================================*/
#ifndef F_CPU
#define F_CPU 1843200UL
#endif

#ifndef SOFT_SERIAL_H
#define SOFT_SERIAL_H

#include <avr/io.h>

// --- CONFIGURAÇÃO DE PINOS ---
// Altere aqui para usar qualquer pino do ATmega sem ocupar o PORTD inteiro
#define SOFT_TX_LETTER  B
#define SOFT_TX_BIT     PB3

#define SOFT_RX_LETTER  B
#define SOFT_RX_BIT     PB4

// --- CÁLCULO DE BAUD RATE (9600 bps @ 1.8432MHz) ---
// 1/9600 = 102μs. O valor 102 compensa o overhead das chamadas de função.
#define BIT_DELAY 62

// --- MACROS DE ABSTRAÇÃO ---
#define CONCAT(a, b) a ## b
#define PORT(letter) CONCAT(PORT, letter)
#define DDR(letter)  CONCAT(DDR, letter)
#define PIN(letter)  CONCAT(PIN, letter)

#define TX_PORT PORT(SOFT_TX_LETTER)
#define TX_DDR  DDR(SOFT_TX_LETTER)

#define RX_PIN_REG PIN(SOFT_RX_LETTER)
#define RX_DDR     DDR(SOFT_RX_LETTER)

#define ENTER() Serial_Print_Soft("\r\n")

/*==============================================================================================
PROTÓTIPOS DAS FUNÇÕES
===============================================================================================*/

void soft_serial_init(void);

void soft_serial_write(uint8_t Dado);

void Envia_String_Soft(__flash const unsigned char *pont_mens);

void Serial_Print_Soft (char *pont_mens);

void EnvNumSoft (uint8_t Qtde_Algarismos, uint16_t Numero);

uint8_t soft_serial_available(void);

char soft_serial_read(void);

#endif
