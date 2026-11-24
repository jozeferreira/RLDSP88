/*================================================================================
  	PROJETO: Relógio Despertador com 3 opções
  	DATA:Maio/2022
 	IDE: Codeblocks 20.03
	Compilador: AVR-GCC
	PROCSSADOR: ATMEGA88P
	FREQUÊNCIA DO SISTEMA: 1843200Hz/1
	CONF. FUSES: L:0xF7 H:D9 Ext:0xFF
	================================================================================
	CONFIGURAÇÃO DE FUSES - ATMEGA328P (Sem Bootloader / SPI Habilitado)
	================================================================================
	FUSE      VALOR    DESCRIÇÃO EM PORTUGUÊS
	--------------------------------------------------------------------------------
	LOW       0xF7     	Oscilador a Cristal Externo (Full Swing).
						Sem divisão de clock por 8 (roda na velocidade real).

	HIGH      0xD9     	Programação via SPI Habilitada (Essencial para gravar).
						Vetor de Reset de Boot DESATIVADO.
						O programa inicia instantaneamente no endereço 0x0000.

	EXTENDED  0xFF     	Detector de Subtensão (Brown-out) DESATIVADO.

	EXEMPLO DE COMANDO AVRDUDE (Via USBasp):
	avrdude -c usbasp -p m328p -U lfuse:w:0xf7:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
	--------------------------------------------------------------------------------
  	AUTOR: José Carlos
 	comentários:
  ------------------------------------------------------------*/
#define Mod_Main
#include "Mod_Def.h"
#include "soft_serial.h"
/**---------------------------------------------------------------
		USE PARA TESTES
---------------------------------------------------------------*/
#define TESTE
//================================================================

unsigned int Timer_SW_ACIONADA_Local;

//---------------------------------------------------------------
//Tabela da sequência da máquina de estado
__flash const uint8_t Tab_Seq_Mq_Estado[]=
{
	Stat_Atualiza_Rel,
    Stat_Chaves,
    Stat_Despertador,
    Stat_Soneca,
    FIM
};
#define COL_HORA_DESP		0
#define COL_MINUTO_DESP		1
#define COL_EEPHORA_DESP	2
#define COL_EEPMINUTO_DESP	3

