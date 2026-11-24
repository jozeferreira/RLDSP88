/*========================================================================
 *		Módulo de definições
 *		Nesse Módulo são definidos as variáveis e funções usadas
 *		nos demais módulos do projeto.
 * 		Plataforma: IDE ARDUINO
 ========================================================================*/
#define F_CPU 1843200UL

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/eeprom.h>
#include "soft_serial.h"

#ifdef Mod_Main
#define subs /**/
#else
#define subs extern
#endif

#define		INPUT	0
#define		OUTPUT	1
#define NOP() __asm__ __volatile__ ("nop")
//-----------------------------------------------
//Máscaras das Leituras das Chaves
#define 	CHON_HAB_DESP	0b00000110
#define 	CHON_SEL_HORA	0b00000101
#define		CHON_SEL_MINUTO	0b00000011
#define		CHON_SEL_DESP	0b10000000
#define		CHOFF			0b00000111
//-----------------------------------------------
//#define RTC_1307
//--------------------------------------
//Definições dos Pórticos
//PORTB
#define 	SW_Hab_Desp		0
#define 	SW_Sel_Hora		1
#define 	SW_Sel_Minuto	2
#define 	Led_Desp_1		3
#define 	Led_Desp_2		4
#define 	Led_Desp_3		5
//#define 					6
#define		PB7T			7
//--------------------------------------
//PORTC
#define 	A0_HC139		8
#define 	A1_HC139		9
#define		Buzzer			10
#define		Led_Seg			11
#define		SDA				12
#define		SCL				13
//#define 					14
//#define 					15
//--------------------------------------
//PORTD
//#define 					16
//#define 					17
//#define 					18
//#define 					19
//#define 					20
//#define 					21
//#define 					22
#define		SW_Sel_Desp		23		//SW_Sel_Hora SW1
//--------------------------------------
//Constantes Numéricas
//--------------------------------------
//#define CLKXTAL_1840000Hz
//#define CLKXTAL_230400Hz

#define 	PERIODO_TIMER_1			5			            //em milisegundos
#define		T_SW_ACIONADA		    1000/PERIODO_TIMER_1   //em milisegundos
#define     T_PISCA_DSP_ADJ         20/PERIODO_TIMER_1     //Cadência de piscadas dos displays no modo ajuste
#define     TEMPO_SAIDA_AJUSTE      5						//Tempo para sair do modo ajuste (em segundos)
#define		MAX_TEMPO_DESPERTADOR	60						//Tempo para parar despertador automaticamente(em segundos)
#define		TEMPO_SONECA			600						// Em segundos
#define		TEMPO_CH_AC_SONECA		3						//Tempo para manter uma chave pressiona e sair do soneca (em segundos)
#define		LOW_BAT					471						//Equivale a bateria com 7,2 Volts lidos no menor resistor de
															//uma combinação série 1M-> 470K
#define		NEAR_LOW_BAT			523						//O mesmo, para tensão de bateria de 8,0V
#define		POWER_FAIL				355						//Equivale a 7V da tensão da fonte de alimentação lida no menor
															//resistor de uma combinação série 1M:330K
#define		VHISTERESE				7						//Equivale a ~100mV na Bateria lido na série de resistores
															//1M-> 470K
#define		TIME_BOUNCING			100						//tEMPO PARA BOUNCING DAS CHAVES
#define 	FATOR_CORR_PORTC_IMG
/**---------------------------------------------------------------
//Mascaras para números dos displays
---------------------------------------------------------------*/
//                Pgfedcba
#define num_0   ~0b00111111    //num_0
#define num_1   ~0b00000110    //num_1
#define num_2   ~0b01011011    //num_2
#define num_3   ~0b01001111    //num_3 Teste_Userbit
#define num_4   ~0b01100110    //num_4
#define num_5   ~0b01101101    //num_5
#define num_6   ~0b01111101    //num_6
#define num_7   ~0b00000111    //num_7
#define num_8   ~0b01111111    //num_8
#define num_9   ~0b01100111    //num_9Teste_Userbit

#define	Let_E	~0b01111001	  //Letra E
#define Let_R   ~0b01010000	  //Letra R

