/**========================================================================
 * Rotina: Conjunto de Rotinas para emulação de comunicação serial RS232
   em 2 pinos genéricos
 * Função:Emula portas serial RX e TX em 2 pinos IO
 * Chamada:Serial_Pin
 * Retorna:Nada
 * Comentário: Originalmente feito para Atmega328
 * ----------------------------------------------------------------------*/
#include "soft_serial.h"

/*=======================================================================================================================
MOD_SOFT_SERIAL
Trata as rotinas que envolvem a comunicação serial via software (Bit-Banging)
Adaptado para rodar a 1MHz sem uso de structs.
=======================================================================================================================*/
#define Mod_Soft_Serial
#include "soft_serial.h"
#include <util/delay.h>

/*==============================================================================================
Rotina:	soft_serial_write (Equivalente a Wr_Data_Usart)
Função:	Escreve um byte na serial via software
Chamada: soft_serial_write(<data>)
-----------------------------------------------------------------------------------------------*/
// No seu .h: #define BIT_DELAY 58 // Para 9600bps @ 1.8432MHz

void soft_serial_write(uint8_t Dado)
{
    __asm__ __volatile__ (
        /* --- ESTADO DE REPOUSO (IDLE) --- */
        "  sbi %[port], %[pino]  \n\t"  // Garante 5V na linha antes de começar
        "  rcall delay_asm       \n\t"  // Pequeno fôlego para o receptor sincronizar

        /* --- START BIT --- */
        "  cbi %[port], %[pino]  \n\t"  // Cai para 0V (Início do frame)
        "  rcall delay_asm       \n\t"

        /* --- LOOP DE 8 BITS DE DADOS --- */
        "  ldi r18, 8            \n\t"  // Contador de bits
        "bit_loop:               \n\t"
        "  ror %[dado]           \n\t"  // Rotaciona o bit para o Carry
        "  brcc bit_zero         \n\t"  // Se Carry=0, pula para bit_zero
        "  sbi %[port], %[pino]  \n\t"  // Se Carry=1, linha em 5V
        "  rjmp bit_done         \n\t"
        "bit_zero:               \n\t"
        "  cbi %[port], %[pino]  \n\t"  // Linha em 0V
        "bit_done:               \n\t"
        "  rcall delay_asm       \n\t"  // Espera o tempo do bit (104us)
        "  dec r18               \n\t"
        "  brne bit_loop         \n\t"

        /* --- STOP BIT --- */
        "  sbi %[port], %[pino]  \n\t"  // Volta para 5V (Fim do frame)
        "  rcall delay_asm       \n\t"
        "  rjmp end_asm          \n\t"

        /* --- SUB-ROTINA DE DELAY (9600 bps) --- */
        "delay_asm:              \n\t"
        "  mov r19, %[delay]     \n\t"  // Carrega o valor de BIT_DELAY (ex: 57 ou 58)
        "d_loop:                 \n\t"
        "  dec r19               \n\t"
        "  brne d_loop           \n\t"
        "  ret                   \n\t"

        "end_asm:                \n\t"

        /* --- MAPEAMENTO DE ENTRADAS/SAÍDAS --- */
        : : [port]  "I" (_SFR_IO_ADDR(TX_PORT)), // Porta de saída (ex: PORTD)
            [pino]  "I" (SOFT_TX_BIT),           // Pino de saída (ex: PD1)
            [dado]  "r" (Dado),                 // O byte a ser enviado
            [delay] "r" (BIT_DELAY)              // Valor calculado para o cristal
        : "r18", "r19"                           // Registradores usados no Assembly
    );
}
/*========================================================================
 ROTINA  : Envia_String_Soft
 FUNCAO  : Transmite uma string previamente definida na Flash (__flash)
========================================================================*/
void Envia_String_Soft(__flash const unsigned char *pont_mens)
{
	while (*pont_mens != 0)
	{
		soft_serial_write(*pont_mens);
		pont_mens++;
	}
}

/*========================================================================
ROTINA  : Serial_Print_Soft (Equivalente a Serial_Print)
FUNCAO  : Transmite uma string da RAM via Soft Serial.
========================================================================*/
void Serial_Print_Soft (char *pont_mens)
{
	while (*pont_mens != 0)
	{
		soft_serial_write(*pont_mens);
		pont_mens++;
	}
}

/*==============================================================================================
Rotina:  EnvNumSoft
Função:  Envia números formatados (Equivalente a EnvNumUsart)
-----------------------------------------------------------------------------------------------*/
void EnvNumSoft (uint8_t Qtde_Algarismos, uint16_t Numero)
{
	unsigned char Dezena_Milhar, Milhar, Centena, Dezena, Unidade;

	Dezena_Milhar = Numero/10000;
	Milhar =  ((Numero-Dezena_Milhar*10000))/1000;
	Centena = (Numero - Dezena_Milhar*10000 - Milhar*1000)/100;
	Dezena = (Numero - Dezena_Milhar*10000 - Milhar*1000 - Centena*100)/10;
	Unidade = (Numero - Dezena_Milhar*10000 - Milhar*1000 - Centena*100 - Dezena*10);

	switch(Qtde_Algarismos)
	{
		case 5:
		soft_serial_write(Dezena_Milhar + '0');
		__attribute__ ((fallthrough));
		case 4:
		soft_serial_write(Milhar + '0');
		__attribute__ ((fallthrough));
		case 3:
		soft_serial_write(Centena + '0');
		__attribute__ ((fallthrough));
		case 2:
		soft_serial_write(Dezena + '0');
		__attribute__ ((fallthrough));
		case 1:
		soft_serial_write(Unidade + '0');
	}
}

/*==============================================================================================
Rotina: soft_serial_init (Equivalente a Config_Usart)
Função: Inicializa os pinos de TX e RX conforme definido no .h
-----------------------------------------------------------------------------------------------*/
void soft_serial_init(void)
{
	TX_DDR  |= (1 << SOFT_TX_BIT);   // TX como saída
	RX_DDR  &= ~(1 << SOFT_RX_BIT);  // RX como entrada
	TX_PORT |= (1 << SOFT_TX_BIT);   // TX em nível alto (Idle)
}

/*==============================================================================================
Rotina:  soft_serial_available
Função:  Verifica se há início de recepção (Start Bit) no pino RX
-----------------------------------------------------------------------------------------------*/
uint8_t soft_serial_available(void)
{
	return !(RX_PIN_REG & (1 << SOFT_RX_BIT));
}

/*==============================================================================================
Rotina:  soft_serial_read
Função:  Lê um byte via software
-----------------------------------------------------------------------------------------------*/
char soft_serial_read(void)
{
	char caractere = 0;

	// Sincroniza com o meio do Start Bit
	_delay_us(BIT_DELAY + (BIT_DELAY / 2));

	for (uint8_t i = 0; i < 8; i++)
	{
		if (RX_PIN_REG & (1 << SOFT_RX_BIT))
		{
			caractere |= (1 << i);
		}
		_delay_us(BIT_DELAY);
	}

	return caractere;
}
