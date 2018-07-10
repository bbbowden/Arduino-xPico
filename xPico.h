/*
  
	xPico.cpp - xPico Library
	Copyright (c) 2014 Brett Bowden.  All rights reserved.
  
	Rev 1.0 - October 13, 2014

	This library implements the mux interface to the Lantronix xPico
	Wifi module.  This software was inspired by the ICSC library by
	Majenko Technologies and S.J.Hoeksma



	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef _XPICO_H
#define _XPICO_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define LANTRONIX_SETHTTP			10
#define LANTRONIX_COMMANDRESP		15
#define LANTRONIX_CONNECTNOTIFY		17
#define LANTRONIX_WAITCONNECTION	20
#define LANTRONIX_CMDREADDATA		30
#define LANTRONIX_RESPREADDATA		35
#define LANTRONIX_PROCESSAJAXCMD	37
#define LANTRONIX_SENDRESPONSE1		40
#define LANTRONIX_SENDRESPONSE2		45
#define LANTRONIX_ENDCONNECTION		50

#define MAX_COMMANDS				1
#define MAX_MESSAGE					40

typedef void(*callbackFunction)(unsigned char, char *);

typedef struct
{
	char termchar;
	callbackFunction callback;
} command_t, *command_ptr;


class _XPICO 
{
	private:
		boolean _cmdError;
		command_t _commands[MAX_COMMANDS];
		volatile int _lantronixState;
		volatile int _lantronixNextStep;
		volatile char _ch;
		char _indata[MAX_MESSAGE];
		int _respbyteindex;
		unsigned long _respDataTime;
		HardwareSerial *_serial;
		unsigned long _baud;
		char *_respData;
		boolean _ajaxRespReady;

	protected:
		void reset();

	public:
		_XPICO();
		~_XPICO();
		void begin(unsigned long baud=9600);
		void begin(unsigned long baud, HardwareSerial *sdev);
		void process();
		void registerCommand(char termchar, callbackFunction func);
		void unregisterCommand(char termchar);
		void sendAJAXResponse(char *respData);
};

extern _XPICO XPICO;

#endif
