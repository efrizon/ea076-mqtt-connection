/* ###################################################################
**     Filename    : main.c
**     Project     : exp1
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-12-21, 00:18, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "MCUC1.h"
#include "ESP.h"
#include "ASerialLdd3.h"
#include "PC.h"
#include "ASerialLdd2.h"
#include "LEDR.h"
#include "LEDpin4.h"
#include "BitIoLdd4.h"
#include "LEDG.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "LEDB.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
volatile uint8_t i = 0; /**<@brief Índice para posições de arrays em geral. */
volatile uint8_t j = 0; /**<@brief Índice para posições de arrays em geral. */

volatile char pMsg[100] = {0}; /**<@brief Mensagem do PC, com máximo de 100 caracteres. */
volatile char eMsg[100] = {0}; /**<@brief Mensagem do ESP, com máximo de 100 caracteres. */

volatile char pFlag = 0; /**<@brief Flag de mensagem do PC pronta. */
volatile char eFlag = 0; /**<@brief Flag de mensagem do ESP pronta. */

char cStr[20] = {0}; /**<@brief Substring de comando pertencente à mensagem recebida pelo ESP (Ex.: "MENSAGEM"). */
char tStr[50] = {0}; /**<@brief Substring de tópico pertencente à mensagem recebida pelo ESP. */
char pStr[10] = {0}; /**<@brief Substring de parâmetro pertencente à mensagem recebida pelo ESP. */


/**
 * @brief 
 * Envia a string colocada como parâmetro para o terminal do PC 
 * por meio de uma varredura em todos os caracteres da string
 * e enviando-os utilizando a função PC_SendChar() gerada pelo Processor Expert.
 * */
void SendP(char *s) {
	while(*s) {
		while(PC_SendChar(*s));
		s++;
	}
}

/**
 * @brief 
 * Envia a string colocada como parâmetro para o módulo ESP 
 * por meio de uma varredura em todos os caracteres da string
 * e enviando-os utilizando a função ESP_SendChar() gerada pelo Processor Expert.
 * */
void SendE(char *s) {
	while(*s) {
		while(ESP_SendChar(*s));
		s++;
	}
}

/** 
 * @brief
 * Realiza a inicialização automática utilizando a sintaxe do protocolo MQTT, ou seja,
 * envia iterativamente os seguintes comandos ao módulo ESP na sequência abaixo:
 * 
 * 1. Conecta-se ao Wi-Fi;
 * 2. Conecta-se ao MQTT;
 * 3. Inscreve-se no tópico LEDR;
 * 4. Inscreve-se no tópico LEDG;
 * 5. Inscreve-se no tópico LEDB;
 * 
 * Em seguida, envia para o terminal do PC uma mensagem de configurações prontas.
 * Caso aconteça falha em algum dos passos acima listados, uma tentativa é feita
 * novamente desde o início a partir da próxima iteração.
 * */
void AutoInit(void) {
	int a = 0;
	while(1) {
		if(a == 0) {
			SendE("CONNWIFI \"wifi-network\",\"wifi-password\"\r\n");
		} else if(a == 1) {
			SendE("CONNMQTT \"host-name\",1883,\"mac-address\"\r\n");
		} else if(a == 2) {
			SendE("SUBSCRIBE \"EA076/166974/LEDR\"\r\n");
		} else if(a == 3) {
			SendE("SUBSCRIBE \"EA076/166974/LEDG\"\r\n");
		} else if(a == 4) {
			SendE("SUBSCRIBE \"EA076/166974/LEDB\"\r\n");
		} else if(a == 5) {
			SendP("SETTINGS READY\r\n");
			break;
		}
		
		while(!eFlag);
		eFlag = 0;
		SendP(eMsg);
		
		if(!strcmp(eMsg, "CONNECT WIFI\r\n") || !strcmp(eMsg, "CONNECT MQTT\r\n") || !strcmp(eMsg, "OK SUBSCRIBE\r\n")) {
			a++;
		} else if(!strcmp(eMsg, "ERROR WIFI\r\n") || !strcmp(eMsg, "NO WIFI\r\n") || !strcmp(eMsg, "NOT CONNECTED\r\n")) {
			a = 0;
		}
	}
}
/**
 * @brief
 * Realiza uma varredura na mensagem recebida pelo módulo ESP,
 * repartindo-a em três substrings:
 * 
 * 1. Substring de comando (cStr);
 * 2. Substring de tópico (tStr);
 * 3. Substring de parâmetro (pStr);
 */
void ParseMsg(void) {
	uint8_t i = 0;
	uint8_t j = 0;
	
	while(eMsg[i] != ' ') {
		cStr[i] = eMsg[i]; 
		i++;
	}
	i++;
	cStr[i] = 0;
	i++;
	while(eMsg[i] != ']') {
		tStr[j] = eMsg[i];
		i++;
		j++;
	}
	tStr[j++] = 0;
	j = 0;
	while(eMsg[i] != '[') {
		i++;
	}
	i++;
	while(eMsg[i] != ']') {
		pStr[j] = eMsg[i];
		i++;
		j++;
	}
	pStr[j++] = 0;
}
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */


/**
 * @brief
 * A função main realiza as seguintes funções:
 * 
 * 1. Inicializa os componentes do Procesor Expert;
 * 2. Inicializa automaticamente a conexão MQTT (Função AutoInit());
 * 3. Entra em um loop infinito:
 * 	- Se uma nova mensagem é digitada, ela é enviada ao terminal do PC (Interrupção PC_OnRxChar()) e quando finalizada é enviada ao módulo ESP (Função SendE()).
 * 	- Se uma mensagem é recebida pelo módulo ESP (Interrupção ESP_OnRxChar()), é realizada sua separação em strings
 * 	de tópico e parâmetro (Função ParseMsg()), acendendo ou desligando os LEDs vermelho, verde e azul. 
 */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  
  AutoInit();
  
  while(1) {
	  if(pFlag) {
		  pFlag = 0;
		  while(PC_SendChar(0x0A));
		  SendE(pMsg);
	  }
		  
	  if(eFlag) {
		  eFlag = 0;
		  SendP(eMsg);
		  
		  ParseMsg();
		  
		  if(strcmp(cStr, "MESSAGE")) {
			  //SendP(eMsg);
		  } else if(!strcmp(tStr, "EA076/166974/LEDR") && !strcmp(pStr, "ON")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDR ON\"\r\n");
			  LEDR_On();
		  } else if(!strcmp(tStr, "EA076/166974/LEDR") && !strcmp(pStr, "OFF")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDR OFF\"\r\n");
			  LEDR_Off();
		  } else if(!strcmp(tStr, "EA076/166974/LEDG") && !strcmp(pStr, "ON")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDG ON\"\r\n");
			  LEDG_On();
		  } else if(!strcmp(tStr, "EA076/166974/LEDG") && !strcmp(pStr, "OFF")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDG OFF\"\r\n");
			  LEDG_Off();
		  } else if(!strcmp(tStr, "EA076/166974/LEDB") && !strcmp(pStr, "ON")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDB ON\"\r\n");
			  LEDB_On();
		  } else if(!strcmp(tStr, "EA076/166974/LEDB") && !strcmp(pStr, "OFF")) {
			  SendE("PUBLISH \"EA076/166974/reply\",\"LEDB OFF\"\r\n");
			  LEDB_Off();
		  }
	  }
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