#define		CTR_Unid_Hora		0
#define		CTR_Dez_Hora		1
#define		CTR_Dez_Min			2
#define		CTR_Unid_Min		3
/**---------------------------------------------------------------
//Definições dos estados da Máquina de estado
---------------------------------------------------------------*/
#define 	Stat_Chaves			0
#define		Stat_Despertador	1
#define		Stat_Soneca			2
#define 	Stat_Atualiza_Rel	3
#define		FIM					100
/**---------------------------------------------------------------
//Macros para escrita e leitura na eeprom
---------------------------------------------------------------*/
#define GRAVA_EEPROM(Endereco,Dado) eeprom_write_byte((uint8_t*)Endereco,Dado);
#define	GRAVA_EEPROM_INT(Endereco,Dado) eeprom_write_word((uint16_t*)Endereco,Dado);
#define	LE_EEPROM(Endereco) eeprom_read_byte((uint8_t*)Endereco)
#define	LE_EEPROM_INT(Endereco) eeprom_read_word((uint16_t*)Endereco)
/**---------------------------------------------------------------
	Endereços na EEPROM para os despertadores
---------------------------------------------------------------*/
#define EEP_ASSINATURA_1	0x05
#define EEP_HORA_DESP_1		0x10
#define EEP_MINUTO_DESP_1	0x11
#define EEP_HORA_DESP_2		0x12
#define EEP_MINUTO_DESP_2	0x13
#define EEP_HORA_DESP_3		0x14
#define EEP_MINUTO_DESP_3	0x15
#define EEP_ALARME_HAB_1	0x16
#define EEP_ALARME_HAB_2	0x17
#define EEP_ALARME_HAB_3	0x18
//-----						0x19
#define EEP_ASSINATURA_2	0x20
/*================================================================*/
///Posições com para uso de bits como flags
subs volatile uint8_t  AddresBit_0;
#define		bSoneca				0
#define		bt_Over_ADJ			1
#define		bPWR_Fail			2       	//Habilita o tempo que o relógio sai do modo ajuste automaticamente
#define		bToggleSegundo		3			//Sinaliza que virou o segundo
#define		bForceAtlDSP		4			//Indica que tem um Alarme Ativo
#define		bPower_Fail			5
#define		bAlarme_ON			6
#define		bAjuste				7
//-----------------------------------
subs volatile uint8_t  AddresBit_1;
#define		bSW_HORA_TEMP	8
#define		bHab_Desp1		9
#define		bHab_Desp2		10
#define		bHab_Desp3		11
#define		bDesp1_ON		12
#define		bDesp2_ON		13
#define		bDesp3_ON		14
#define		bTeclas_Ativas	15
//-----------------------------------
subs volatile uint8_t  AddresBit_2;
#define		bToggleSegImg	16
#define		bMinutoAtualizado	17			//Trava: já atualizou Hora/Minuto nesta janela
#define		bit_18			18
#define		bit_19			19
#define		bit_20			20
#define		bit_21			21
#define		bit_22			22
#define		bit_23			23
//-----------------------------------
subs volatile uint8_t  PORTC_Img;
#define		bA0_HC139_Img	24
#define		bA1_HC139_Img	25
#define		bBuzzer_Img		26
#define		bLed_Seg_Img	27
#define		bSDA_Img		28
#define		bSCL_Img		29
//#define					30
//#define					31

