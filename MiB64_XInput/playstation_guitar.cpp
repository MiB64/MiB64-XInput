/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "playstation_devices.h"
#include "playstation_codes.h"

PS2_Guitar::PS2_Guitar(_Settings &config):
	DualShock2(config) {}

void PS2_Guitar::ReadInputGuitar(const bool config, u8 *buffer)
{
	if(config) ReadInput(buffer);
	else ReadInputPressure(buffer);

	buffer[3] &= 0x7F; // Switch off LEFT DPAD for some reason.
	// 9 - A // A - F // E - 9 // F - E
	u8 bitswap[4] = {0};

	bitswap[0] = (buffer[4] & 0x2)? 0: 1 << 2; // 9 - A
	bitswap[1] = (buffer[4] & 0x4)? 0: 1 << 7; // A - F
	bitswap[2] = (buffer[4] & 0x40)? 0: 1 << 1; // E - 9
	bitswap[3] = (buffer[4] & 0x80)? 0: 1 << 6; // F - E

	buffer[4] |= 0xC6;
	buffer[4] &= ~(bitswap[0] + bitswap[1] + bitswap[2] + bitswap[3]);
}


void PS2_Guitar::Cmd1(u8 data)
{
	switch(data)
	{
	case 0x42: // Polls common input and handles vibration
		if(bConfig) ReadInputGuitar(true, dataBuffer);
		else ReadInputGuitar(false, dataBuffer);
		break;

	case 0x43: // Toggle config mode, poll input and pressures.
		if(bConfig) memset(&dataBuffer[3], 0x00, 6);
		else ReadInputGuitar(false, dataBuffer);
		break;

	case 0x45: if(bConfig) { // Query model, 5th means LED status
		memcpy(&dataBuffer[3], GUITARHERO_MODEL, 6);
		dataBuffer[5] = padID == ID_DIGITAL? 0x00 : 0x01; }
		break;

	default: DualShock2::Cmd1(data);
	}
}

void PS2_Guitar::Cmd4(u8 data)
{
	switch(data)
	{
	case 0x46: if(bConfig) {// Unknown constant part 1 and 2
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], GUITARHERO_ID[0], 5);
		else memcpy(&dataBuffer[4], GUITARHERO_ID[1], 5);}
		break;

	case 0x47: if(bConfig) {//Unknown constant part 3
		memcpy(&dataBuffer[4], GUITARHERO_ID[2], 5); }
		break;

	case 0x4C: if(bConfig) {// Unknown constant part 4 and 5
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], GUITARHERO_ID[3], 5);
		else memcpy(&dataBuffer[4], GUITARHERO_ID[4], 5);}
		break;

	default: DualShock2::Cmd4(data);
	}
}

