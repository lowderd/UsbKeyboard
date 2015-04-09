//*****************************************************************************
//*     UsbKeyboard Library                                                   *    
//*****************************************************************************
//
//      This file contains an implementation of a USB HID Keyboard using the
//      V-USB Library from Objective Development. This header file provides
//      Arduino access to the usbdrv library. 
//
//      This implementation heavily leverages the implementation from 
//      rancidbacon, the original library is available here: 
//      https://code.google.com/p/vusb-for-arduino/
//
//      NOTE: Depending on the version of Arduino that is being used, it 
//      may be necessary to compile the V-USB library prior to compiling
//      the Arduino sketch. Please see the ArduinoNotes.txt file for more
//      information. 
//
//      Author: Duncan Lowder
//      E-Mail: duncan.lowder@gmail.com
//      Date: 2015-04-07
//      License: GNU GPL v2
//
//      Copyright (C) 2015  Duncan Lowder
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <http://www.gnu.org/licenses/>.
//*****************************************************************************
#ifndef __UsbKeyboard_h__
#define __UsbKeyboard_h__

// --- INCLUDES ---------------------------------------------------------------

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

extern "C" {
  #include "usbdrv.h"
}

// --- TYPE DEFINITIONS -------------------------------------------------------

#define BUFFER_SIZE 4 // Minimum of 2: 1 for modifiers + 1 for keystroke 


static uchar    idleRate;           // in 4 ms units 


/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses. 
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
const PROGMEM char usbHidReportDescriptor[35] = { /* USB report descriptor */
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,                    // USAGE (Keyboard) 
  0xa1, 0x01,                    // COLLECTION (Application) 
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,                    //   REPORT_SIZE (1) 
  0x95, 0x08,                    //   REPORT_COUNT (8) 
  0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
  0x95, BUFFER_SIZE-1,           //   REPORT_COUNT (simultaneous keystrokes) 
  0x75, 0x08,                    //   REPORT_SIZE (8) 
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
  0xc0                           // END_COLLECTION 
};



/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A               0x04    // Keyboard a and A
#define KEY_B               0x05    // Keyboard b and B
#define KEY_C               0x06    // Keyboard c and C
#define KEY_D               0x07    // Keyboard d and D
#define KEY_E               0x08    // Keyboard e and E
#define KEY_F               0x09    // Keyboard f and F 
#define KEY_G               0x0A    // Keyboard g and G
#define KEY_H               0x0B    // Keyboard h and H 
#define KEY_I               0x0C    // Keyboard i and I 
#define KEY_J               0x0D    // Keyboard j and J
#define KEY_K               0x0E    // Keyboard k and K
#define KEY_L               0x0F    // Keyboard l and L
#define KEY_M               0x10    // Keyboard m and M
#define KEY_N               0x11    // Keyboard n and N
#define KEY_O               0x12    // Keyboard o and O
#define KEY_P               0x13    // Keyboard p and P
#define KEY_Q               0x14    // Keyboard q and Q
#define KEY_R               0x15    // Keyboard r and R
#define KEY_S               0x16    // Keyboard s and S
#define KEY_T               0x17    // Keyboard t and T
#define KEY_U               0x18    // Keyboard u and U
#define KEY_V               0x19    // Keyboard v and V
#define KEY_W               0x1A    // Keyboard w and W
#define KEY_X               0x1B    // Keyboard x and X
#define KEY_Y               0x1C    // Keyboard y and Y
#define KEY_Z               0x1D    // Keyboard z and Z
    
#define KEY_1               0x1E    // Keyboard 1 and !
#define KEY_2               0x1F    // Keyboard 2 and @
#define KEY_3               0x20    // Keyboard 3 and #
#define KEY_4               0x21    // Keyboard 4 and $
#define KEY_5               0x22    // Keyboard 5 and % 
#define KEY_6               0x23    // Keyboard 6 and ^ 
#define KEY_7               0x24    // Keyboard 7 and &
#define KEY_8               0x25    // Keyboard 8 and * 
#define KEY_9               0x26    // Keyboard 9 and (
#define KEY_0               0x27    // Keyboard 0 and )
    