//======================================================================
//Macros
//--------------------
#define SET_PORT_BIT(port,bit)	\
port |= (1 << bit)
//--------------------
#define CLR_PORT_BIT(port,bit)	\
port &= ~(1 << bit)
//--------------------
#define CPL_LED_SEG()	\
{						\
	Cpl_Userbit(bLed_Seg_Img);    \
	PORTC = PORTC_Img;	\
}
//--------------------
#define CLR_PORT_BIT(port,bit)	\
port &= ~(1 << bit)
//--------------------
#define SET_PORT_BIT(port,bit)	\
port |= (1 << bit)
//--------------------
#define LE_SW_SEL_HORA() Teste_Port_Bit(SW_Sel_Hora)
//---------------------------------------------------------------
#define LE_SW_SEL_MINUTO() Teste_Port_Bit(SW_Sel_Minuto)
//---------------------------------------------------------------
#define LE_SW_SEL_DESP() Teste_Port_Bit(SW_Sel_Desp)
//---------------------------------------------------------------
#define LE_SW_HAB_DESP() Teste_Port_Bit(SW_Hab_Desp)
//---------------------------------------------------------------
#define ACENDE_LED_DESP(led)	\
Set_Port_Bit(led)
//---------------------------------------------------------------
#define APAGA_LED_DESP(led)	\
Clr_Port_Bit(led)
//---------------------------------------------------------------
#define ACENDE_LED_SEGUNDO()	\
{							\
	Set_Userbit(bLed_Seg_Img);		\
	PORTC = PORTC_Img;		\
}
//---------------------------------------------------------------
#define APAGA_LED_SEGUNDO()		\
{								\
	Clr_Userbit(bLed_Seg_Img);		\
	PORTC = PORTC_Img;				\
}
//-------------------------------------
#define SET_SDA()		\
{							\
	Set_Userbit(bSDA_Img);		\
	PORTC = PORTC_Img;		\
}
//---------------------------------------------------------------
#define CLR_SDA()			\
{							\
	Clr_Userbit(bSDA_Img);	\
	PORTC = PORTC_Img;		\
}
//---------------------------------------------------------------
#define SET_SCL()		\
{							\
	Set_Userbit(bSCL_Img);		\
	PORTC = PORTC_Img;		\
}
//---------------------------------------------------------------
#define CLR_SCL()		\
{							\
	Clr_Userbit(bSCL_Img);		\
	PORTC = PORTC_Img;		\
}
//---------------------------------------------------------------
#define CPL_LED_DESP(led) Cpl_Port_Bit(led)
//---------------------------------------------------------------
#define  CARREGA_TIMER_OVER_ADJ()		\
Timer_Over_ADJ = TEMPO_SAIDA_AJUSTE
//---------------------------------------------------------------
#define LIGA_BUZZER()	\
Set_Userbit (bBuzzer_Img);	\
PORTC = PORTC_Img
//---------------------------------------------------------------
#define DESLIGA_BUZZER() \
Clr_Userbit (bBuzzer_Img);	\
PORTC = PORTC_Img
//---------------------------------------------------------------
#define RESET_PONTEIROS_TAB_HORARIOS()	\
pTabHoraMinuto_bkp = 0;					\
pTabHoraMinuto = 0;						\
/**---------------------------------------------------------------
			Módulo RTC 3231
---------------------------------------------------------------*/
#define RTC_Address		0b01101000
#define RTC_Segundo		0x00
#define RTC_Minuto		0x01
#define RTC_Hora		0x02
#define RTC_Address		0b01101000
#define RTC_RS0_RS1		0b00011000	// DS3231. Máscara para teste dos bits Rs1 e RS2 que definem a  frequência de saída
#define RTC_CTR_REG		0x0E		//DS3231

//----------------------------------
subs uint8_t 	nClock;
subs uint8_t	LeitRTC;
subs uint8_t 	Mask;
//-----------------------------------------------------------
#define CLOCK()											\
Set_Port_Bit(SCL);		/*Coloca SCl em 1*/				\
/*_delay_us(100);*/		/*100us de largura de pulso*/	\
Clr_Port_Bit(SCL);		/*Coloca SCl em 0*/				\
//======================================================================
//Memória Volátil
subs volatile uint8_t Aux8;
subs volatile uint16_t Aux16;
subs volatile uint8_t Hora;					//Buffer para guardar a Hora
subs volatile uint8_t Minuto;				//Buffer para guardar o Minuto
subs volatile uint8_t Segundo;
subs uint8_t VL_Ch;					//Guarda o valor da chave acionada
subs uint8_t Estado;
subs uint8_t Pont_Seq_Mq_Estado;
subs uint8_t CTR_Display;
subs volatile uint8_t Real_Timer;
subs volatile uint16_t Timer_Led_Seg;		//Buffer da cadência das piscadas do led do segundo
subs volatile uint8_t Timer_Led_Desp;
subs volatile uint8_t Timer_Led_Alarme;
subs volatile uint16_t Timer_SW_ACIONADA;    //Temporiza o acionamento da chave
subs volatile uint8_t Timer_Over_ADJ;       //Buffer do tempo que o relógio leva para sair do modo ajuste automaticamente
subs volatile uint16_t Timer_Wait;
subs volatile uint8_t Timer_500ms;
subs volatile uint8_t Timer_Over_Despertador;	//Temporiza máximo tempo do despertador
subs uint16_t Leitura_ADC;
subs uint8_t Hora_Desp_1;
subs uint8_t Hora_Desp_2;
subs uint8_t Hora_Desp_3;
subs uint8_t Fim_Tab_Hora;
subs uint8_t Minuto_Desp_1;
subs uint8_t Minuto_Desp_2;
subs uint8_t Minuto_Desp_3;
subs uint8_t Fim_Tab_Minuto;
subs volatile uint8_t Timer_Mon_Energia;
subs volatile uint8_t Display_Hora;
subs volatile uint8_t Display_Minuto;
subs volatile uint8_t pTabHoraMinuto;
subs uint8_t pTabHoraMinuto_bkp;
subs uint8_t Seq_Buzzer;
subs volatile uint8_t Timer_Buzzer_Alarme;
subs volatile uint16_t Timer_Soneca;
subs volatile uint8_t Timer_Ch_Saida_Soneca;
subs volatile uint8_t Seq_Leit_Chaves;
subs volatile uint8_t ValTeclabkp;
subs volatile uint8_t Time_Bouncing;
subs volatile uint8_t VL_Ch_Temp;
subs volatile uint8_t VL_Chaves;
subs uint8_t Hab_Despertador;
//=====================================================================
//Memória de Programa
#ifdef Mod_Main

