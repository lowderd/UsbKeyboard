//*****************************************************************************
//*     unicode_chars Header                                                  *    
//*****************************************************************************
//
//      This file contains arrays containing the bytes needed to be sent for
//		different unicode characters to be created. These arrays can be used
//		with the sendUnicodeKeyStroked function within the UsbKeyboard.h 
//		library to create different unicode characters
//
//      Author: Duncan Lowder
//      E-Mail: duncan.lowder@gmail.com
//      Date: 2015-04-13
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
//
//*****************************************************************************

#ifndef UNICODE_CHARS 
#define UNICODE_CHARS(o)

uint8_t exclamation_mark[6] = {KEY_PAD_0, KEY_PAD_0, KEY_PAD_0, KEY_PAD_0, KEY_PAD_3, KEY_PAD_3};
uint8_t left_bracket[6] = {KEY_PAD_0, KEY_PAD_0, KEY_PAD_0, KEY_PAD_1, KEY_PAD_2, KEY_PAD_3};
uint8_t left_parenthesis[2] = {KEY_PAD_4, KEY_PAD_0};
uint8_t arc_up_and_left[4] = {KEY_PAD_9, KEY_PAD_5, KEY_PAD_8, KEY_PAD_3};
uint8_t degree_sign[3] = {KEY_PAD_1, KEY_PAD_7, KEY_PAD_6};
uint8_t white_square[4] = {KEY_PAD_9, KEY_PAD_6, KEY_PAD_3, KEY_PAD_3};
uint8_t right_parenthesis[2] = {KEY_PAD_4, KEY_PAD_1};
uint8_t vertical_left_parenthesis[5] = {KEY_PAD_6, KEY_PAD_5, KEY_PAD_0, KEY_PAD_7, KEY_PAD_7};
uint8_t space[2] = {KEY_PAD_3, KEY_PAD_2};
uint8_t	heavy_up_horizontal[4] = {KEY_PAD_9, KEY_PAD_5, KEY_PAD_3, KEY_PAD_1};
uint8_t heavy_horizontal[4] = {KEY_PAD_9, KEY_PAD_4, KEY_PAD_7, KEY_PAD_3};

#endif // UNICODE_CHARS 