/*===============================================================================

				+++ MÓDULO DE MANIPULAÇÃO DO TIMER 1 +++

===============================================================================*/
#define Mod_Timer
#include "Mod_Def.h"


//------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)          // interrupção por igualdade de comparação no TIMER1 Atmega88
{
uint8_t AuxReg8;
uint8_t PORTD_Img;

	//	Set_Port_Bit(Led_Desp_3);
/*--------------------------------------------------------------
			Temporiza o relógio
--------------------------------------------------------------*/
	if(--Real_Timer == 0)			//É a cadência do tempo de 1 em 1 segundo
	{
		Set_Userbit(bToggleSegundo);			//Sinaliza que virou o segundo
		Timer_500ms = 500/PERIODO_TIMER_1;
		Real_Timer = 1000/PERIODO_TIMER_1;				//Carrega com 1 segundo

		/*--------------------------------------------
			Temporiza o tempo que uma das chaves deve ficar acionada para sair da soneca
		--------------------------------------------*/
		if(Timer_Ch_Saida_Soneca > 0)
		Timer_Ch_Saida_Soneca--;
		/*-----------------------------------------
			Temporiza parada automática do despertador
		--------------------------------------------*/
		if(Timer_Over_Despertador > 0)
		Timer_Over_Despertador--;
		/*-----------------------------------------
			Temporiza a saída automática do modo ajuste
		--------------------------------------------*/
		if(Timer_Over_ADJ > 0)
		Timer_Over_ADJ--;
		/**------------------------------------------´
		Temporiza a Soneca
		--------------------------------------------*/
		if(Timer_Soneca > 0)
		Timer_Soneca--;
	}
	//Mantem o bit sinanlizando por 500ms
	else if(Teste_Userbit(bToggleSegundo) != 0)
	{
		if(Timer_500ms > 0)
		Timer_500ms--;
		else
		Clr_Userbit(bToggleSegundo);
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

	/*------------------------------------------------------------------------
	*		Driver de controle dos displays de 7 segmentos
	*		Os registros Display_Hora e Display_Minuto devem
	*		estar em formato decimal
	*		Exemplo: Display_Hora = 15 -> 15 Horas
	*				 Dispĺay_Minuto = 50 -> 50 minutos
	*-----------------------------------------------------------------------*/
	if(Teste_Userbit(bPWR_Fail) == 0)
	{
		switch (CTR_Display)
		{
			case 	CTR_Dez_Hora:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);
			AuxReg8 = Display_Hora/10;
			if(AuxReg8 == 0)	//Se a dezena de hora for zero,...
			PORTD_Img = 0xFF;
			else
			PORTD_Img = Tab_Num[AuxReg8];

			Clr_Userbit(bA0_HC139_Img);
			Clr_Userbit(bA1_HC139_Img);

			PORTC = PORTC_Img;
			PORTD = PORTD_Img;

			CTR_Display = CTR_Unid_Hora;				//Próximo
			break;
		//--------------------------------------
			case 	CTR_Unid_Hora:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);

			AuxReg8 = Display_Hora%10;
			PORTD_Img = Tab_Num[AuxReg8];

			Set_Userbit(bA0_HC139_Img);
			Clr_Userbit(bA1_HC139_Img);

			PORTC = PORTC_Img;
			PORTD = PORTD_Img;

			CTR_Display = CTR_Dez_Min;				//Próximo
			break;
		//--------------------------------------
			case 	CTR_Dez_Min:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);

			AuxReg8 = Display_Minuto/10;
			PORTD_Img = Tab_Num[AuxReg8];

			Clr_Userbit(bA0_HC139_Img);
			Set_Userbit(bA1_HC139_Img);

			PORTC = PORTC_Img;
			PORTD = PORTD_Img;

			CTR_Display = CTR_Unid_Min;				//Próximo
			break;
		//--------------------------------------
			case 	CTR_Unid_Min:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);

			AuxReg8 = Display_Minuto %10;
			PORTD_Img = Tab_Num[AuxReg8];

			Set_Userbit(bA0_HC139_Img);
			Set_Userbit(bA1_HC139_Img);

			PORTC = PORTC_Img;
			PORTD = PORTD_Img;

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
			_delay_us(10);

			Clr_Userbit(bA0_HC139_Img);
			Clr_Userbit(bA1_HC139_Img);
			PORTC = PORTC_Img;
			PORTD =  Let_E;

			CTR_Display = CTR_Unid_Hora;				//Próximo
			break;
			//-----------------
			case CTR_Unid_Hora:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);
			Set_Userbit(bA0_HC139_Img);
			Clr_Userbit(bA1_HC139_Img);
			PORTC = PORTC_Img;
			PORTD = Let_R;

			CTR_Display = CTR_Dez_Min;				//Próximo
			break;
			//-----------------
			case CTR_Dez_Min:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);

			Clr_Userbit(bA0_HC139_Img);
			Set_Userbit(bA1_HC139_Img);
			PORTC = PORTC_Img;
			PORTD = Let_R;

			CTR_Display = CTR_Unid_Min;				//Próximo
			break;
			//-----------------
			case CTR_Unid_Min:

			PORTD = 0xFF;			//Apaga Tudo
			_delay_us(10);

			Set_Userbit(bA0_HC139_Img);
			Set_Userbit(bA1_HC139_Img);
			PORTC = PORTC_Img;
			PORTD = num_0;

			CTR_Display = CTR_Dez_Hora;		//Próximo
			break;
		}
	}
	/*-----------------------------------------
	Temporiza as piscadas do Led Segundo
	--------------------------------------------*/
