/* ###################################################################
**     Filename    : Events.c
**     Project     : exp1
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-12-21, 00:18, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/** @brief Interrupção não utilizada. */
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PC_OnError (module Events)
**
**     Component   :  PC [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/** @brief Interrupção não utilizada. */
void PC_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PC_OnRxChar (module Events)
**
**     Component   :  PC [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/


/**
 * @brief
 * Esta interrupção é acionada toda vez que um caracter é digitado,
 * realizando as seguintes funções:
 * 
 * 1. Envia o caracter para o terminal do PC;
 * 2. Coloca o caracter na próxima posição vazia do buffer de mensagem do PC (pMsg), exceto quando o caracter é um Backspace, nesse caso a posição no buffer retorna uma unidade.
 * Se o caracter recebido for um Carriage Return, adiciona o Line Feed na próxima posição, e finaliza a string com valor nulo;
 * 3. Aciona a flag de mensagem pronta (pFlag).
*/
void PC_OnRxChar(void)
{
  /* Write your code here ... */
	extern volatile uint8_t i;
	extern volatile char pMsg[100];
	extern volatile char pFlag;
	uint8_t c;
	
	PC_RecvChar(&c);           // Le um caracter recebido pelo pc e armazena na variavel c
	
	if(c == 0x08) {            // se o caracter for 0x08 (Backspace)...
		while(PC_SendChar(c)); // Transmite para o terminal
		i--;
	} else {                   // Senao...
		while(PC_SendChar(c)); // Transmite para o terminal
		pMsg[i] = c;           // Adiciona a string
		i++;
	}
	
	if(c == 0x0D) {     // Se o caracter for 0x0D (Carriage return)...
		pMsg[i] = 0x0A; // Adiciona 0x0A (Line feed) na string
		i++;
		pMsg[i] = 0;    // Adiciona NULL na string
		pFlag = 1;      // Ativa a flag (aciona o envio do input ao ESP em main.c)
		i = 0;
	}
}

/*
** ===================================================================
**     Event       :  PC_OnTxChar (module Events)
**
**     Component   :  PC [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/** @brief Interrupção não utilizada. */
void PC_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  ESP_OnError (module Events)
**
**     Component   :  ESP [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/** @brief Interrupção não utilizada. */
void ESP_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  ESP_OnRxChar (module Events)
**
**     Component   :  ESP [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/**
 * @brief
 * Esta interrupção é acionada toda vez que um caracter é recebido pelo módulo ESP,
 * realizando as seguintes funções:
 * 
 * 1. Coloca o caracter na próxima posição vazia do buffer de mensagem do ESP (eMsg). 
 * Se o caracter recebido for um Carriage Return, adiciona o Line Feed na próxima posição, e finaliza a string com valor nulo;
 * 2. Aciona a flag de mensagem pronta (eFlag).
*/
/* ===================================================================*/
void ESP_OnRxChar(void)
{
  /* Write your code here ... */
	extern volatile uint8_t i;
	extern volatile char eMsg[100];
	extern volatile char eFlag;
	uint8_t c;
	
	ESP_RecvChar(&c);       // Chama funcao criada pelo Processor Expert que le o caracter recebido e coloca na variavel "c"
	eMsg[i] = c;
	i++;
	if(c == 0x0A){          // Se 0x0A foi encontrado, entao...
		eMsg[i] = 0;       // ... o ultimo caracter da string é o valor nulo
		i = 0;
		eFlag = 1;            // ... a flag msg é sinalizada (mensagem pronta)
	}
}

/*
** ===================================================================
**     Event       :  ESP_OnTxChar (module Events)
**
**     Component   :  ESP [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/** @brief Interrupção não utilizada. */
void ESP_OnTxChar(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