#define KEY_ENTER           0x28    // Keyboard Return (ENTER)
#define KEY_ESCAPE          0x29    // Keyboard Escape
#define KEY_BACKSPACE       0x2A    // Keyboard Backspace
#define KEY_TAB             0x2B    // Keyboard Tab
#define KEY_SPACE           0x2C    // Keyboard Space
    
#define KEY_DASH            0x2D    // Keyboard - and _
#define KEY_EQUAL           0x2E    // Keyboard = and +
#define KEY_LEFT_BRACKET    0x2F    // Keyboard [ and {
#define KEY_RIGHT_BRACKET   0x30    // Keyboard ] and }
#define KEY_BACK_SLASH      0x31    // Keyboard \ and |
#define KEY_COLON           0x33    // Keyboard ; and :
#define KEY_QUOTE           0x34    // Keyboard ' and "
#define KEY_TILDE           0x35    // Keyboard ` and ~
#define KEY_COMMA           0x36    // Keyboard , and <
#define KEY_PERIOD          0x37    // Keyboard . and >
#define KEY_FORWARD_SLASH   0x38    // Keyboard / and ?

#define KEY_CAPS_LOCK       0x39    // Keyboard CAPS LOCK

#define KEY_F1              0x3A    // Keyboard F1
#define KEY_F2              0x3B    // Keyboard F2
#define KEY_F3              0x3C    // Keyboard F3
#define KEY_F4              0x3D    // Keyboard F4
#define KEY_F5              0x3E    // Keyboard F5
#define KEY_F6              0x3F    // Keyboard F6
#define KEY_F7              0x40    // Keyboard F7
#define KEY_F8              0x41    // Keyboard F8
#define KEY_F9              0x42    // Keyboard F9
#define KEY_F10             0x43    // Keyboard F10
#define KEY_F11             0x44    // Keyboard F11



class UsbKeyboardDevice {
 public:
  UsbKeyboardDevice () {
    PORTD = 0; // TODO: Only for USB pins?
    DDRD |= ~USBMASK;

    cli();
    usbDeviceDisconnect();
    usbDeviceConnect();


    usbInit();
      
    sei();

    // TODO: Remove the next two lines once we fix
    //       missing first keystroke bug properly.
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
    
  void update() {
    usbPoll();
  }
    
  void sendKeyStroke(uint8_t keyStroke) {
    sendKeyStroke(keyStroke, 0);
  }

  void sendKeyStroke(uint8_t keyStroke, uint8_t modifiers) {
      
    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    }
      
    memset(reportBuffer, 0, sizeof(reportBuffer));

    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyStroke;
        
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    }
      
    // This stops endlessly repeating keystrokes:
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

  }
    
  //private: TODO: Make friend?
  uchar    reportBuffer[4];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]

};

UsbKeyboardDevice UsbKeyboard = UsbKeyboardDevice();

#ifdef __cplusplus
extern "C"{
#endif 
  // USB_PUBLIC uchar usbFunctionSetup
uchar usbFunctionSetup(uchar data[8]) 
  {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = UsbKeyboard.reportBuffer; //
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
      /* class request type */

      if(rq->bRequest == USBRQ_HID_GET_REPORT){
	/* wValue: ReportType (highbyte), ReportID (lowbyte) */

	/* we only have one report type, so don't look at wValue */
        // TODO: Ensure it's okay not to return anything here?    
	return 0;

      }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
	//            usbMsgPtr = &idleRate;
	//            return 1;
	return 0;
      }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
	idleRate = rq->wValue.bytes[1];
      }
    }else{
      /* no vendor specific requests implemented */
    }
    return 0;
  }
#ifdef __cplusplus
} // extern "C"
#endif


#endif // __UsbKeyboard_h__
