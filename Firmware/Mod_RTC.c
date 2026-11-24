/*=====================================================================================
	Módulo I2C para dispositivos com 7 bits de endereço + 1 bit de escrita/leitura
=====================================================================================*/
#define		Mod_1307
#include	"Mod_Def.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#ifdef _000_

#define _delay_us(data)													\
for(char delayus = data/100;delayus > 0; delayus--)								\
{																			\
	for(ctedelay100us = Cte_Delay_100us;ctedelay100us > 0; ctedelay100us--)	\
	NOP();																\
}
//------------------------------------------------------------------------------
#define CLOCK()												\
SET_SCL();		/*Coloca SCl em 1*/				\
_delay_us(100);*/		/*100us de largura de pulso*/	\
CLR_SCL();		/*Coloca SCl em 0*/				\

#endif // _000_
//------------------------------------------------------------------------------
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void TWI_RTC_Write(uint8_t Reg_Address, uint8_t Data)
{
    // 1. Inicia a transmissão (START + Endereço do RTC em modo Escrita)
    // O endereço do RTC costuma ser 0xD0 (0x68 << 1)
    TWI_Start((RTC_Address << 1) | 0);

    // 2. Envia o endereço do registrador onde queremos escrever (Ex: 0x00 para segundos)
    TWI_Write(Reg_Address);

    // 3. Envia o dado propriamente dito
    TWI_Write(Data);

    // 4. Envia o sinal de STOP para liberar o barramento
    TWI_Stop();
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
uint8_t TWI_RTC_Read(uint8_t Reg_Address)
{
    uint8_t dado;

    // 1. Start e Endereço do RTC (Escrita para setar o ponteiro)

    TWI_Start((RTC_Address << 1) | 0);
    TWI_Write(Reg_Address);

    // 2. Restart para Leitura
    TWI_Start((RTC_Address << 1) | 1);

    // 3. Lê o dado e termina com NACK
    dado = TWI_ReadNACK();

    // 4. Stop
    TWI_Stop();

    return dado;
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void TWI_Init(void)
{
    // Para 1.8432MHz, 100kHz é difícil (o clock da CPU é baixo).
    // Vamos configurar para a maior velocidade estável possível:
    TWSR = 0x00; // Prescaler = 1
    TWBR = 0x02; // Valor baixo para compensar o clock baixo da CPU
    // Isso resultará em aprox. 92kHz de SCL.
    TWCR = (1 << TWEN); // Habilita o módulo TWI
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void TWI_Write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
uint8_t TWI_ReadACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Lê e envia ACK
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
uint8_t TWI_ReadNACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN); // Lê sem enviar ACK (último byte)
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void TWI_Stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    // O Stop não precisa esperar o TWINT
}
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void TWI_Start(uint8_t address)
{
    // Envia sinal de START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Espera completar

    // Envia o endereço do dispositivo
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

/*====================================================
Rotina:Converte_BCD_to_DEC
Função:Converte um numero BCD para decimal
Chamada:Converte_BCD_to_DEC(<Numero BCB>)
Devolve: Numero BCD convertido
Comentário: 0x85 devolve 85 decimal
====================================================*/
uint8_t Converte_BCD_to_DEC(uint8_t BCD_Num)
{
	uint8_t x,y,Num_Dec;

	x = BCD_Num & 0x0F;		//Separa a unidade
	y = BCD_Num & 0xF0;		//Separa a dezena
	y = ((y >> 4)*10);

	Num_Dec = x + y;

	return Num_Dec;
}
/**========================================================================
 * Rotina: Converte_DEC_to_BCD
 * Função: Converte um Número decimal para BCD
 * Chamada:Converte_DEC_to_BCD(<Numero Decimal>)
 * Retorna: O número convertido
 * Comentário: N/A
 * ----------------------------------------------------------------------*/
uint8_t Converte_DEC_to_BCD(uint8_t Dec_Num)
{
/*	uint8_t	Num_BCD;
	uint8_t 	x,y;

	x = Dec_Num/10;		//Pega a dezena
	y = Dec_Num%10;		//Pega a Unidade
	x = (x << 4);
	Num_BCD =  x ^ y;

	return Num_BCD;
*/
	return ((Dec_Num / 10) << 4) | (Dec_Num % 10);
}
/*======================================================================
					DESABILITADOS
======================================================================*/
#ifdef _000_
/**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/
void RTC_Write(uint8_t Addres_Data,uint8_t Data_Write)
{
	uint8_t y;

	SET_SDA();			//começa em alto para facilitar
	SET_SCL();

/*-------------------------------------
Montando o byte de 8 bits para ser enviado
-------------------------------------*/
	y = (RTC_Address << 1);		//Desloca para esquerda
	y &= (0b11111110);				//Escrita
/*--------------------------------------------
	Gerando Start
----------------------------------------------*/
	Config_Port_Bit(SDA,OUTPUT);		//Escrita
	CLR_SDA();
	_delay_us(5);		//500us
	CLR_SCL();
	_delay_us(5);		//100us
	/*----------------------------------------------
		Enviando 7 bits do endereço do dispositivo + R/W
	----------------------------------------------*/
	Mask = 0x80;

	for(nClock = 0; nClock < 8; nClock++)
	{
		Mask = 0x80;
		Mask = (Mask >> nClock);

		if((y & Mask) != 0)
		SET_SDA()
		else
		CLR_SDA();
		//_delay_us(100);		//100us
		CLOCK();
	}
	/*---------------------------------------
		Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	Config_Port_Bit(SDA,INPUT);	//Garante Port modo leitura antes do disposivo enviar dado
	_delay_us(5);		//100us
	CLOCK();
	_delay_us(5);		//100us
	/*---------------------------------------------
		Enviando 8 bits do endereço de dados
	----------------------------------------------*/
	Config_Port_Bit(SDA,OUTPUT);		//Escrita
	Mask = 0x80;

	for(nClock = 0; nClock < 8; nClock++)
	{
		Mask = 0x80;
		Mask = (Mask >> nClock);

		if((Addres_Data & Mask) != 0)
		SET_SDA()
		else
		CLR_SDA()

		_delay_us(5);		//100us
		CLOCK();
		_delay_us(5);		//100us
	}
	/*---------------------------------------
		Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	DDRC &= (0b11101111);				//SDA Leitura (DIMINUI TEMPO DE CONFLITO ENTRE PINOS SDA)
	//_delay_us(100);		//100us
	CLOCK();
	_delay_us(5);		//100us
	/*---------------------------------------
		Enviando o dado
	---------------------------------------*/
	Mask = 0x80;
	Config_Port_Bit(SDA,OUTPUT);		//Escrita
	_delay_us(5);		//100us
	for(nClock = 0; nClock < 8; nClock++)
	{
		Mask = 0x80;
		Mask = (Mask >> nClock);

		if((Data_Write & Mask) != 0)
		SET_SDA()
		else
		CLR_SDA();
		//_delay_us(100);		//100us
		CLOCK();
	}
	/*---------------------------------------
	Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	DDRC &= (0b11101111);				//SDA Leitura (DIMINUI TEMPO DE CONFLITO ENTRE PINOS SDA)
	//_delay_us(100);		//100us
	CLOCK();
	_delay_us(5);		//100us
	/*--------------------------------------
	Gerando um STOP
	---------------------------------------*/
	Config_Port_Bit(SDA,OUTPUT);		//Escrita
	CLR_SDA();
	_delay_us(5);		//100us
	SET_SCL();
	_delay_us(5);				//500us
	SET_SDA();
}
/*====================================================
Rotina:I2C_Control_Read
Função:Ĝera um sinal de leitura para o RTC com endereço definido
Chamada:I2C_Control_Read(<endereço que se deseja ler>
Comentário:Rotina encerra comunicação após a leitura do dado
Exemplo:
-----------------------------------------------------*/
uint8_t RTC_Read(uint8_t Address_Data)
{
	uint8_t y;

	SET_SDA();			//começa em Alto para facilitar
	SET_SCL();

/*--------------------------------------------
				Gerando Start
----------------------------------------------*/

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		Config_Port_Bit(SDA,OUTPUT);		//Escrita
		CLR_SDA();
	}
	CLR_SCL();

	/*----------------------------------------------
		Enviando 7 bits do endereço do dispositivo modo escrita
	----------------------------------------------*/


	y = (RTC_Address << 1);		//Desloca para esquerda
	y &= (0b11111110);			//Escrita

	Mask = 0x80;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(nClock = 0; nClock < 8; nClock++)
		{
			Mask = 0x80;
			Mask = (Mask >> nClock);

			if((y & Mask) != 0)
			SET_SDA()
			else
			CLR_SDA();
			//_delay_us(5);		//100us
			CLOCK();
		}
	}
	/*---------------------------------------
		Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	Config_Port_Bit(SDA,INPUT);	//Garante Port modo leitura antes do disposivo enviar dado
	//_delay_us(5);		//100us
	CLOCK();
	//_delay_us(5);		//500us
	/*---------------------------------------------
		Enviando 8 bits do endereço de dados
	----------------------------------------------*/
	Mask = 0x80;
	Config_Port_Bit(SDA,OUTPUT);		//Escrita
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(nClock = 0; nClock < 8; nClock++)
		{
			Mask = 0x80;
			Mask = (Mask >> nClock);

			if((Address_Data & Mask) != 0)
			SET_SDA()
			else
			CLR_SDA();

			CLOCK();
		}
	}
	/*---------------------------------------
		Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{ DDRC &= (0b11101111);} //SDA Leitura (DIMINUI TEMPO DE CONFLITO ENTRE PINOS SDA)
	CLOCK();
/*--------------------------------------------
	Gerando Restart
----------------------------------------------*/
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		Config_Port_Bit(SDA,OUTPUT);		//Escrita
		SET_SDA();			//começa em alto para facilitar
	}
	SET_SCL();
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		Config_Port_Bit(SDA,OUTPUT);		//Escrita
		CLR_SDA();
	}
	CLR_SCL();

	/*----------------------------------------------
		Enviando 7 bits do endereço do dispositivo no modo leitura
	----------------------------------------------*/

	y = (RTC_Address << 1);		//Desloca para esquerda
	y |= (0b00000001);				//Leitura
	Mask = 0x80;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(nClock = 0; nClock < 8; nClock++)
		{
			Mask = 0x80;
			Mask = (Mask >> nClock);

			if((y & Mask) != 0)
			SET_SDA()
			else
			CLR_SDA()

			//_delay_us(5);		//100us

			CLOCK();
		}
	}
	/*---------------------------------------
	Gerando um  clock para o ACK do receprtor
	---------------------------------------*/
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{ DDRC &= (0b11101111); } //SDA Leitura (DIMINUI TEMPO DE CONFLITO ENTRE PINOS SDA)
	CLOCK();
	/*------------------------------------
	Gerando 8 clocks de leitura de dados
	-------------------------------------*/
	LeitRTC = 0x00;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(nClock = 0; nClock < 8; nClock++)
		{
			Mask = 0x80;
			Mask = (Mask >> nClock);

			if((PINC & 0b0010000) != 0)
			LeitRTC |= Mask;
			else
			LeitRTC &= ~Mask;

			CLOCK();
		}
	}
	/*------------------------------------
			Gerando um Nack
	----------------------------------*/
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		Config_Port_Bit(SDA,OUTPUT);		//Escrita
		SET_SDA();
	}
	/*---------------------------------
		Gerando um Clock
	----------------------------------*/
	CLOCK();
	/*--------------------------------------
	Gerando um STOP
	---------------------------------------*/

	CLR_SDA();

	SET_SCL();

	SET_SDA();

	return (LeitRTC);
}
#endif // _000_

 /**========================================================================
 * Rotina:
 * Função:
 * Chamada:
 * Retorna:
 * Comentário:
 * ----------------------------------------------------------------------*/


