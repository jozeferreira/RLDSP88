/*===============================================================================

				+++ MÓDULO DE MANIPULAÇÃO DO TIMER 1 +++

===============================================================================*/
#define Mod_Timer
#include "Mod_Def.h"

//------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)          // interrupção por igualdade de comparação no TIMER1 Atmega88
{
	//	PORTB |= (1 << PB3);	//TESTE

/*--------------------------------------------------------------
			Temporiza o relógio
--------------------------------------------------------------*/

	if(TESTE_USERBIT(bAjuste) == 0)		//Não está ajustando nada
	{
		if(--Real_Timer == 0)			//É a cadência do tempo de 1 em 1 segundo
		{
			Real_Timer = 1000/PERIODO_TIMER_1;				//Carrega com 1 segundo

			Segundo++;
			if(Segundo == 60)
			{
				Segundo = 0;
				Minuto++;
				if(Minuto == 60)
				{
					Minuto = 0;
					//---------------------------------------
					Hora++;
					if(Hora == 24)
					Hora = 0;
				}
			}
			Display_Hora = Hora;
			Display_Minuto = Minuto;
		}
	}
	/*------------------------------------------
		Temporiza o contador de acionamento das Chaves
		para entrar no avanço rápido
	--------------------------------------------*/
	if(Timer_SW_ACIONADA > 0)
	Timer_SW_ACIONADA --;
	/*------------------------------------------
		Temporizador de uso Geral
	--------------------------------------------*/
	if(Timer_Wait > 0)
	Timer_Wait--;
	/*---------------------------------------
	Temporiza a cadência do Monitoramento de Bateria
	-----------------------------------------*/
	if (Timer_Mon_Energia > 0)
	Timer_Mon_Energia--;
	/*-----------------------------------------
	Temporiza as piscadas dos Leds no Modo Ajuste
	--------------------------------------------*/
	if (Timer_Led_Desp > 0)
	Timer_Led_Desp--;
	/*-----------------------------------------
	Temporiza as piscadas do Led Segundo
	--------------------------------------------*/
	if(Timer_Led_Seg > 0)
	Timer_Led_Seg--;
	/*-----------------------------------------
	Temporiza a saída automática do modo ajuste
	--------------------------------------------*/
	if(Timer_Over_ADJ > 0)
	Timer_Over_ADJ--;
	/*-----------------------------------------
	Temporiza as piscadas dos leds dos despertadores durante um alarme
	--------------------------------------------*/
	if(Timer_Led_Alarme > 0)
	Timer_Led_Alarme--;
	/*-----------------------------------------
	Temporiza o toque do buzzer durante um Alarme
	--------------------------------------------*/
	if(Timer_Buzzer_Alarme > 0)
	Timer_Buzzer_Alarme--;
	/*-----------------------------------------
	Temporiza parada automática do despertador
	--------------------------------------------*/
	if(Timer_Over_Despertador > 0)
	Timer_Over_Despertador--;
/*-----------------------------------------------------------------------
			Temporiza a cadência da piscada do led do segundo
-----------------------------------------------------------------------*/
	if(Timer_Led_Seg == 0 && TESTE_USERBIT(bAjuste) == 0)		//Não fazendo ajuste
	{
		if(TESTE_USERBIT(bPower_Fail) != 0)		//Falta de energia AC
		{
			if((PORTC_Img & (1 << Led_Seg)) == 0)		//Led Apagado?
			{
				Timer_Led_Seg = 200/PERIODO_TIMER_1;
				ACENDE_LED_SEGUNDO();
			}
			else
			{
				Timer_Led_Seg = 2800/PERIODO_TIMER_1;
				APAGA_LED_SEGUNDO();
			}

		}
		else if(TESTE_USERBIT(bNear_Low_Bat) != 0)		//Bateria está perto de acabar
		{

			if((PORTC_Img & (1 << Led_Seg)) == 0 )		//Led Apagado?
			{
				Timer_Led_Seg = 100/PERIODO_TIMER_1;
				ACENDE_LED_SEGUNDO();
			}
			else
			{
				Timer_Led_Seg = 2900/PERIODO_TIMER_1;
				APAGA_LED_SEGUNDO();
			}

		}
		else if(TESTE_USERBIT(bLow_Bat) != 0)		//Bateria já era
		{

			if((PORTC_Img & (1 << Led_Seg)) == 0)		//Led Apagado?
			{
				Timer_Led_Seg = 100/PERIODO_TIMER_1;
				ACENDE_LED_SEGUNDO();
			}
			else
			{
				Timer_Led_Seg = 500/PERIODO_TIMER_1;
				APAGA_LED_SEGUNDO();
			}

		}
		else
		{
			Timer_Led_Seg = 500/PERIODO_TIMER_1;			//2 Piscadas por segundo
			CPL_LED_SEG();
		}
	}

/*-----------------------------------------------------------------------
	Temporiza as piscadas dos leds dos despertadores no modo ajuste ou quando estiver despertando
-------------------------------------------------------------------------*/
	if( TESTE_USERBIT(bAjuste)!= 0)
	{

		if(Timer_Led_Desp == 0)					//Temporiza as piscadas dos leds no ajuste
		{
			Timer_Led_Desp = 100/PERIODO_TIMER_1;	//Recarrega o Timer

			if(pTabHoraMinuto == 1)
			CPL_LED_DESP(Led_Desp_1);

			else if(pTabHoraMinuto == 2)	//Está ajustando o Despertador 2
			CPL_LED_DESP(Led_Desp_2);

			else if(pTabHoraMinuto == 3)//Está ajustando o Despertador 3
			CPL_LED_DESP(Led_Desp_3);

			else if(pTabHoraMinuto == 0)		//Está ajustando a Hora
			CPL_LED_SEG();		//Pisca o led do segundo

		}
		/*-----------------------------------------------------------------------
		Gerencia  sinalização do Led Segundo para os despertadores habilitados ou não
		durante o ajuste. Aceso se despertador estiver habilitado
		-------------------------------------------------------------------------*/
		if(pTabHoraMinuto != 0) //Não está ajustando a hora
		{
			if(pTabHoraMinuto == 1)
			{
				if(TESTE_USERBIT(bHab_Desp1) != 0)	//	//Despertador Habilitado
				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO()
			}

			if(pTabHoraMinuto == 2)
			{
				if(TESTE_USERBIT(bHab_Desp2) != 0)	//	//Despertador Habilitado
				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO()
			}
			if(pTabHoraMinuto == 3)	//	//Despertador Habilitado
			{
				if(TESTE_USERBIT(bHab_Desp3) != 0)
				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO()
			}
		}
	}
	/*-----------------------------------------------------------------------
	Gerencia  sinalização dos leds dos despertadores
	Aceso se despertador estiver habilitado
	------------------------------------------------------------------------*/
	if(TESTE_USERBIT(bDesp1_ON) == 0 && TESTE_USERBIT(bDesp2_ON) == 0 && TESTE_USERBIT(bDesp3_ON) == 0)
	{
		if((TESTE_USERBIT(bAjuste) != 0 && pTabHoraMinuto != 1) || TESTE_USERBIT(bAjuste) == 0)
		{
			if(TESTE_USERBIT(bHab_Desp1) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_1);
			else
			APAGA_LED_DESP(Led_Desp_1);
		}
		//----------------------------------------------------------------
		if((TESTE_USERBIT(bAjuste) != 0 && pTabHoraMinuto  != 2) || TESTE_USERBIT(bAjuste) == 0)
		{
			if(TESTE_USERBIT(bHab_Desp2) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_2);
			else
			APAGA_LED_DESP(Led_Desp_2);
		}
		//----------------------------------------------------------------
		if((TESTE_USERBIT(bAjuste) != 0 && pTabHoraMinuto != 3) || TESTE_USERBIT(bAjuste) == 0)
		{
			if(TESTE_USERBIT(bHab_Desp3) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_3);
			else
			APAGA_LED_DESP(Led_Desp_3);
		}
	}
	/*--------------------------------------------------------------------------
						Hora de Despertar?
	--------------------------------------------------------------------------*/
	//Não deixa habilitar despertador durante um ajuste e durante despertador ativo
	// Segundo = 0 previne repetir ativação dentro mesmo minuto
	if(TESTE_USERBIT(bAjuste) == 0 && TESTE_USERBIT(bAlarme_ON) == 0 && Segundo == 0)
	{
		if(TESTE_USERBIT(bHab_Desp1) != 0)		//Programado para despertar
		{
			if(*Tab_Hora_Desp[1] == Hora && *Tab_Minuto_Desp[1] == Minuto)	//Ops, A hora bateu.
			SET_USERBIT(bDesp1_ON);		//Sinaliza para despertar Despertador 1
			SET_USERBIT(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = 30000/PERIODO_TIMER_1;		//30 segundos para parada automática
		}
		if(TESTE_USERBIT(bHab_Desp2) != 0)		//Programado para despertar
		{
			if(*Tab_Hora_Desp[2] == Hora && *Tab_Minuto_Desp[2] == Minuto)	//Ops, A hora bateu.
			SET_USERBIT(bDesp2_ON);		//Sinaliza para despertar Despertador 2
			SET_USERBIT(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = 30000/PERIODO_TIMER_1;		//30 segundos para parada automática
		}
		if(TESTE_USERBIT(bHab_Desp3) != 0)		//Programado para despertar
		{
			if(*Tab_Hora_Desp[3] == Hora && *Tab_Minuto_Desp[3] == Minuto)	//Ops, A hora bateu.
			SET_USERBIT(bDesp3_ON);		//Sinaliza para despertar Despertador 3
			SET_USERBIT(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = 30000/PERIODO_TIMER_1;		//30 segundos para parada automática
		}
	}
	/*------------------------------------------------------------------------
	*		Gerencia as piscadas dos leds dos despertadores durante o alarme
	*-----------------------------------------------------------------------*/
	if(Timer_Led_Alarme == 0 && TESTE_USERBIT(bAlarme_ON) != 0)
	{
		Timer_Led_Alarme = 100/PERIODO_TIMER_1;

		if(TESTE_USERBIT(bDesp1_ON) != 0)
		CPL_LED_DESP(Led_Desp_1);

		else if(TESTE_USERBIT(bDesp2_ON) != 0)
		CPL_LED_DESP(Led_Desp_2);

		else if(TESTE_USERBIT(bDesp3_ON) != 0)
		CPL_LED_DESP(Led_Desp_3);
	}
	/*------------------------------------------------------------------------
	*		    Toca o Buzzer no Despertador
	*-----------------------------------------------------------------------*/
	if(TESTE_USERBIT(bAlarme_ON) != 0 && TESTE_USERBIT(bSoneca) == 0)
	{
		if (Timer_Buzzer_Alarme == 0)
		{

			switch(Seq_Buzzer)
			{
				case 0:
				case 2:
				case 4:
				case 6:
				LIGA_BUZZER();
				Timer_Buzzer_Alarme = 50/PERIODO_TIMER_1;
				Seq_Buzzer++;
				break;
				//-----------------------------
				case 1:
				case 3:
				case 5:
				case 7:
				DESLIGA_BUZZER();
				Timer_Buzzer_Alarme = 100/PERIODO_TIMER_1;
				Seq_Buzzer++;
				break;

				case 8:
				LIGA_BUZZER();
				Timer_Buzzer_Alarme = 100/PERIODO_TIMER_1;
				Seq_Buzzer++;
				break;

				case 9:
				DESLIGA_BUZZER();
				Timer_Buzzer_Alarme = 600/PERIODO_TIMER_1;
				Seq_Buzzer++;
				break;

				default:
				Seq_Buzzer = 0;
			}

		}
	}
	/*------------------------------------------------------------------------
	*		Driver de controle dos displays de 7 segmentos
	*-----------------------------------------------------------------------*/
	if(	TESTE_USERBIT(bApaga_DSP) == 0 ||	//Sem sinalização para apagar o display
		TESTE_USERBIT(bAjuste) != 0) 	//Fazendo algum ajuste
	{
		if(TESTE_USERBIT(bFirst_Time) != 0)		//Relógio está consistente
		{
			switch (CTR_Display)
			{
				case 	CTR_Dez_Hora:

				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");

				PORTC_Img &= 0b11111100;	//00
				PORTC = PORTC_Img;

				PORTD =  Tab_Num[Display_Hora/10];
				CTR_Display = CTR_Unid_Hora;				//Próximo
				break;
			//--------------------------------------

				case 	CTR_Unid_Hora:

				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");

				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000001);	//Merge Bit0 e Bit1 modificados em PORTC
				PORTC = PORTC_Img;

				PORTD = Tab_Num[Display_Hora%10];
				CTR_Display = CTR_Dez_Min;				//Próximo
				break;
			//--------------------------------------
				case 	CTR_Dez_Min:

				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");

				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000010);		//10
				PORTC = PORTC_Img;

				PORTD = Tab_Num[Display_Minuto/10];
				CTR_Display = CTR_Unid_Min;				//Próximo
				break;
			//--------------------------------------
				case 	CTR_Unid_Min:

				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");

				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000011);	//11
				PORTC = PORTC_Img;

				PORTD = Tab_Num[Display_Minuto%10];
				CTR_Display = CTR_Dez_Hora;		//Próximo
				break;
			}
		}
		else		//Escreve Mensagem do Display
		{
			switch (CTR_Display)
			{
				case CTR_Dez_Hora:
				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				PORTC_Img &= 0b11111100;	//00
				PORTC = PORTC_Img;
				PORTD =  Let_E;
				CTR_Display = CTR_Unid_Hora;				//Próximo
				break;
				//-----------------
				case CTR_Unid_Hora:
				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000001);	//Merge Bit0 e Bit1 modificados em PORTC
				PORTC = PORTC_Img;
				PORTD = Let_R;
				CTR_Display = CTR_Dez_Min;				//Próximo
				break;
				//-----------------
				case CTR_Dez_Min:
				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000010);		//10
				PORTC = PORTC_Img;
				PORTD = Let_R;
				CTR_Display = CTR_Unid_Min;				//Próximo
				break;
				//-----------------
				case CTR_Unid_Min:
				PORTD = 0xFF;			//Apaga Tudo
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				PORTC_Img &= (0b11111100);
				PORTC_Img ^= (0b00000011);	//11
				PORTC = PORTC_Img;
				PORTD = num_0;
				CTR_Display = CTR_Dez_Hora;		//Próximo
				break;
			}
		}
	}
	else
	PORTD = 0xFF;			//Apaga Tudo


