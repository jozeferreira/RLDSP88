/*========================================================================
 *		Módulo de definições
 *		Nesse Módulo são definidos as variáveis e funções usadas
 *		nos demais módulos do projeto.
 * 		Plataforma: IDE ARDUINO
 ========================================================================*/
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef Mod_Main
#define subs /**/
#else
#define subs extern
#endif

#define		INPUT	0
#define		OUTPUT	1


/*------------------------------------
	REDEFININDO OS PINOS Atmega328
-------------------------------------*/
//------------------------------------
//     Definições do Projeto
//----------------------------------
//								Pinos do
//								ATmega328
/*
#define		Seg_A					2
#define		Seg_B					3
#define		Seg_C					4
#define		Seg_D					5
#define		Seg_E					6
#define		Seg_F					11
#define		Seg_G					12
#define		SW_Sel_Hora				13		//Chave de seleção para acertar hora
#define		SW_Sel_Minuto			14		//Chave de seleção para acertar minuto
#define		SW_Sel_Desp				15
#define		SW_Hab_Desp				16
#define		Led_Desp_1				17
#define		Led_Desp_2				18
#define		Led_Desp_3				19
#define		A0						23
#define		A1						24
#define     Buzzer					25
#define		Led_Seg					26
#define		Mon_AC					27		//(ADC4) Monitora se há Rede AC
#define		Mon_Bat					28		//(ADC5) Monitora tensão da bateria
*/
//--------------------------------------
//Definições dos Pórticos
//PORTB
#define 	SW_Sel_Minuto	PB0
#define 	SW_Sel_Desp		PB1
#define 	SW_Hab_Desp		PB2
#define 	Led_Desp_1		PB3
#define 	Led_Desp_2		PB4
#define 	Led_Desp_3		PB5
//--------------------------------------
//PORTC
#define		Buzzer			PC2
#define		Led_Seg			PC3
#define		Mon_AC			PC4
#define		Mon_Bat			PC5
//--------------------------------------
//PORTD
#define		SW_Sel_Hora		PD7
//--------------------------------------
//Constantes Numéricas
//--------------------------------------
#define CLKXTAL_1840000Hz
//#define CLKXTAL_230400Hz

#define 	PERIODO_TIMER_1			5			            //em milisegundos
#define		T_SW_ACIONADA		    1000/PERIODO_TIMER_1   //em milisegundos
#define     T_PISCA_DSP_ADJ         20/PERIODO_TIMER_1     //Cadência de piscadas dos displays no modo ajuste
#define     T_OVER_ADJ              5000/PERIODO_TIMER_1

#define		LOW_BAT					471						//Equivale a bateria com 7,2 Volts lidos no menor resistor de
															//uma combinação série 1M-> 470K
#define		NEAR_LOW_BAT			523						//O mesmo, para tensão de bateria de 8,0V
#define		POWER_FAIL				355						//Equivale a 7V da tensão da fonte de alimentação lida no menor
															//resistor de uma combinação série 1M:330K
#define		VHISTERESE				7						//Equivale a ~100mV na Bateria lido na série de resistores
															//1M-> 470K


//Mascaras para números dos displays
//                Pgfedcba
#define num_0   ~0b00111111    //num_0
#define num_1   ~0b00000110    //num_1
#define num_2   ~0b01011011    //num_2
#define num_3   ~0b01001111    //num_3Teste_Userbit
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

