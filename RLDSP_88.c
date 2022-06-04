/*==============================================================
 * 			PROJETO: Relógio Despertador com 3 opções
 * 			DATA:Maio/2022
 *			IDE: Codeblocks 20.03
			Compilador: AVR-GCC
			PROCSSADOR: ATMEGA88P
			FREQUÊNCIA DO SISTEMA: 1843200Hz/1
			CONF. FUSES: L:0xF7 H:DF Ext:0xFF (Ver Arquivo Config_Fuse.pdf)
 * 			AUTOR: José Carlos
 			comentários:
 * ------------------------------------------------------------*/
#define Mod_Main
#include "Mod_Def.h"

//---------------------------------------------------------------
//Tabela da sequência da máquina de estado
const unsigned char Tab_Seq_Mq_Estado[]=
{
    Stat_Mon_Power,
    Stat_Mon_Bat,
    Stat_Chaves,
    Stat_Despertador,
    Stat_Soneca,
 };
/*------------------------------------------------------------*/
int main ()
{
	/*-------------------------------------------
			Configurando o hardware
	-------------------------------------------*/

	//Configurando Pórticos
	//PORTB
	DDRB = 0b11111000;		//wwwwwrrr

	//PORTC
	DDRC = 0b11001111;		//wwrrwwww

	//PORTD
	DDRD = 0b01111111;		//rwwwwwww

	/*------------------------------------------*/
	PORTC_Img = PORTC;
	Hora = 0;
	Minuto = 0;
	Segundo = 0;
	Hora_Desp_1 = 0;
	Hora_Desp_2 = 0;
	Hora_Desp_3 = 0;
	Minuto_Desp_1 = 0;
	Minuto_Desp_2 = 0;
	Minuto_Desp_3 = 0;
	pTabHoraMinuto = 0;
	pTabHoraMinuto_bkp = 0;
	Seq_Buzzer = 0;
	Timer_Buzzer_Alarme = 0;
	CTR_Display = CTR_Unid_Hora;		//Começa pelo display da hora
	VL_Ch = 0xFF;				//Limpa o buffer das Chaves
	Estado = 0;
	AddresBit = 0;				//Reseta todos os bits
	Real_Timer = 1000/PERIODO_TIMER_1;
	Timer_Led_Seg = 0;
	Timer_Led_Desp = 0;
	Timer_Mon_Energia = 0;
	Timer_SW_ACIONADA = T_SW_ACIONADA;		//Em milissegundos
	Timer_Over_ADJ = T_OVER_ADJ;				//Carrega o buffer
	Timer_Wait = 0;
	Timer_Over_Despertador = 0;
	Pont_Seq_Mq_Estado = 0;			//Define a primeira passada pela máquina
	Config_ADC();					//Configura o ADC
	Configura_Timer_1_Modo_CTC();
	sei();						//Habilita todas as interrupções

	Buzzer_On(2,50,100);

	//-------------------------

#define	LEIT_ADC_DISPLAY()	\
	SET_USERBIT(bFirst_Time);\
	Le_ADC(Mon_Bat);\
	Hora = Leitura_ADC/100;\
	Minuto = Leitura_ADC%100;\
	Wait(1000);\
	//-------------------------

	while (1)
	{
		//Estado = Tab_Seq_Mq_Estado[Pont_Seq_Mq_Estado];		//Pega a tarefa

		switch (Estado)
		{
			case	Stat_Mon_Power:				//Monitora se tem tensão da fonte
			if(Timer_Mon_Energia == 0)
			{
				Le_ADC(Mon_AC);

				if (Leitura_ADC < (POWER_FAIL) && TESTE_USERBIT(bPower_Fail) == 0)	//Tensão da rede caiu e não sinalizou
				SET_USERBIT(bPower_Fail);				//Sinaliza

				else if(Leitura_ADC > (POWER_FAIL + VHISTERESE) && TESTE_USERBIT(bPower_Fail) != 0)	//Tensão da rede está boa e está sinalizando ruim
				CLR_USERBIT(bPower_Fail);
			}
			//Pont_Seq_Mq_Estado++;
			Estado = Stat_Mon_Bat;
			break;
		//-------------------------------------
			case	Stat_Mon_Bat:				//Verifica a tensão da bateria
			if(Timer_Mon_Energia == 0)
			{
				Timer_Mon_Energia = 500/PERIODO_TIMER_1;		//Verifica as tensões de 1/2 em 1/2 segundo

				Le_ADC(Mon_Bat);

				if(Leitura_ADC <= (NEAR_LOW_BAT) && (Leitura_ADC - VHISTERESE > LOW_BAT) && (TESTE_USERBIT(bNear_Low_Bat))== 0)	//Bateria está perto de estar baixa
				{
					SET_USERBIT(bNear_Low_Bat);				//Sinaliza que a bateria está perto de estar baixa
					CLR_USERBIT(bLow_Bat);					//Sinaliza
				}
				else if(Leitura_ADC <= (LOW_BAT) && TESTE_USERBIT(bLow_Bat) == 0)	//Bateria está ruim e aina não sinalizou
				{
					SET_USERBIT(bLow_Bat);					//Sinaliza
					CLR_USERBIT(bNear_Low_Bat);				//Limpa o flag de bateria quase ruim
				}
				else if (Leitura_ADC > (NEAR_LOW_BAT + VHISTERESE) && (TESTE_USERBIT(bNear_Low_Bat) != 0 || TESTE_USERBIT(bLow_Bat) != 0))	//Bat Ok, mas alguém está sinalizando ruim
				{
					CLR_USERBIT(bNear_Low_Bat);				//Limpa Sinalização que a bateria está perto de estar baixa
					CLR_USERBIT(bLow_Bat);					//Idem Bateria baixa
				}
			}
			//Pont_Seq_Mq_Estado++;
			Estado = Stat_Chaves;
			break;
		//------------------------------------------
			case	Stat_Chaves:

			if(TESTE_USERBIT(bAlarme_ON) != 0)		//se estiver despertando...
			Estado = Stat_Despertador;				//...trata o despertador
			else
			{
				if(LE_SW_SEL_DESP() == 0)
				{
					Wait(20);		//Bouncing
					if(LE_SW_SEL_DESP() == 0)
					{
						while(LE_SW_SEL_DESP() == 0)	//Espera soltar a chave
						{}
						pTabHoraMinuto = pTabHoraMinuto_bkp;		//Pega a próxima sequênciq
						VL_Ch = 0xFF;
						SET_USERBIT(bFirst_Time);		//HAbita mostrar o horário no display;
						SET_USERBIT(bAjuste);			//Sinaliza que está ajustando
						Display_Hora = *Tab_Hora_Desp[pTabHoraMinuto];
						Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto];
						//---------------------------------------------------
						//Indo para próxima sequência da tabela de horários (Guarda para a próxima passada)
						//---------------------------------------------------
						pTabHoraMinuto_bkp++;
						if(	Tab_Hora_Desp[pTabHoraMinuto_bkp] == &Fim_Tab_Hora ||
							Tab_Hora_Desp[pTabHoraMinuto_bkp] == &Fim_Tab_Minuto)
						pTabHoraMinuto_bkp = 0;
						CARREGA_TIMER_OVER_ADJ();
						//---------------------------------------------------
					}
				}
				//---------------------
				else if(LE_SW_HAB_DESP() == 0)
				{
					Wait(20);	//Bouncing
					if(LE_SW_HAB_DESP() == 0)
					{
						while(LE_SW_HAB_DESP() == 0)	//Espera soltar a chave
						{}

						if(pTabHoraMinuto == 1)
						CPL_USERBIT(bHab_Desp1);


						else if(pTabHoraMinuto == 2)
						CPL_USERBIT(bHab_Desp2);

						else if(pTabHoraMinuto == 3)
						CPL_USERBIT(bHab_Desp3);

						CARREGA_TIMER_OVER_ADJ();
					}
				}
				//----------------------
				else if(LE_SW_SEL_HORA() == 0 && TESTE_USERBIT(bAjuste) != 0)		//Só executa se estiver no modo ajuste)
				{
					Wait(20); //Bouncing
					if(LE_SW_SEL_HORA() == 0)
					{
						Display_Hora = *Tab_Hora_Desp[pTabHoraMinuto];	//Pega Hora correspondente e coloca no display
						Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto]; //idem para Minuto
						Timer_SW_ACIONADA = T_SW_ACIONADA;		//Carrega o temporizador da chave
						while(Timer_SW_ACIONADA > 0 && LE_SW_SEL_HORA() == 0)
						{}
						//---------------------------------------------
						//		Avanço Automático da Hora
						//---------------------------------------------
						if(Timer_SW_ACIONADA == 0)
						{
							while (LE_SW_SEL_HORA() == 0)
							{
								*Tab_Hora_Desp[pTabHoraMinuto] += 1;
								if(*Tab_Hora_Desp[pTabHoraMinuto] == 24)
								*Tab_Hora_Desp[pTabHoraMinuto] = 0;
								Display_Hora = *Tab_Hora_Desp[pTabHoraMinuto];
								CARREGA_TIMER_OVER_ADJ();
								Wait(200);
							}
						}

						//---------------------------------------------
						else
						{
							*Tab_Hora_Desp[pTabHoraMinuto] += 1;
							if(*Tab_Hora_Desp[pTabHoraMinuto] == 24)
							*Tab_Hora_Desp[pTabHoraMinuto] = 0;
							Display_Hora = *Tab_Hora_Desp[pTabHoraMinuto];
							CARREGA_TIMER_OVER_ADJ();

						}
						Segundo = 55;		//TESTE
						Habilita_Desp();		//Habilita o Despertador Correspondente
					}
				}
				//----------------------
				else if(LE_SW_SEL_MINUTO() == 0 && TESTE_USERBIT(bAjuste) != 0)		//Só executa se estiver no modo ajuste
				{
					Wait(20); //Bouncing
					if(LE_SW_SEL_MINUTO() == 0)
					{
						Display_Hora = *Tab_Hora_Desp[pTabHoraMinuto];	//Pega Hora correspondente e coloca no display
						Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto]; //idem para Minuto

						Timer_SW_ACIONADA = T_SW_ACIONADA;		//Carrega o temporizador da chave
						while(Timer_SW_ACIONADA > 0 && LE_SW_SEL_MINUTO() == 0)
						{}
						//---------------------------------------------
						//		Avanço Automático do Minuto
						//---------------------------------------------
						if(Timer_SW_ACIONADA == 0)
						{
							while (LE_SW_SEL_MINUTO() == 0)
							{
								*Tab_Minuto_Desp[pTabHoraMinuto] += 1;
								if(*Tab_Minuto_Desp[pTabHoraMinuto] == 60)
								*Tab_Minuto_Desp[pTabHoraMinuto] = 0;
								Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto];
								CARREGA_TIMER_OVER_ADJ();
								Wait(200);
							}
						}
						//---------------------------------------------
						else
						{
							*Tab_Minuto_Desp[pTabHoraMinuto] += 1;
							if(*Tab_Minuto_Desp[pTabHoraMinuto] == 60)
							*Tab_Minuto_Desp[pTabHoraMinuto] = 0;
							Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto];
							CARREGA_TIMER_OVER_ADJ();
						}
						Segundo = 55;		//TESTE
						Habilita_Desp();	//Habilita o Despertador Correspondente
					}
				}

				//----------------------
				else if(TESTE_USERBIT(bAjuste)!=0 && Timer_Over_ADJ == 0)		//Está no modo ajuste, O tempo encerrou
				{
					CLR_USERBIT(bAjuste);
					Segundo = 55;		//TESTE		//Recomeça a contagem
					pTabHoraMinuto_bkp = 0;
					pTabHoraMinuto = 0;
				}
				//Pont_Seq_Mq_Estado = 0;
				Estado = Stat_Mon_Power;
			}
			break;
			//----------------------
			//Trata o despertador
			//----------------------
			case	Stat_Despertador:

			if(((PINB & 0b00000111) != 0b00000111) || (PIND & 0b10000000) == 0)	//Tem chave acionada?
			{
				Wait(50);		//Espera o bouncing

				while ((PIND & 0b00000111) != 0b00000111 || (PIND & 0b10000000) == 0)
				{}		//Espera soltar a Chave

				CLR_USERBIT(bAlarme_ON);
				Timer_Buzzer_Alarme = 0;
				Timer_Over_Despertador = 0;

				CPL_LED_DESP(Led_Desp_3);
			}
			else if( Timer_Over_Despertador == 0)
			{
				CLR_USERBIT(bAlarme_ON);
				Timer_Buzzer_Alarme = 0;
			}
			Estado = Stat_Mon_Power;
			break;
		}
	}
}
/*========================================================================
					+++ ROTINAS AUXILIARES +++
 ========================================================================*/