//	PORTB &= ~(1 << PB3);			//TESTE
}
/*=========================================================================
 * Configura o Timer 1 Compare Mode
 *----------------------------------------------------------------------*/
void Configura_Timer_1_Modo_CTC()
{

	TCCR1A = 0;                				//Configura timer para operação normal
	TCCR1B = 0;                				//Limpa registrador
	TCNT1  = 0;                				//Zera temporizador

#ifdef CLKXTAL_1840000Hz

	OCR1A = 0x008F;            				//Carrega os registradores de comparação (5ms 1843200Hz/1; Prescaler 64)
	TCNT1 = 0xFF70;
#endif // CLKXTAL_1840000Hz

#ifdef CLKXTAL_230400Hz
	OCR1A = 0x0011;            			   //Carrega os registradores de comparação (230400Hz 1843200Hz/8; 5ms; Prescaler 64)
	TCNT1 = 0xFFEE;
#endif // CLKXTAL_230400Hz

	TCCR1B |= (1 << WGM12)|(1<<CS10)|(1 << CS11);   // modo CTC, prescaler de 64: CS11 = 1 e CS10 = 1
	TIMSK1 |= (1 << OCIE1A);  						// habilita interrupção por igualdade de comparação

}
/**========================================================================
 * Rotina:	Incrementa_Modo_BCD
 * Função:	Incrementa um número no modo BCD
 * Chamada: Incrementa_Modo_BCD(<data>)
 * Retorna: O número incrementado no modo BCD
 * Comentário:Cada nibble do numero a ser incrementado não pode ser maior
 *			  do que 9.Portanto qualquer número em hexadecimal é aceito
 *			  desde que não contenha os algarismos letras.
 * ----------------------------------------------------------------------*/