__flash const uint8_t Tab_Num[]={num_0,num_1,num_2,num_3,num_4,num_5,num_6,num_7,num_8,num_9};

#else
extern __flash const uint8_t Tab_Num[];

#endif
//----------------------------------------------------------------
// Tabelas Memória Ram
#ifdef Mod_Main
volatile uint8_t *Tab_Hora_Desp[]= {&Hora,&Hora_Desp_1,&Hora_Desp_2,&Hora_Desp_3,&Fim_Tab_Hora};
volatile uint8_t *Tab_Minuto_Desp[]= {&Minuto,&Minuto_Desp_1,&Minuto_Desp_2,&Minuto_Desp_3,&Fim_Tab_Minuto};

#else
extern volatile uint8_t *Tab_Hora_Desp[];
extern volatile uint8_t *Tab_Minuto_Desp[];

#endif // Mod_Main

//======================================================================
//Rotinas:
subs void Buzzer_On(uint8_t n_beeps,uint16_t t_beep,uint16_t t_intervalo_beep);
subs uint8_t Incrementa_Modo_BCD(uint8_t Numero_BCD);
subs void Contador_Relogio();
subs void Configura_Timer_1_Modo_CTC();
subs void Config_ADC();
subs void Le_ADC(uint8_t CH_ADC);;
subs void Wait (uint16_t buf_loop_delay);
subs void Rd_Byte_Usart();
subs void Sai_Modo_Ajuste();
subs void Habilita_Desp();
subs uint8_t Verifica_Chave_Acionada();
subs void Default_Parametros();
subs uint8_t Teste_Userbit (uint8_t num_bit);
subs void Set_Userbit(uint8_t num_bit);
subs void Clr_Userbit(uint8_t num_bit);
subs void Cpl_Userbit(uint8_t num_bit);
subs void Set_Port_Bit(uint8_t num_bit);
subs void Clr_Port_Bit(uint8_t num_bit);
subs void Cpl_Port_Bit(uint8_t num_bit);
subs uint8_t Teste_Port_Bit(uint8_t num_bit);
subs void Config_Port_Bit(uint8_t num_bit,uint8_t rw);
//subs uint8_t RTC_Read(uint8_t Address_Data);
//subs void RTC_Write(uint8_t Addres_Data,uint8_t Data_Write);
subs uint8_t Converte_BCD_to_DEC(uint8_t BCD_Num);
subs uint8_t Converte_DEC_to_BCD(uint8_t Dec_Num);
subs void TWI_Init(void);
subs void TWI_Write(uint8_t data);
subs uint8_t TWI_ReadNACK(void);
subs void TWI_Start(uint8_t address) ;
subs void TWI_Stop(void);
subs uint8_t TWI_RTC_Read(uint8_t Reg_Address);
subs void TWI_RTC_Write(uint8_t Reg_Address, uint8_t Data);
//======================================================================
