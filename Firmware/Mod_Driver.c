/**=========================================================================
	Esse Módulo contém rotinas de manipulação do Hardware do Arduino.
-------------------------------------------------------------------------*/
#define Mod_Driver
#include "Mod_Def.h"

uint8_t pont_bit;
uint8_t bit;

volatile uint8_t *Tab_Ports[]={&PORTB,&PORTC,&PORTD};
volatile uint8_t *Tab_Pins[]={&PINB,&PINC,&PIND};
volatile uint8_t *Tab_DDRs[]={&DDRB,&DDRC,&DDRD};

volatile uint8_t *Tab_Ram_Bit[]={&AddresBit_0,&AddresBit_1,&AddresBit_2,&PORTC_Img};
/**========================================================================
 * Rotina: Teste_Userbit
 * Função: Testa se o bit desejado é 0 ou 1
 * Chamada: Set_Userbit <número do bit>
 * Retorna: Nada
 * Comentário:Essa rotina foi feita para manipulação de bits da memória Ram interna
 * ----------------------------------------------------------------------*/
uint8_t Teste_Userbit (uint8_t num_bit)
{
	uint8_t Status_Bit = 0;

	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	Status_Bit = *Tab_Ram_Bit[pont_bit] & (1 << bit);

	if(Status_Bit != 0)
	Status_Bit = 1;

	return (Status_Bit);
}
/**========================================================================
 * Rotina: Set_Userbit
 * Função: Coloca 1 no bit desejada
 * Chamada: Set_Userbit <número do bit>
 * Retorna: Nada
 * Comentário:Essa rotina foi feita para manipulação de bits da memória Ram interna
 * ----------------------------------------------------------------------*/
void Set_Userbit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	*Tab_Ram_Bit[pont_bit] |= (1 << bit);
}
/**========================================================================
 * Rotina:	Clr_Userbit()
 * Função:	Coloca o bit desejado em 0
 * Chamada: Clr_Bit(<número do bit>)
 * Retorna: Nada
 * Comentário:Essa rotina foi feita para manipulação de bits da memória Ram interna
 * ----------------------------------------------------------------------*/
void Clr_Userbit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	*Tab_Ram_Bit[pont_bit] &= ~(1 << bit);
}
/**========================================================================
 * Rotina:Cpl_Userbit
 * Função: Muda o estado do bit informado
 * Chamada:Cpl_Userbit(<bit>)
 * Retorna: Nada
 * Comentário:N/A
 * ----------------------------------------------------------------------*/
void Cpl_Userbit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	*Tab_Ram_Bit[pont_bit] ^= (1 << bit);
}
/**========================================================================
 * Rotina: Set_Port_Bit
 * Função: Coloca nível "1" em um bit do pórtico
 * Chamada:Set_Port_Bit(bit do Pórtico)
 * Retorna: nada
 * Comentário:
 * ----------------------------------------------------------------------*/
void Set_Port_Bit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	*Tab_Ports[pont_bit] |= (1 << bit);
}
/**========================================================================
 * Rotina: Clr_Port_Bit
 * Função: Coloca nível "0" em um bit do pórtico
 * Chamada:Clr_Port_Bit(bit do Pórtico)
 * Retorna: nada
 * Comentário:
 * ----------------------------------------------------------------------*/
void Clr_Port_Bit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit
	*Tab_Ports[pont_bit] &= ~(1 << bit);
}
/**========================================================================
 * Rotina: Cpl_Port_Bit
 * Função: Complementa um bit do pórtico
 * Chamada:Cpl_Port_Bit(bit do Pórtico)
 * Retorna: nada
 * Comentário:
 * ----------------------------------------------------------------------*/
void Cpl_Port_Bit(uint8_t num_bit)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	*Tab_Ports[pont_bit] ^= (1 << bit);
}
/**========================================================================
 * Rotina: Teste_Port_Bit
 * Função: Testa o estado de um bit do pórtico
 * Chamada:Teste_Port_Bit(bit do Pórtico)
 * Retorna: 0 se o o bit estiver em "0" e um valor qualquer se em "1"
 * Comentário:
 * ----------------------------------------------------------------------*/
uint8_t Teste_Port_Bit(uint8_t num_bit)
{
	uint8_t Status_Bit;

	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	Status_Bit = *Tab_Pins[pont_bit] & (1 << bit);
	if(Status_Bit != 0)
	Status_Bit = 1;

	return (Status_Bit);
}
/**========================================================================
 * Rotina: Config_Port_Bit
 * Função: Configura um bit do pórtico como leitura ou escrita
 * Chamada:Config_Port_Bit(<bit do Pórtico>,<estado>)
 * Retorna:
 * Comentário:estado deve ser 0 para Leitura e 1 para escrita
 * ----------------------------------------------------------------------*/
void Config_Port_Bit(uint8_t num_bit,uint8_t rw)
{
	pont_bit = num_bit/8;	//Calculou o PORT
	bit = num_bit%8;	//Calcula o bit

	if(rw == INPUT)
	*Tab_DDRs[pont_bit] &= ~(1 << bit);
	else
	*Tab_DDRs[pont_bit] |= (1 << bit);
}
/*===========================================================================================
Rotina Wait:
Função:Gera um delay em milisegundos
Descrição: A rotina gera um delay em milesegundos informados na chamada.
==============================================================================================*/
void Wait (unsigned int buf_loop_delay)
{
	unsigned int x;

	for (x = buf_loop_delay ; x > 0; x--)
	_delay_ms(1);
}
/*===========================================================================================
Rotina: Config_ADC()
Função: Configura o ADC para Single Conversion mode (ADCSRA = 0)
Descrição:
=============================================================================================*/
void Config_ADC()
{
	ADCSRA = 0;
	//ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //Prescaler at 128 so we have an 125Khz clock source
	ADCSRA |= (1<<ADPS0);    		//Prescaler = 2 (115KHz @ 230400Hz)

	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);                //Avcc(+5v) as voltage reference
	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));    //ADC in free-running mode
	ADCSRA &= ~(1<<ADATE);              //Single Conversion mode

	ADCSRA |= (1<<ADEN);                //Power up the ADC
	//ADCSRA |= (1<<ADSC);               //Start converting
}
/**========================================================================
 * Rotina:  Le_ADC()
 * Função:  Le o canal ADC informado
 * Chamada: Le_ADC(<canal>)
 * Retorna: Nada
 * Comentário: Devolve o valor lido em ADCW
 * ----------------------------------------------------------------------*/
void Le_ADC(uint8_t CH_ADC)
{

	ADMUX	&=	0xf0;
	ADMUX |= CH_ADC;
	ADCSRA |= (1<<ADSC);                //Start converting
	while((ADCSRA & (1<<ADSC)))NOP();		//Aguarda Conversão
	Leitura_ADC = ADCW;
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/





 /**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