unsigned char Incrementa_Modo_BCD(unsigned char Numero_BCD)
{
	unsigned char Unidade_Num;
	unsigned char Dezena_Num;

	Numero_BCD++;		//Conta
	Unidade_Num = Numero_BCD & 0x0F;		//Pega a Unidade
	Dezena_Num = Numero_BCD & 0xF0;			//Separa a dezena
	Dezena_Num = (Dezena_Num >> 4);

	if(Unidade_Num >= 0x0A)				//É 10 ou maior?
	{
		Unidade_Num = 0;					//Corrrige para BCD
		if(++Dezena_Num >= 0x0A)			//É 10 ou maior?
		Dezena_Num = 0;						//Corrige para BCD
		else
		Dezena_Num = (Dezena_Num << 4);

		Numero_BCD = Dezena_Num ^ Unidade_Num;			//Junta os dois
	}

	return (Numero_BCD);
}
 /**========================================================================
 * Rotina:	Contador_Relogio
 * Função:	Conta como um relogio
 * Chamada:	Contador Relogio()
 * Retorna:	Nada
 * Comentário:Nenhum
 * ----------------------------------------------------------------------*/
void Contador_Relogio()
{
	if(++Segundo == 0x60)
	{
		Segundo = 0;
		Minuto = Incrementa_Modo_BCD(Minuto);
		if(Minuto == 0x60)
		{
			Minuto = 0;
			Hora = Incrementa_Modo_BCD(Hora);
			if(Hora == 0x24)
			Hora = 0;
		}
	}
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
