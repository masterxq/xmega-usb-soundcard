#include "osc.h"
#include <avr/pgmspace.h>
#ifndef F_CPU
 #include <avr/iox128a1u.h>
#endif

void CCPWrite( volatile uint8_t * address, uint8_t value )
{
#ifdef __ICCAVR__
  asm("movw r30, r16");
#ifdef RAMPZ
  RAMPZ = 0;
#endif
  asm("ldi  r16,  0xD8 \n"
      "out  0x34, r16  \n"
      "st     Z,  r18  \n");

#elif defined __GNUC__
  volatile uint8_t * tmpAddr = address;
#ifdef RAMPZ
  RAMPZ = 0;
#endif
  asm volatile(
    "movw r30,  %0" "\n\t"
    "ldi  r16,  %2" "\n\t"
    "out   %3, r16" "\n\t"
    "st     Z,  %1"
    :
    : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "m" (CCP)
    : "r16", "r30", "r31"
    );
#endif
}


void ext_osc_init(void)
{
 /*Oscillator auf externen Oscilator einstellen*/
 OSC.CTRL |= OSC_XOSCEN_bm;
 /*Wenn Oscillator stabil wird das Flag XOSCRDY
  * gesetzt und der externe Oscilator kann benutzt werden*/
 while (!(OSC.STATUS & OSC_XOSCRDY_bm)) {};
 /*Clock auf 32Mhz einstellen*/
 CCPWrite(&CLK.CTRL, CLK_SCLKSEL_XOSC_gc);
 /* auto kalibierung ein */
 //DFLLRC32M.CTRL = DFLL_ENABLE_bm; //Nur für internen
}

void int48_ppl_overclock(void)
{
  OSC.PLLCTRL = 0 | OSC_PLLFAC4_bm | OSC_PLLFAC3_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while ( !(OSC.STATUS & OSC_PLLRDY_bm) )
    ;

  CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);

}

#ifndef _AVR_IOXXX_H_
 #include <avr/iox128a1u.h>
#endif

void int48_ppl_overclock_PER2_2x(void)
{
  OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;

  /*Wenn Oscillator stabil wird das Flag RC32MRDY
  * gesetzt und 32Mhz können benutzt werden*/
  while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm))
    ;
  
  OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | 12; //Select source for PLL and multiply with 12 (RC32M will be dived by 4) 32/4 * 12 = 96 MHz
  
  CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_2_gc); //clk_sys -> diva(1) -> divb(1) -> divc(2) -> cpu_clk = 48Mhz
  
  OSC.CTRL |= OSC_PLLEN_bm;
  
  while (!(OSC.STATUS & OSC_PLLRDY_bm)) //Wait for PLL is ready
    ;
  
  CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);
}



// void int6x_ppl_overclock_PER2_2x_USB48(uint8_t x)
// {
// 
//   //kalibierungs referenz quelle einstellen
//   OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
//   //Hole kalibierungs wert aus production row und setze es ein -> RC32 = 48 32Mhz
//   NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
//   DFLLRC32M.CALA = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSCA));
//   NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
//   DFLLRC32M.CALB = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
//   
//   //Irgendwas setzen siehe au manual 4.17.19??!! Spinner! Egal soll helfen
//   DFLLRC32M.COMP1 = 0x1B; //Xmega AU manual, 4.17.19
//   DFLLRC32M.COMP2 = 0xB7;
//   
//   //Anschalten
//   uint8_t t = OSC.CTRL;
//   CCPWrite(&OSC.CTRL, t | OSC_RC32MEN_bm | OSC_RC32KEN_bm);
// 
//   /*Wenn Oscillator stabil wird das Flag RC32MRDY
//   * gesetzt und 32Mhz können benutzt werden*/
//   while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm))
//     ;
// 
//   //Schaltet die kalibrierung an, ist jedoch ein bisschen fragwuerdig da referenz quelle aktiv und stabil sein sollte (USBSOF?)
//   DFLLRC32M.CTRL = DFLL_ENABLE_bm;
//   
//   
// 
// 
//   //System CLK setzen
//   OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | x;                       //Sample: (x == 6) Select source for PLL and multiply with 6 (RC32M will be dived by 4) 48/4 * 6 = 72 MHz
//   CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_2_gc); //        CLK_SYS -> diva(1) -> PER4 = 72MHz -> divb(1) -> PER2 = 72MHz -> divc(2) -> cpu_clk = 36Mhz
//   
//   //PLL Aktivieren
//   t = OSC.CTRL;
//   CCPWrite(&OSC.CTRL, t | OSC_PLLEN_bm);
//   
//   while (!(OSC.STATUS & OSC_PLLRDY_bm)) //Wait for PLL is ready
//     ;
//   
//   //PLL Auswaehlen
//   CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);
//   
//   CLK.USBCTRL = (CLK_USBPSDIV_1_gc | CLK_USBSRC_RC32M_gc | CLK_USBSEN_bm);
// 
// }