//Definições dos estados da Máquina de estado
#define     Stat_Mon_Power      0           //Verifica a Rede AC
#define		Stat_Mon_Bat		1			//Monitora a tensão da bateria
#define		Stat_First_Time		2			//Monitora se relógio perdeu consistência
#define 	Stat_Chaves			3
#define		Stat_Despertador	4
#define		Stat_Soneca			5
/*================================================================*/
///Posições com para uso de bits como flags
unsigned int  AddresBit;
#define		bApaga_DSP 		0	//1
#define		bt_Over_ADJ		1	//2
#define		bLow_Bat		2   //4      	//Habilita o tempo que o relógio sai do modo ajuste automaticamente
#define		bNear_Low_Bat	3	//8
#define		bFirst_Time		4	//16
#define		bPower_Fail		5	//32
#define		bAlarme_ON		6	//64
#define		bAjuste			7	//128
//-----------------------------------
#define		bSW_HORA_TEMP	8	//256
#define		bHab_Desp1		9   //512 		 Habilita ajustar hora e minuto
#define		bHab_Desp2		10	//1024
#define		bHab_Desp3		11	//2048
#define		bDesp1_ON		12	//4096
#define		bDesp2_ON		13	//8092
#define		bDesp3_ON		14	//16384
#define		bSoneca			15	//32768
//======================================================================
//Macros
#define CLR_USERBIT(bit)	\
AddresBit &= ~(1 << bit)
//--------------------
#define SET_USERBIT(bit)	\
AddresBit |=(1 << bit)
//--------------------
#define CPL_USERBIT(bit)	\
AddresBit ^=(1 << bit)
//--------------------
#define TESTE_USERBIT(bit)	\
(AddresBit & (1 << bit))
//--------------------
#define SET_PORT_BIT(port,bit)	\
port |= (1 << bit)
//--------------------
#define CLR_PORT_BIT(port,bit)	\
port &= ~(1 << bit)
//--------------------
#define CPL_LED_SEG()	\
{PORTC_Img ^= (1 << Led_Seg); PORTC = PORTC_Img;}
//--------------------
#define CLR_PORT_BIT(port,bit)	\
port &= ~(1 << bit)
//--------------------
#define SET_PORT_BIT(port,bit)	\
port |= (1 << bit)
//--------------------
#define LE_SW_SEL_HORA() (PIND & (1 << SW_Sel_Hora))
//---------------------------------------------------------------
#define LE_SW_SEL_MINUTO() (PINB & (1 << SW_Sel_Minuto))
//---------------------------------------------------------------
#define LE_SW_SEL_DESP() (PINB & (1 << SW_Sel_Desp))
//---------------------------------------------------------------
#define LE_SW_HAB_DESP() (PINB & (1 << SW_Hab_Desp))
//---------------------------------------------------------------
#define ACENDE_LED_DESP(led)	\
PORTB |= (1 << led)
//---------------------------------------------------------------
#define APAGA_LED_DESP(led)	\
PORTB &= ~(1 << led)
//---------------------------------------------------------------
#define ACENDE_LED_SEGUNDO()		\
{PORTC_Img |= (1 << Led_Seg); PORTC = PORTC_Img;}
//---------------------------------------------------------------
#define APAGA_LED_SEGUNDO()	\
{PORTC_Img &= ~(1 << Led_Seg); PORTC = PORTC_Img;}
//---------------------------------------------------------------
#define CPL_LED_DESP(led)	\
PORTB ^= (1 << led)
//---------------------------------------------------------------
#define  CARREGA_TIMER_OVER_ADJ()		\
Timer_Over_ADJ = T_OVER_ADJ
//---------------------------------------------------------------
#define LIGA_BUZZER() { SET_PORT_BIT(PORTC_Img,Buzzer);	PORTC = PORTC_Img;}
//---------------------------------------------------------------
#define DESLIGA_BUZZER() {CLR_PORT_BIT(PORTC_Img,Buzzer);	PORTC = PORTC_Img;}
//======================================================================
//Memória Volátil
subs unsigned char Hora;					//Buffer para guardar a Hora
subs unsigned char Minuto;				//Buffer para guardar o Minuto
subs unsigned char Segundo;
subs unsigned char VL_Ch;					//Guarda o valor da chave acionada
subs unsigned char Estado;
subs unsigned char Pont_Seq_Mq_Estado;
subs unsigned char CTR_Display;
subs unsigned char Real_Timer;
subs unsigned int Timer_Led_Seg;		//Buffer da cadência das piscadas do led do segundo
subs unsigned char Timer_Led_Desp;
subs unsigned char Timer_Led_Alarme;
subs unsigned int Timer_SW_ACIONADA;    //Temporiza o acionamento da chave
subs unsigned int Timer_Over_ADJ;       //Buffer do tempo que o relógio leva para sair do modo ajuste automaticamente
subs unsigned int Timer_Wait;
subs unsigned int Timer_Over_Despertador;	//Temporiza máximo tempo do despertador
subs unsigned int Leitura_ADC;
subs unsigned char Hora_Desp_1;
subs unsigned char Hora_Desp_2;
subs unsigned char Hora_Desp_3;
subs unsigned char Fim_Tab_Hora;
subs unsigned char Minuto_Desp_1;
subs unsigned char Minuto_Desp_2;
subs unsigned char Minuto_Desp_3;
subs unsigned char Fim_Tab_Minuto;
subs unsigned char Timer_Mon_Energia;
subs unsigned char Display_Hora;
subs unsigned char Display_Minuto;
subs unsigned char pTabHoraMinuto;
subs unsigned char pTabHoraMinuto_bkp;
subs unsigned char PORTC_Img;
subs unsigned char Seq_Buzzer;
subs unsigned char Timer_Buzzer_Alarme;
//=====================================================================
//Memória de Programa
#ifdef Mod_Main

__flash const unsigned char Tab_Num[]={num_0,num_1,num_2,num_3,num_4,num_5,num_6,num_7,num_8,num_9};

#else
extern __flash const unsigned char Tab_Num[];

#endif
//----------------------------------------------------------------
// Tabelas Memória Ram
#ifdef Mod_Main
unsigned char *Tab_Hora_Desp[]= {&Hora,&Hora_Desp_1,&Hora_Desp_2,&Hora_Desp_3,&Fim_Tab_Hora};
unsigned char *Tab_Minuto_Desp[]= {&Minuto,&Minuto_Desp_1,&Minuto_Desp_2,&Minuto_Desp_3,&Fim_Tab_Minuto};

#else
extern unsigned char *Tab_Hora_Desp[];
extern unsigned char *Tab_Minuto_Desp[];

#endif // Mod_Main

//======================================================================
//Rotinas:
subs void Buzzer_On(unsigned char n_beeps,unsigned int t_beep,unsigned int t_intervalo_beep);
subs unsigned char Incrementa_Modo_BCD(unsigned char Numero_BCD);
subs void Contador_Relogio();
subs void Configura_Timer_1_Modo_CTC();
subs void Config_ADC();
subs void Le_ADC(unsigned char CH_ADC);;
subs void Wait (unsigned int buf_loop_delay);
subs void Rd_Byte_Usart();
subs void Sai_Modo_Ajuste();
subs void Habilita_Desp();
//======================================================================