/* Rotina:  Buzzer_On()
 * Função:  Aciona o Buzzer por um número de vezes e por tempo pré definido
 * Chamada: Buzzer_On(<Tempo de Ligado>,<Numero de vezes que repete>,<Intervalo entre os beeps>)
 * Retorna: Nada
 * Comentário: Exemplos:
 * 				Acionar o buzzer 1 vez por 50ms sem repetição:
 * 				Buzzer_On(1,50,0);
 *
 * 				Acionar o buzzer 2 com 2 beeps de 50ms com intervalo de 100ms
 * 				Buzzer_On(2,50,100);
 * ----------------------------------------------------------------------*/
 void Buzzer_On(unsigned char n_beeps,uint16_t t_beep,uint16_t t_intervalo_beep)
{
	do
	{
		SET_PORT_BIT(PORTC_Img,Buzzer);		//Liga o Buzzer
		PORTC = PORTC_Img;
		Wait(t_beep);
		CLR_PORT_BIT(PORTC_Img,Buzzer);		//Desliga o buzzer
		PORTC = PORTC_Img;
		Wait(t_intervalo_beep);		//O tempo de intervalo
		n_beeps--;						//Acabou?
	} while(n_beeps > 0);
}
 /**========================================================================
 * Rotina: Sai_Modo_Ajuste
 * Função:	Sai do modo ajuste. Deixa o relógio no modo Hora
 * Chamada:	Sai_Modo_Ajuste()
 * Retorna: Nada
 * Comentário:N/A
 * ----------------------------------------------------------------------*/
void Sai_Modo_Ajuste()
{
	CLR_USERBIT(bAjuste);
	Segundo = 0;				//Recomeça a contagem
	pTabHoraMinuto_bkp = 0;
	pTabHoraMinuto = 0;
}

 /**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void Habilita_Desp()
{
	if(pTabHoraMinuto == 1)
	SET_USERBIT(bHab_Desp1);

	else if(pTabHoraMinuto == 2)
	SET_USERBIT(bHab_Desp2);

	else if(pTabHoraMinuto == 3)
	SET_USERBIT(bHab_Desp3);
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