void int2x_USB48(uint8_t x6_P2_x12)
{
// 	while(1)
// 	{
// 	  _delay_ms(1000);
// 	  PORTE.OUTTGL = PIN0_bm;
  //kalibierungs referenz quelle einstellen
  OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
  //Hole kalibierungs wert aus production row und setze es ein -> RC32 = 48 32Mhz
  NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
  DFLLRC32M.CALA = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSCA));
  NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
  DFLLRC32M.CALB = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
  
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
  //Irgendwas setzen siehe au manual 4.17.19??!! Spinner! Egal soll helfen
  DFLLRC32M.COMP1 = 0x1B; //Xmega AU manual, 4.17.19
  DFLLRC32M.COMP2 = 0xB7;
	
	//Schaltet die kalibrierung an, ist jedoch ein bisschen fragwuerdig da referenz quelle aktiv und stabil sein sollte (USBSOF?)
  DFLLRC32M.CTRL = DFLL_ENABLE_bm;
  
  //Alle benötigten clks Anschalten
  uint8_t t = OSC.CTRL;
  CCPWrite(&OSC.CTRL, t | OSC_RC32MEN_bm | OSC_RC32KEN_bm | OSC_RC2MEN_bm);

  
  /*Wenn Oscillator stabil wird das Flag RC32MRDY
  * gesetzt und 32Mhz können benutzt werden*/
  while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC2MRDY_bm | OSC_RC32KRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC2MRDY_bm | OSC_RC32KRDY_bm))
    ;

  
  
  

  CCPWrite(&CLK.USBCTRL, (CLK_USBPSDIV_1_gc | CLK_USBSRC_RC32M_gc | CLK_USBSEN_bm));

  //System CLK setzen
  OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | x6_P2_x12;                       //Sample: (x == 6) Select source for PLL and multiply with 6 (RC32M will be dived by 4) 48/4 * 6 = 72 MHz
  CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_2_gc); //        CLK_SYS -> diva(1) -> PER4 = 72MHz -> divb(1) -> PER2 = 72MHz -> divc(2) -> cpu_clk = 36Mhz
  
  //PLL Aktivieren
  t = OSC.CTRL;
  CCPWrite(&OSC.CTRL, t | OSC_PLLEN_bm);
  
  while (!(OSC.STATUS & OSC_PLLRDY_bm)) //Wait for PLL is ready
    ;
  
  DFLLRC32M.CTRL = DFLL_ENABLE_bm;
  
  //PLL Auswaehlen
  CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);

}


void int48_ppl_USB48(void)
{

  //kalibierungs referenz quelle einstellen
  OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
  //Hole kalibierungs wert aus production row und setze es ein -> RC32 = 48 32Mhz
  NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
  DFLLRC32M.CALB = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
  NVM.CMD = NVM_CMD_NO_OPERATION_gc;
  //Irgendwas setzen siehe au manual 4.17.19??!! Spinner! Egal soll helfen
  DFLLRC32M.COMP1 = 0x1B;
  DFLLRC32M.COMP2 = 0xB7;
  
  //Schaltet die kalibrierung an, ist jedoch ein bisschen fragwuerdig da referenz quelle aktiv und stabil sein sollte (USBSOF?)
  DFLLRC32M.CTRL = DFLL_ENABLE_bm;
  
  
  //Anschalten
  CCPWrite(&OSC.CTRL, OSC_RC32MEN_bm | OSC_RC2MEN_bm | OSC_RC32KEN_bm);

  /*Wenn Oscillator stabil wird das Flag RC32MRDY
  * gesetzt und 32Mhz können benutzt werden*/
  while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm))
    ;

  //System CLK setzen
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | 24; //Select source for PLL and multiply with 16...  2 * 16 = 32 MHz
  CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc); //clk_sys -> diva(1) -> PER4 = 32MHz -> divb(1) -> PER2 = 32MHz -> divc(2) -> cpu_clk = 32Mhz
  
	DFLLRC2M.CTRL = DFLL_ENABLE_bm;
  //PLL Aktivieren
  uint8_t t = OSC.CTRL;
  CCPWrite(&OSC.CTRL, t | OSC_PLLEN_bm);
  
  while (!(OSC.STATUS & OSC_PLLRDY_bm)) //Wait for PLL is ready
    ;
  
  //PLL Auswaehlen
  CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);
  
  CLK.USBCTRL = (CLK_USBPSDIV_1_gc | CLK_USBSRC_RC32M_gc | CLK_USBSEN_bm);

}