/*	if(Teste_Userbit(bAjuste) == 0)
	{
		if(Timer_Led_Seg > 0)
		Timer_Led_Seg--;

		if(Timer_Led_Seg == 0)
		{
			CPL_LED_SEG();
			Timer_Led_Seg = 500/PERIODO_TIMER_1;
		}
	}
*/
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
	/*--------------------------------------------------------------------------
						Hora de Despertar?
	--------------------------------------------------------------------------*/
	//Não deixa habilitar despertador durante um ajuste e durante despertador ativo
	// Segundo = 0 previne repetir ativação dentro mesmo minuto
	if((Teste_Userbit(bAjuste) | Teste_Userbit(bAlarme_ON)) == 0 && Segundo == 00)
	{

		if(Teste_Userbit(bHab_Desp1) != 0 && *Tab_Hora_Desp[1] == Hora && *Tab_Minuto_Desp[1] == Display_Minuto)		//Programado para despertar
		{
			Set_Userbit(bDesp1_ON);		//Sinaliza para despertar Despertador 1
			Clr_Userbit(bDesp2_ON);
			Clr_Userbit(bDesp3_ON);
			Set_Userbit(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = MAX_TEMPO_DESPERTADOR;

		}
		else if(Teste_Userbit(bHab_Desp2) != 0 && *Tab_Hora_Desp[2] == Hora && *Tab_Minuto_Desp[2] == Display_Minuto)		//Programado para despertar
		{
			Clr_Userbit(bDesp1_ON);
			Set_Userbit(bDesp2_ON);		//Sinaliza para despertar Despertador 2
			Clr_Userbit(bDesp3_ON);
			Set_Userbit(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = MAX_TEMPO_DESPERTADOR;
		}
		else if(Teste_Userbit(bHab_Desp3) != 0 && *Tab_Hora_Desp[3] == Hora && *Tab_Minuto_Desp[3] == Display_Minuto)		//Programado para despertar
		{
			Clr_Userbit(bDesp1_ON);
			Clr_Userbit(bDesp2_ON);
			Set_Userbit(bDesp3_ON);		//Sinaliza para despertar Despertador 3
			Set_Userbit(bAlarme_ON);	//Sinaliza que está despertando
			Timer_Over_Despertador = MAX_TEMPO_DESPERTADOR;
		}
	}
	/*-----------------------------------------------------------------------
	Temporiza as piscadas dos leds dos despertadores no modo ajuste ou quando estiver despertando
	-------------------------------------------------------------------------*/
	if(Teste_Userbit(bAjuste)!= 0 || Teste_Userbit(bAlarme_ON) != 0 || Teste_Userbit(bSoneca) != 0 || Timer_Over_ADJ > 0)
	{
		if(Timer_Led_Desp == 0)					//Temporiza as piscadas dos leds no ajuste
		{
			Timer_Led_Desp = 100/PERIODO_TIMER_1;	//Recarrega o Timer

			if(pTabHoraMinuto == 1  || (Teste_Userbit(bDesp1_ON) != 0)) //Está ajustando o Despertador 1 ou despertando
			CPL_LED_DESP(Led_Desp_1);

			else if(pTabHoraMinuto == 2 || (Teste_Userbit(bDesp2_ON) != 0))	//Está ajustando o Despertador 2 ou despertando
			CPL_LED_DESP(Led_Desp_2);

			else if(pTabHoraMinuto == 3 || (Teste_Userbit(bDesp3_ON) != 0 ))//Está ajustando o Despertador 3 ou despertando
			CPL_LED_DESP(Led_Desp_3);

			else if(pTabHoraMinuto == 0/* && Teste_Userbit(bAlarme_ON) == 0*/)		//Está ajustando a Hora e não está despertando
			CPL_LED_SEG();		//Pisca o led do segundo

		}
		/*-----------------------------------------------------------------------
		Gerencia  sinalização do Led Segundo para os despertadores habilitados ou não
		durante o ajuste. Aceso se despertador estiver habilitado
		-------------------------------------------------------------------------*/
		if(pTabHoraMinuto != 0 && Teste_Userbit(bAlarme_ON) == 0 && Teste_Userbit(bSoneca) == 0) //Não está ajustando a hora
		{
			if(pTabHoraMinuto == 1)
			{
				if(Teste_Userbit(bHab_Desp1) != 0)	//	//Despertador Habilitado

				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO();
			}

			if(pTabHoraMinuto == 2)
			{
				if(Teste_Userbit(bHab_Desp2) != 0)	//	//Despertador Habilitado
				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO();
			}
			if(pTabHoraMinuto == 3)	//	//Despertador Habilitado
			{
				if(Teste_Userbit(bHab_Desp3) != 0)
				ACENDE_LED_SEGUNDO()
				else
				APAGA_LED_SEGUNDO();
			}
		}
	}
	/*-----------------------------------------------------------------------
	Gerencia  sinalização dos leds dos despertadores
	Aceso se despertador estiver habilitado
	------------------------------------------------------------------------*/
	if(Teste_Userbit(bAlarme_ON) == 0 && Teste_Userbit(bSoneca) == 0)
	{

		if((Teste_Userbit(bAjuste) != 0 && pTabHoraMinuto != 1) || Teste_Userbit(bAjuste) == 0)
		{
			if(Teste_Userbit(bHab_Desp1) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_1);
			else
			APAGA_LED_DESP(Led_Desp_1);
		}
		//----------------------------------------------------------------
		if((Teste_Userbit(bAjuste) != 0 && pTabHoraMinuto  != 2) || Teste_Userbit(bAjuste) == 0)
		{
			if(Teste_Userbit(bHab_Desp2) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_2);
			else
			APAGA_LED_DESP(Led_Desp_2);
		}
		//----------------------------------------------------------------
		if((Teste_Userbit(bAjuste) != 0 && pTabHoraMinuto != 3) || Teste_Userbit(bAjuste) == 0)
		{
			if(Teste_Userbit(bHab_Desp3) != 0)	//Despertador habilitado
			ACENDE_LED_DESP(Led_Desp_3);
			else
			APAGA_LED_DESP(Led_Desp_3);
		}
	}
	/*------------------------------------------------------------------------
	*		    Toca o Buzzer no Despertador
	*-----------------------------------------------------------------------*/
	if(Teste_Userbit(bAlarme_ON) != 0)
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
}
/*=========================================================================
 * Configura o Timer 1 Compare Mode
 *----------------------------------------------------------------------*/
void Configura_Timer_1_Modo_CTC()
{

	TCCR1A = 0;                				//Configura timer para operação normal
	TCCR1B = 0;                				//Limpa registrador
	TCNT1  = 0;                				//Zera temporizador

// Estrutura condicional para seleção de clock
#if defined(F_CPU) && (F_CPU == 1843200UL)
    // Configuração para 1.8432 MHz
    // 5ms com Prescaler 64: (1843200 / 64) * 0.005 = 144 (0x0090)
    OCR1A = 0x008F;            // 144 - 1 = 143 (0x8F) para modo CTC
    TCNT1 = 0xFF70;            // Offset para estouro (se não usar CTC)

#elif defined(F_CPU) && (F_CPU == 230400UL)
    // Configuração para 230.4 kHz
    // 5ms com Prescaler 64: (230400 / 64) * 0.005 = 18 (0x0012)
    OCR1A = 0x0011;            // 18 - 1 = 17 (0x11)
    TCNT1 = 0xFFEE;

#else
    #warning "Frequencia de CPU nao suportada ou nao definida para os cálculos do Timer1"
#endif

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
uint8_t Incrementa_Modo_BCD(uint8_t Numero_BCD)
{
	uint8_t Unidade_Num;
	uint8_t Dezena_Num;

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
