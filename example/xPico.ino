#include <xPico.h>

char ajaxResp[40];
char buf[12];

void setup()
{
	XPICO.begin(115200, &Serial1);
	XPICO.registerCommand('!', &processXPicoCommand);
}

void loop()
{
	XPICO.process();
}

void processXPicoCommand(unsigned char length, char *data)
{
	Serial.print("Data from Ajax: ");
	for (int i = 0; i < length; i++)
	{
		Serial.print(data[i]);
	}
	Serial.println();

	ajaxResp[0] = '\0';

	strcat(ajaxResp, "Hello World: ");
	strcat(ajaxResp, itoa(random(100),buf, DEC));
	XPICO.sendAJAXResponse(ajaxResp);
}