// Tabela na FLASH: 3 linhas, 2 colunas (armazenando o ENDEREÇO das variáveis)
__flash const uint16_t Tab_Desp[][4] =
{
    {0,0,0,0},
    {(uint16_t)&Hora_Desp_1, (uint16_t)&Minuto_Desp_1, EEP_HORA_DESP_1, EEP_MINUTO_DESP_1},
    {(uint16_t)&Hora_Desp_2, (uint16_t)&Minuto_Desp_2, EEP_HORA_DESP_2, EEP_MINUTO_DESP_2},
    {(uint16_t)&Hora_Desp_3, (uint16_t)&Minuto_Desp_3, EEP_HORA_DESP_3, EEP_MINUTO_DESP_3}
};
/*------------------------------------------------------------*/
int main ()
{
	/*-------------------------------------------
			Configurando o hardware
	-------------------------------------------*/
	Config_Port_Bit(SW_Hab_Desp,INPUT);
	Config_Port_Bit(SW_Sel_Hora,INPUT);
	Config_Port_Bit(SW_Sel_Minuto,INPUT);
	Config_Port_Bit(SW_Sel_Desp,INPUT);
	Config_Port_Bit(Led_Desp_1,OUTPUT);
	Config_Port_Bit(Led_Desp_2,OUTPUT);
	Config_Port_Bit(Led_Desp_3,OUTPUT);
	Config_Port_Bit(Buzzer,OUTPUT);
	Config_Port_Bit(A0_HC139,OUTPUT);
	Config_Port_Bit(A1_HC139,OUTPUT);
	Config_Port_Bit(Led_Seg,OUTPUT);
	//Config_Port_Bit(SDA,OUTPUT);
	//Config_Port_Bit(SCL,OUTPUT);

	TWI_Init();		//Configura o I2C nativo do uP

	//PORTD
	DDRD = 0b01111111;		//rwwwwwww

	/*------------------------------------------*/
	PORTC_Img = PORTC;
	Hora = 0;
	Minuto = 0;
	Segundo = 0;
	pTabHoraMinuto = 0;
	pTabHoraMinuto_bkp = 0;
	Seq_Buzzer = 0;
	Timer_Buzzer_Alarme = 0;
	CTR_Display = CTR_Unid_Hora;		//Começa pelo display da hora
	VL_Ch = 0xFF;				//Limpa o buffer das Chaves
	Estado = 0;
/**---------------------------
	Limpa a área de bits
------------------------------*/
	AddresBit_0 = 0;
	AddresBit_1 = 0;
	AddresBit_2 = 0;
	Set_Userbit(bForceAtlDSP);		//Força atualizar o display
	Set_Userbit(bPWR_Fail);				//Assume que perdeu a ram
//------------------------------
	Real_Timer = 0;
	Timer_Led_Seg = 0;
	Timer_Led_Desp = 0;
	Timer_Mon_Energia = 0;
	Timer_SW_ACIONADA = T_SW_ACIONADA;		//Em milissegundos
	Timer_Over_ADJ = 0;
	Timer_Wait = 0;
	Timer_Over_Despertador = 0;
	Display_Hora = 0;
	Display_Minuto = 0;
	Pont_Seq_Mq_Estado = 0;			//Define a primeira passada pela máquina
	Config_ADC();					//Configura o ADC
	Configura_Timer_1_Modo_CTC();
	sei();						//Habilita todas as interrupções

	Buzzer_On(2,50,100);
	VL_Chaves = CHOFF;
		/**---------------------------------------------------------------
			Verifica se EEPROM já foi inicializada.
	---------------------------------------------------------------*/
	if(LE_EEPROM(EEP_ASSINATURA_1) != 0x55 || LE_EEPROM(EEP_ASSINATURA_2) != 0xAA)
	{
		//----------
		//EEprom corrompida ou nova.
		//----------
		GRAVA_EEPROM(EEP_ASSINATURA_1,0x55);	//Grava Assinatura
		GRAVA_EEPROM(EEP_ASSINATURA_2,0xAA);
		//Zera os horários
		GRAVA_EEPROM(EEP_HORA_DESP_1,0);
		GRAVA_EEPROM(EEP_HORA_DESP_2,0);
		GRAVA_EEPROM(EEP_HORA_DESP_3,0);
		GRAVA_EEPROM(EEP_MINUTO_DESP_1,0);
		GRAVA_EEPROM(EEP_MINUTO_DESP_2,0);
		GRAVA_EEPROM(EEP_MINUTO_DESP_3,0);
		//Zera os alarmes
		GRAVA_EEPROM(EEP_ALARME_HAB_1,0);
		GRAVA_EEPROM(EEP_ALARME_HAB_2,0);
		GRAVA_EEPROM(EEP_ALARME_HAB_3,0);

	}
	/**---------------------------------------------------------------
			Verifica consistência do relógio RTC
	---------------------------------------------------------------*/
	if(TWI_RTC_Read(RTC_CTR_REG) == 0)  //3231  Está no valor programado default
	Clr_Userbit(bPWR_Fail);		//limpa o flag de perda de ram
	else
	{
		//---------------------
		// Relógio se perdeu provavelmente por falta de bateria. Dados corrompidos
		//---------------------
		Set_Userbit(bPWR_Fail);

		TWI_RTC_Write(RTC_Hora,0x00);
		TWI_RTC_Write(RTC_Minuto,0x00);
		TWI_RTC_Write(RTC_Segundo,0x00);
	}
	//---------------------------------------------------------------
	// Força EOSC=0 (oscilador sempre ativo, mesmo em VBAT) a cada boot,
	// em vez de depender do usuário apertar o botão de ajuste.
	//---------------------------------------------------------------
	TWI_RTC_Write(RTC_CTR_REG,0x00); //Saída Habilitada, Frequência = 1Hz

	/**---------------------------------------------------------------
		Pega a configuração dos horários de despertar da EEPROM
	---------------------------------------------------------------*/
	Hora_Desp_1 = LE_EEPROM(EEP_HORA_DESP_1);
	Hora_Desp_2 = LE_EEPROM(EEP_HORA_DESP_2);
	Hora_Desp_3 = LE_EEPROM(EEP_HORA_DESP_3);
	Minuto_Desp_1 = LE_EEPROM(EEP_MINUTO_DESP_1);
	Minuto_Desp_2 = LE_EEPROM(EEP_MINUTO_DESP_2);
	Minuto_Desp_3 = LE_EEPROM(EEP_MINUTO_DESP_3);

	if(LE_EEPROM(EEP_ALARME_HAB_1) == 0)
	Clr_Userbit(bHab_Desp1);
	else
	Set_Userbit(bHab_Desp1);

	if(LE_EEPROM(EEP_ALARME_HAB_2) == 0)
	Clr_Userbit(bHab_Desp2);
	else
	Set_Userbit(bHab_Desp2);

	if(LE_EEPROM(EEP_ALARME_HAB_3) == 0)
	Clr_Userbit(bHab_Desp3);
	else
	Set_Userbit(bHab_Desp3);
	//-------------------------------------------


	#ifdef TESTE
	Serial_Print_Soft("TESTE");
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		Serial_Print_Soft("RL_DSP_88_RTC\r\n");
	}
	#endif // TESTE



	while (1)
	{

		Estado = Tab_Seq_Mq_Estado[Pont_Seq_Mq_Estado];		//Pega a tarefa

		switch (Estado)
		{
			case Stat_Atualiza_Rel:
			if(((Teste_Userbit(bPWR_Fail) | Teste_Userbit(bAjuste)) == 0))
			{
				if(Teste_Userbit(bToggleSegundo) != Teste_Userbit(bToggleSegImg))	//
				{
					Cpl_Userbit(bToggleSegImg);
					CPL_LED_SEG();			//Pisca o led do segundo
					Segundo = Converte_BCD_to_DEC(TWI_RTC_Read(RTC_Segundo));

					if(Segundo >= 55 || Segundo < 5)		//Janela de tolerância ao redor da virada do minuto
					{
						if(Teste_Userbit(bMinutoAtualizado) == 0 || Teste_Userbit(bForceAtlDSP) != 0)
						{
							Minuto = Converte_BCD_to_DEC(TWI_RTC_Read(RTC_Minuto));
							Display_Minuto = Minuto;
							Hora = Converte_BCD_to_DEC(TWI_RTC_Read(RTC_Hora));
							Display_Hora = Hora;

							Set_Userbit(bMinutoAtualizado);	//Trava até sair da janela
							Clr_Userbit(bForceAtlDSP);
						}
					}
					else
					Clr_Userbit(bMinutoAtualizado);		//Fora da janela, libera pra próxima virada
				}
			}
			Pont_Seq_Mq_Estado++;
			break;
		//=====================================================
		//				TRATA AS CHAVES
		//=====================================================
			case	Stat_Chaves:

			if((Teste_Userbit(bAlarme_ON) == 0 && Teste_Userbit(bSoneca) == 0))
			{
				if(LE_SW_SEL_DESP() == 0)
				{
					_delay_ms(50);
					if(LE_SW_SEL_DESP() == 0)		//Confirma leitura
					{
						if(Teste_Userbit(bPWR_Fail) != 0)  //Perdeu os dados?
						{
							TWI_RTC_Write(RTC_CTR_REG,0x00); //Saída Habilitada, Frequência = 1Hz
							Clr_Userbit(bPWR_Fail);		//Limpa o flag de falha de energia
						}

						while(LE_SW_SEL_DESP() == 0)	//Espera soltar a chave
						NOP();
						_delay_ms(50);

						Clr_Userbit(bPWR_Fail);		//limpa o flag de perda de ram
						pTabHoraMinuto = pTabHoraMinuto_bkp;		//Pega a próxima sequênciq

						Set_Userbit(bAjuste);			//Sinaliza que está ajustando
						if(pTabHoraMinuto == 0)
						{
							Display_Hora = Converte_BCD_to_DEC(TWI_RTC_Read(RTC_Hora));
							Display_Minuto = Converte_BCD_to_DEC(TWI_RTC_Read(RTC_Minuto));
							Hora = Display_Hora;
							Minuto = Display_Minuto;
						}
						else
						{
							Display_Hora  = *Tab_Hora_Desp[pTabHoraMinuto];	//Pega Hora correspondente e coloca no display
							Display_Minuto = *Tab_Minuto_Desp[pTabHoraMinuto]; //idem para Minuto
						}
						//-------------------------------------------------
						//Indo para próxima sequência da tabela de horários (Guarda para a próxima passada)
						//---------------------------------------------------
						pTabHoraMinuto_bkp++;
						if(Tab_Hora_Desp[pTabHoraMinuto_bkp] == &Fim_Tab_Hora ||Tab_Hora_Desp[pTabHoraMinuto_bkp] == &Fim_Tab_Minuto)
						pTabHoraMinuto_bkp = 0;

						CARREGA_TIMER_OVER_ADJ();
						//---------------------------------------------------

						Buzzer_On(1,20,0);
					}
				}
				//---------------------
				else if(LE_SW_HAB_DESP() == 0)
				{
					_delay_ms(50);
					if(LE_SW_HAB_DESP() == 0) //Confirma
					{
						while(LE_SW_HAB_DESP() == 0)	//Espera soltar a chave
						NOP();
						_delay_ms(50);

						if(pTabHoraMinuto == 1)
						{
							Cpl_Userbit(bHab_Desp1);
							GRAVA_EEPROM(EEP_ALARME_HAB_1,Teste_Userbit(bHab_Desp1));
						}

						else if(pTabHoraMinuto == 2)
						{
							Cpl_Userbit(bHab_Desp2);
							GRAVA_EEPROM(EEP_ALARME_HAB_2,Teste_Userbit(bHab_Desp2));
						}

						else if(pTabHoraMinuto == 3)
						{
							Cpl_Userbit(bHab_Desp3);
							GRAVA_EEPROM(EEP_ALARME_HAB_3,Teste_Userbit(bHab_Desp3));
						}

						CARREGA_TIMER_OVER_ADJ();

						Buzzer_On(1,20,0);
					}
				}
				//----------------------
				else if(LE_SW_SEL_HORA() == 0 && Teste_Userbit(bAjuste) != 0)		//Só executa se estiver no modo ajuste)
				{
					_delay_ms(50);
					if(LE_SW_SEL_HORA() == 0) //Confirma
					{
						Display_Hora  = (*Tab_Hora_Desp[pTabHoraMinuto]);	//Pega Hora correspondente e coloca no display
						Display_Minuto = (*Tab_Minuto_Desp[pTabHoraMinuto]); //idem para Minuto

						ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
						{Timer_SW_ACIONADA  = T_SW_ACIONADA;} //Carrega o temporizador da chave

						do
						{
							ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
							{Timer_SW_ACIONADA_Local = Timer_SW_ACIONADA;}

						}while(Timer_SW_ACIONADA_Local > 0 && LE_SW_SEL_HORA() == 0);

						//---------------------------------------------
						//		Avanço Automático da Hora
						//---------------------------------------------
						if(Timer_SW_ACIONADA_Local == 0)
						{
							while (LE_SW_SEL_HORA() == 0)
							{
								*Tab_Hora_Desp[pTabHoraMinuto] += 1;
								if(*Tab_Hora_Desp[pTabHoraMinuto] == 24)
								*Tab_Hora_Desp[pTabHoraMinuto] = 0;
								Display_Hora = (*Tab_Hora_Desp[pTabHoraMinuto]);
								Hora = Display_Hora;
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
							Display_Hora = (*Tab_Hora_Desp[pTabHoraMinuto]);
							Hora = Display_Hora;
							CARREGA_TIMER_OVER_ADJ();		//

						}
						if(pTabHoraMinuto == 0)		//Se estiver ajustando o horário atualiza o RTC
						{
							TWI_RTC_Write(RTC_Hora,Converte_DEC_to_BCD(Hora));
							TWI_RTC_Write(RTC_Segundo,0);
						}
						else
						{
							Habilita_Desp();		//Habilita o Despertador Correspondente

							GRAVA_EEPROM(Tab_Desp[pTabHoraMinuto][COL_EEPHORA_DESP],Hora); //Grava na eeprom

							/**---------------------------------------------------------------
												Grava na RAM
							---------------------------------------------------------------*/
							// 1. Pega o endereço da RAM que está guardado na Flash
							uint16_t endereco = Tab_Desp[pTabHoraMinuto][COL_HORA_DESP];

							// 2. Cria um "ponteiro fantasma" que aponta para esse lugar
							uint8_t *ponteiro_para_ram = (uint8_t *)(uintptr_t)endereco;

							// 3. Altera o valor da variável real (na RAM)
							*ponteiro_para_ram = Hora;

						}
						Buzzer_On(1,20,0);
					}
				}
				//----------------------
				else if(LE_SW_SEL_MINUTO() == 0 && Teste_Userbit(bAjuste) != 0)		//Só executa se estiver no modo ajuste
				{
					_delay_ms(50);
					if(LE_SW_SEL_MINUTO() == 0) //Confirma
					{
						//while(LE_SW_SEL_MINUTO() == 0)	//Espera soltar a chave
						//NOP();
						//_delay_ms(50);

						Display_Hora = (*Tab_Hora_Desp[pTabHoraMinuto]);	//Pega Hora correspondente e coloca no display
						Display_Minuto = (*Tab_Minuto_Desp[pTabHoraMinuto]); //idem para Minuto

						ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
						{ Timer_SW_ACIONADA = T_SW_ACIONADA;}		//Carrega o temporizador da chave

						do
						{
							ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
							{Timer_SW_ACIONADA_Local = Timer_SW_ACIONADA;}

						}while(Timer_SW_ACIONADA_Local > 0 && LE_SW_SEL_MINUTO() == 0);


						//---------------------------------------------
						//		Avanço Automático do Minuto
						//---------------------------------------------
						if(Timer_SW_ACIONADA_Local == 0)
						{
							while (LE_SW_SEL_MINUTO() == 0)
							{
								*Tab_Minuto_Desp[pTabHoraMinuto] += 1;
								if(*Tab_Minuto_Desp[pTabHoraMinuto] == 60)
								*Tab_Minuto_Desp[pTabHoraMinuto] = 0;
								Display_Minuto = (*Tab_Minuto_Desp[pTabHoraMinuto]);
								Minuto = Display_Minuto;
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
							Display_Minuto = (*Tab_Minuto_Desp[pTabHoraMinuto]);
							Minuto = Display_Minuto;
							CARREGA_TIMER_OVER_ADJ();
						}
						if(pTabHoraMinuto == 0)		//Só zera os segundos se estiver ajustando hora ou minuto
						{
							TWI_RTC_Write(RTC_Minuto,Converte_DEC_to_BCD(Minuto));
							TWI_RTC_Write(RTC_Segundo,0);
						}
						else
						{
							Habilita_Desp();	//Habilita o Despertador Correspondente
							GRAVA_EEPROM(Tab_Desp[pTabHoraMinuto][COL_EEPMINUTO_DESP], Minuto); //Grava na eeprom
							/**---------------------------------------------------------------
												Grava na RAM
							---------------------------------------------------------------*/
							// 1. Pega o endereço da RAM que está guardado na Flash
							uint16_t endereco = Tab_Desp[pTabHoraMinuto][COL_MINUTO_DESP];

							// 2. Cria um "ponteiro fantasma" que aponta para esse lugar
							uint8_t *ponteiro_para_ram = (uint8_t *)(uintptr_t)endereco;

							// 3. Altera o valor da variável real (na RAM)
							*ponteiro_para_ram = Minuto;
						}

						Buzzer_On(1,20,0);
					}
				}
				//----------------------
				else if(Teste_Userbit(bAjuste)!=0 && Timer_Over_ADJ == 0)		//Está no modo ajuste, O tempo encerrou
				{
					Clr_Userbit(bAjuste);
					RESET_PONTEIROS_TAB_HORARIOS();
					Set_Userbit(bForceAtlDSP);
				}
			}
			Pont_Seq_Mq_Estado++;
			break;
			//=====================================================
			//				TRATA O DESPERTADOR
			//=====================================================
			case	Stat_Despertador:
			//----------------------------------------
			//Verifica se tem uma chave pressionada
			//----------------------------------------
			if(Teste_Userbit(bAlarme_ON) != 0)
			{
				if(Verifica_Chave_Acionada() != 0)		//Tem uma chave acionada.
				{
					//while(Verifica_Chave_Acionada() != 0)
					//NOP();	//Espera soltar

					Clr_Userbit(bAlarme_ON);		//Libera o Soneca
					Set_Userbit(bSoneca);			//Ativa o Soneca
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						Timer_Soneca = TEMPO_SONECA;	//Carrega o soneca
					}
					Seq_Buzzer = 0;
					DESLIGA_BUZZER();
					Timer_Over_Despertador = 0;
					RESET_PONTEIROS_TAB_HORARIOS();
				}
				//----------------------------------------
				// Desliga o despertador pelo tempo
				//----------------------------------------
				else if(Timer_Over_Despertador == 0)		// Vai sair pelo tempo
				Default_Parametros();			//Arruma a casa e cai fora
			}
			Pont_Seq_Mq_Estado++;
			break;
			//=====================================================
			// 				TRATA A SONECA
			//=====================================================
			case	Stat_Soneca:
			if(Teste_Userbit(bSoneca) != 0)
			{
				if(Timer_Soneca == 0)	//Coloca para despertar
				{
					Set_Userbit(bAlarme_ON);		//Desperta Novamente
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{Timer_Soneca = TEMPO_SONECA;}	//Carrega o tempo para mais uma soneca
					Timer_Over_Despertador = MAX_TEMPO_DESPERTADOR;
				}	/*--------------------------------------------------------------
						Sai da Soneca pressionado qualquer chave por um tempo
					--------------------------------------------------------------*/
				else if(Verifica_Chave_Acionada() != 0)
				{
					Timer_Ch_Saida_Soneca = TEMPO_CH_AC_SONECA;
					/*----------------------------------------------
						Espera soltar a chave ou vencer o tempo
					------------------------------------------------*/
                    while(Verifica_Chave_Acionada() != 0 && Timer_Ch_Saida_Soneca > 0)
					{ NOP(); }

					if(Timer_Ch_Saida_Soneca == 0)	//TEM QUE MANTER A CHAVE ACIONADA POR UM TEMPO
					{
						Buzzer_On(2,50,50);
						Default_Parametros();		//Arruma a casa e cai fora,
						while(Verifica_Chave_Acionada() != 0)
						NOP();
					}
					else
					{
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
						{Timer_Soneca = TEMPO_SONECA;}	//Não que sair, mais uma soneca.
					}
				}
			}
			Pont_Seq_Mq_Estado++;
			break;
			//---------------------------
			default:
			Pont_Seq_Mq_Estado = 0;
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
 void Buzzer_On(uint8_t n_beeps,unsigned int t_beep,unsigned int t_intervalo_beep)
{
	do
	{
		LIGA_BUZZER();
		Wait(t_beep);
		DESLIGA_BUZZER();
		Wait(t_intervalo_beep);		//O tempo de intervalo
		n_beeps--;					//Acabou?
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
	Clr_Userbit(bAjuste);
	Segundo = 0;				//Recomeça a contagem
	RESET_PONTEIROS_TAB_HORARIOS();
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
	{
		Set_Userbit(bHab_Desp1);
		GRAVA_EEPROM(EEP_ALARME_HAB_1,1);
	}

	else if(pTabHoraMinuto == 2)
	{
		Set_Userbit(bHab_Desp2);
		GRAVA_EEPROM(EEP_ALARME_HAB_2,1);
	}

	else if(pTabHoraMinuto == 3)
	{
		Set_Userbit(bHab_Desp3);
		GRAVA_EEPROM(EEP_ALARME_HAB_3,1);
	}
}

 /**========================================================================
 * Rotina:Verica_Chave_Acionada
 * Função:Verifica se há uma chave acionada sem identificar qual é
 * Chamada:Verica_Chave_Acionada()
 * Retorna:Retorna 1 se OK, 0 se não tiver nenhuma chave acionada
 * Comentário:
 * ----------------------------------------------------------------------*/
uint8_t Verifica_Chave_Acionada()
{
	uint8_t Sta_Chaves = 0;

	if(((PINB & 0b00000111) != 0b00000111) || (PIND & 0b10000000) == 0)	//Tem chave acionada?
	{
		_delay_ms(50);		//Espera o bouncing
		if ((PINB & 0b00000111) != 0b00000111 || (PIND & 0b10000000) == 0)
		Sta_Chaves = 1;
	}
	else Sta_Chaves = 0;

	return Sta_Chaves;
}
 /**========================================================================
 * Rotina:Default_Parametros
 * Função:Coloca os valores default nas variáveis envolvidas.
 * Chamada:Default_Parametros()
 * Retorna: Nada
 * Comentário:N/A
 * ----------------------------------------------------------------------*/
void Default_Parametros()
{
	Clr_Userbit(bAlarme_ON);
	Clr_Userbit(bSoneca);
	Timer_Buzzer_Alarme = 0;
	Seq_Buzzer = 0;
	Timer_Over_Despertador = 0;
	Clr_Userbit(bDesp1_ON);
	Clr_Userbit(bDesp2_ON);
	Clr_Userbit(bDesp3_ON);
	DESLIGA_BUZZER();
	RESET_PONTEIROS_TAB_HORARIOS();
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
