/**=========================================================================
	Esse Módulo contém rotinas de manipulação do Hardware do Arduino.
-------------------------------------------------------------------------*/
#define Mod_Driver
#include "Mod_Def.h"
/*===========================================================================================
Rotina Wait:
Função:Gera um Wait em milisegundos
Descrição: A rotina gera um Wait em milesegundos informados na chamada.
==============================================================================================*/
void Wait (unsigned int buf_loop_delay)
{
#ifdef CLKXTAL_230400Hz
#define Cte_Delay			20		//Cristal	230400Hz.
#else
#define Cte_Delay			160
#endif

	unsigned char y;
	unsigned int x;

	for (x = buf_loop_delay ; x > 0; x--)
	{

		for (y = Cte_Delay; y > 0; y--)			//Gera 1ms

		{}
	}
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
void Le_ADC(unsigned char CH_ADC)
{

	ADMUX	&=	0xf0;
	ADMUX |= CH_ADC;
	ADCSRA |= (1<<ADSC);                //Start converting
	while((ADCSRA & (1<<ADSC))){}		//Aguarda Conversão
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