void int32_ppl_PER2_2x(void)
{
  OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;

  /*Wenn Oscillator stabil wird das Flag RC32MRDY
  * gesetzt und 32Mhz können benutzt werden*/
  while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC32KRDY_bm))
    ;
  
  OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | 8; //Select source for PLL and multiply with 12 (RC32M will be dived by 4) 32/4 * 12 = 96 MHz
  
  CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_2_gc); //clk_sys -> diva(1) -> divb(1) -> divc(2) -> cpu_clk = 48Mhz
  
  OSC.CTRL |= OSC_PLLEN_bm;
  
  while (!(OSC.STATUS & OSC_PLLRDY_bm)) //Wait for PLL is ready
    ;
  
  CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc);
}

void int2x_ppl_USB48_2(uint8_t x)
{
	OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
	NVM.CMD  = NVM_CMD_READ_CALIB_ROW_gc;
	DFLLRC32M.CALB = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
	DFLLRC32M.COMP1 = 0x1B; //Xmega AU manual, 4.17.19
	DFLLRC32M.COMP2 = 0xB7;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;

	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	OSC.CTRL = OSC_RC32MEN_bm | OSC_RC2MEN_bm; // enable internal 32MHz oscillator

	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready

	OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | x; // 2MHz * 16 = 32MHz

	CCP = CCP_IOREG_gc;
	OSC.CTRL = OSC_RC32MEN_bm | OSC_PLLEN_bm | OSC_RC2MEN_bm ; // Enable PLL

	while(!(OSC.STATUS & OSC_PLLRDY_bm)); // wait for PLL ready

	DFLLRC2M.CTRL = DFLL_ENABLE_bm;

	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	CLK.CTRL = CLK_SCLKSEL_PLL_gc; // Select PLL
	CLK.PSCTRL = 0x00; // No peripheral clock prescaler
}

//Allways working USB Clock
void int2x_ppl_USB48(uint8_t x)
{
	unsigned char tmp, tmp2;


	/////////////////////////////////////////////////////////////////
	// get USBRCOSC
	NVM.CMD  = NVM_CMD_READ_CALIB_ROW_gc;
	tmp = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
	NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
	tmp2 = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSCA));
	/* Clean up NVM Command register. */ 
	NVM_CMD = NVM_CMD_NO_OPERATION_gc; 
	//
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// enable DFLL for 32MHz osz and trim to 48MHz sync with USB start of frame
	OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
	DFLLRC32M.CALB = tmp;
	DFLLRC32M.CALA = tmp2;
	DFLLRC32M.COMP1 = 0x1B; //Xmega AU manual, 4.17.19
	DFLLRC32M.COMP2 = 0xB7;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;
	//
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// enable 32 MHZ osz (trimmed to 48MHZ for usb)
	CCPWrite(&OSC.CTRL, OSC_RC32MEN_bm | OSC_RC2MEN_bm); // enable internal 32MHz oscillator
    
	while((OSC.STATUS & (OSC_RC32MRDY_bm | OSC_RC2MRDY_bm)) != (OSC_RC32MRDY_bm | OSC_RC2MRDY_bm))
		; // wait for oscillator ready
	//
	/////////////////////////////////////////////////////////////////
	
	OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | x; // 2MHz * x = 2x MHz
    
	CCPWrite(&OSC.CTRL, OSC_RC32MEN_bm | OSC_PLLEN_bm | OSC_RC2MEN_bm); // Enable PLL
    
	while(!(OSC.STATUS & OSC_PLLRDY_bm))
		; // wait for PLL ready
    
	DFLLRC2M.CTRL = DFLL_ENABLE_bm;

	/////////////////////////////////////////////////////////////////
	// use PLL as systemclk
	CCPWrite(&CLK.CTRL, CLK_SCLKSEL_PLL_gc); // use PLL output as system clock 
	//
	/////////////////////////////////////////////////////////////////

//	CLK.CTRL = CLK_SCLKSEL_XOSC_gc;                      //Route Osc on Clock-tree
};
