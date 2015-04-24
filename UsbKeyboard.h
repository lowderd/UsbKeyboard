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

#include "usb_keymap.h"
#include "unicode_chars.h"

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
// const PROGMEM char usbHidReportDescriptor[35] = { /* USB report descriptor */
//   0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
//   0x09, 0x06,                    // USAGE (Keyboard) 
//   0xa1, 0x01,                    // COLLECTION (Application) 
//   0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
//   0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
//   0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
//   0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
//   0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
//   0x75, 0x01,                    //   REPORT_SIZE (1) 
//   0x95, 0x08,                    //   REPORT_COUNT (8) 
//   0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
//   0x95, BUFFER_SIZE-1,           //   REPORT_COUNT (simultaneous keystrokes) 
//   0x75, 0x08,                    //   REPORT_SIZE (8) 
//   0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
//   0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
//   0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
//   0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
//   0xc0                           // END_COLLECTION 
// };

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
#define KEY_F12             0x45    // Keyboard F12

#define KEY_PRINT_SCREEN    0x46    // Keyboard Print Screen
#define KEY_SCROLL_LOCK     0x47    // Keyboard Scroll Lock
#define KEY_PAUSE           0x48    // Keyboard Pause
#define KEY_INSERT          0x49    // Keyboard Insert
#define KEY_HOME            0x4A    // Keyboard Home
#define KEY_PAGE_UP         0x4B    // Keyboard Page Up
#define KEY_DELETE          0x4C    // Keyboard Delete
#define KEY_END             0x4D    // Keyboard End
#define KEY_PAGE_DOWN       0x4E    // Keyboard Page Down

#define KEY_RIGHT_ARROW     0x4F    // Keyboard Right Arrow
#define KEY_LEFT_ARROW      0x50    // Keyboard Left Arrow
#define KEY_DOWN_ARROW      0x51    // Keyboard Down Arrow
#define KEY_UP_ARROW        0x52    // Keyboard Up Arrow
#define KEY_NUM_LOCK        0x53    // Keyboard Num Lock

#define KEY_NUM_SLASH       0x54    // Keyboard Num Pad /
#define KEY_NUM_ASTERISK    0x55    // Keyboard Num Pad *
#define KEY_NUM_DASH        0x56    // Keyboard Num Pad -
#define KEY_NUM_PLUS        0x57    // Keyboard Num Pad +
#define KEY_NUM_ENTER       0x58    // Keyboard Num Pad Enter

#define KEY_PAD_1           0x59    // Keyboard Num Pad 1 and End
#define KEY_PAD_2           0x5A    // Keyboard Num Pad 2 and Down Arrow
#define KEY_PAD_3           0x5B    // Keyboard Num Pad 3 and Page Down
#define KEY_PAD_4           0x5C    // Keyboard Num Pad 4 and Left Arrow
#define KEY_PAD_5           0x5D    // Keyboard Num Pad 5 
#define KEY_PAD_6           0x5E    // Keyboard Num Pad 6 and Right Arrow
#define KEY_PAD_7           0x5F    // Keyboard Num Pad 7 and Home
#define KEY_PAD_8           0x60    // Keyboard Num Pad 8 and Up Arrow
#define KEY_PAD_9           0x61    // Keyboard Num Pad 9 and Page Up
#define KEY_PAD_0           0x62    // Keyboard Num Pad 0 and Insert
#define KEY_PAD_PERIOD      0x63    // Keyboard Num Pad .

#define KEY_APP             0x65    // Keyboard Application
#define KEY_PWR             0x66    // Keyboard Power
#define KEY_NUM_PAD_EQUAL   0x67    // Keyboard Num Pad Equal

#define KEY_F13             0x68    // Keyboard F13
#define KEY_F14             0x69    // Keyboard F14
#define KEY_F15             0x6A    // Keyboard F15
#define KEY_F16             0x6B    // Keyboard F16
#define KEY_F17             0x6C    // Keyboard F17
#define KEY_F18             0x6D    // Keyboard F18
#define KEY_F19             0x6E    // Keyboard F19
#define KEY_F20             0x6F    // Keyboard F20
#define KEY_F21             0x70    // Keyboard F21
#define KEY_F22             0x71    // Keyboard F22
#define KEY_F23             0x72    // Keyboard F23
#define KEY_F24             0x73    // Keyboard F24

#define KEY_EXECUTE         0x74    // Keyboard Execute
#define KEY_HELP            0x75    // Keyboard Help
#define KEY_MENU            0x76    // Keyboard Menu
#define KEY_SELECT          0x77    // Keyboard Select
#define KEY_STOP            0x78    // Keyboard Stop
#define KEY_AGAIN           0x79    // Keyboard Again
#define KEY_UNDO            0x7A    // Keyboard Undo
#define KEY_CUT             0x7B    // Keyboard Cut
#define KEY_COPY            0x7C    // Keyboard Copy
#define KEY_PASTE           0x7D    // Keyboard Paste
#define KEY_FIND            0x7E    // Keybaord Find
#define KEY_MUTE            0x7F    // Keyboard Mute
#define KEY_VOL_UP          0x80    // Keyboard Volume Up
#define KEY_VOL_DOWN        0x81    // Keyboard Volume Down

#define KEY_PLAY            0xB0    // Keyboard Play
#define KEY_PAUSE           0xB1    // Keyboard Pause
#define KEY_RECORD          0xB2    // Keyboard Record
#define KEY_FAST_FORWARD    0xB3    // Keyboard Fast Forward
#define KEY_REWIND          0xB4    // Keyboard Rewind
#define KEY_NEXT_TRACK      0xB5    // Keyboard Scan Next Track
#define KEY_PREVIOUS_TRACK  0xB6    // Keyboard Scan Previous Track
#define KEY_STOP            0xB7    // Keyboard Stop


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

  void sendUnicodeKeyStroke(uint8_t *keyStrokes) {
    sendUnicodeKeyStroke(keyStrokes, 6);
  }

  void sendUnicodeKeyStroke(uint8_t *keyStrokes, uint8_t size) {

    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke 
      //       so we know the previous keystroke was
      //       sent. 
    }

    memset(reportBuffer, 0, sizeof(reportBuffer));

    for(uint8_t i=0; i<size; i++) {
        reportBuffer[0] = MOD_ALT_LEFT;
        reportBuffer[1] = keyStrokes[i];

        usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

        while (!usbInterruptIsReady()) {
          // Note: We wait until we can send keystroke 
          //       so we know the previous keystroke was
          //       sent. 
        }
    }
    

    // This stops endlessly repeating keystrokes:
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }

  void sendConsumerKeyStroke(uint8_t keyStroke) {
    sendKeyStroke(keyStroke, 0);
  }

  void sendConsumerKeyStroke(uint8_t keyStroke, uint8_t modifiers) {
      
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
