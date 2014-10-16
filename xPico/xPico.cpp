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



#include "xPico.h"

_XPICO::_XPICO()
{

}

_XPICO::~_XPICO()
{
	
}

void _XPICO::begin(unsigned long baud)
{
	begin(baud, &Serial1);
}

void _XPICO::begin(unsigned long baud, HardwareSerial *sdev)
{
	_serial = sdev;
	_serial->begin(baud);
	_baud = baud;

	reset();
}

void _XPICO::reset()
{
	_cmdError = false;
	_lantronixState = LANTRONIX_SETHTTP;
	_lantronixNextStep = 0;
	_respbyteindex = 0;
	_ajaxRespReady = false;
}

void _XPICO::process()
{
	switch (_lantronixState)
	{
	case LANTRONIX_SETHTTP:
		{
			_serial->print("1h");
			_serial->write(13);
			_lantronixState = LANTRONIX_COMMANDRESP;
			_lantronixNextStep = LANTRONIX_CONNECTNOTIFY;
			_respbyteindex = 1;
			break;
		}
	case LANTRONIX_CONNECTNOTIFY:
		{
			_serial->print("W1r");
			_serial->write(13);
			_lantronixState = LANTRONIX_COMMANDRESP;
			_lantronixNextStep = LANTRONIX_WAITCONNECTION;
			_respbyteindex = 1;
			break;
		}
	case LANTRONIX_COMMANDRESP:
		{
			if (_serial->available())
			{
				_ch = _serial->read();
				
				if (_ch == 'K')
				{
					_lantronixState = _lantronixNextStep;
				}
				else if (_ch == 'E' && !_cmdError)
				{
					_cmdError = true;
					Serial.print(_ch);
				}
				else if (_cmdError && _ch == '\n')
				{
					Serial.print(_ch);
					_cmdError = false;
					_serial->print("1k");
					_serial->write(13);
					_lantronixState = LANTRONIX_SETHTTP;
				}
				else
				{
					Serial.print(_ch);
				}
			}
			break;
		}
	case LANTRONIX_WAITCONNECTION:
		{
			if (_serial->available())
			{
				_ch = _serial->read();
				if (_respbyteindex == 1)
				{
					if (_ch == '1')
					{
						_respbyteindex = 2;
					}
				}
				else if (_respbyteindex == 2)
				{
					if (_ch == 'r')
					{
						_lantronixState = LANTRONIX_CMDREADDATA;
					}
				}
			}
			break;
		}
	case LANTRONIX_CMDREADDATA:
		{
			_serial->print("1rb~80");
			_serial->write(13);
			_lantronixState = LANTRONIX_COMMANDRESP;
			_lantronixNextStep = LANTRONIX_RESPREADDATA;
			_respbyteindex = 0;
			_indata[_respbyteindex] = '\0';
			break;
		}
	case LANTRONIX_RESPREADDATA:
		{
			if (_serial->available())
			{
				_ch = _serial->read();
				
				if (_ch == _commands[0].termchar)
				{
					_lantronixState = LANTRONIX_SENDRESPONSE1;
					_commands[0].callback(_respbyteindex, _indata);
				}
				else
				{
					_indata[_respbyteindex++] = _ch;
					_indata[_respbyteindex] = '\0';
				}
			}
			break;
		}
	case LANTRONIX_SENDRESPONSE1:
		{
			if (_ajaxRespReady)
			{
				_serial->print("1sb~");
				_serial->write(13);
				_ajaxRespReady = false;
				_lantronixState = LANTRONIX_COMMANDRESP;
				_lantronixNextStep = LANTRONIX_SENDRESPONSE2;
			}
			break;
		}
	case LANTRONIX_SENDRESPONSE2:
		{
			_serial->print(_respData);
			_serial->print("~");
			_serial->write(13);
			_lantronixState = LANTRONIX_COMMANDRESP;
			_lantronixNextStep = LANTRONIX_ENDCONNECTION;
			break;
		}
	case LANTRONIX_ENDCONNECTION:
		{
			_serial->print("1e");
			_serial->write(13);
			_lantronixState = LANTRONIX_SETHTTP;
			_lantronixNextStep = LANTRONIX_SETHTTP;
			break;
		}
	}
}

void _XPICO::registerCommand(char termchar, callbackFunction func)
{
	unsigned char i;

	for (i = 0; i < MAX_COMMANDS; i++)
	{
		if (_commands[i].termchar == 0)
		{
			_commands[i].termchar = termchar;
			_commands[i].callback = func;
			return;
		}
	}
}

void _XPICO::unregisterCommand(char termchar)
{
	unsigned char i;

	for (i = 0; i < MAX_COMMANDS; i++)
	{
		if (_commands[i].termchar == termchar)
		{
			_commands[i].termchar = 0;
			_commands[i].callback = NULL;
			return;
		}
	}
}

void _XPICO::sendAJAXResponse(char *respData)
{
	_ajaxRespReady = true;
	_respData = respData;
}


_XPICO XPICO;
