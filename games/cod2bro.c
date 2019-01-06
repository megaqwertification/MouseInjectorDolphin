//==========================================================================
// Mouse Injector for Dolphin
//==========================================================================
// Copyright (C) 2019 Carnivorous
// All rights reserved.
//
// Mouse Injector is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
//==========================================================================
#include <stdint.h>
#include "../main.h"
#include "../memory.h"
#include "../mouse.h"
#include "game.h"

#define CROSSHAIRY 87.890625f // 0x42AFC800
// COD2BRO ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE TO USE)
#define COD2BRO_camx 0x8048B574 - 0x80466E50
#define COD2BRO_camy 0x8048B570 - 0x80466E50
#define COD2BRO_fov 0x8048B54C - 0x80466E50
// STATIC ADDRESSES BELOW
#define COD2BRO_playerbase 0x815566F0 // playable character pointer

static uint8_t COD2BRO_Status(void);
static void COD2BRO_Inject(void);

static const GAMEDRIVER GAMEDRIVER_INTERFACE =
{
	"Call of Duty 2: Big Red One",
	COD2BRO_Status,
	COD2BRO_Inject
};

const GAMEDRIVER *GAME_COD2BRO = &GAMEDRIVER_INTERFACE;

//==========================================================================
// Purpose: return 1 if game is detected
//==========================================================================
static uint8_t COD2BRO_Status(void)
{
	return (MEM_ReadInt(0x80000000) == 0x47514345 && MEM_ReadInt(0x80000004) == 0x35320000); // check game header to see if it matches COD2BRO
}
//==========================================================================
// Purpose: calculate mouse movement and inject into current game
//==========================================================================
static void COD2BRO_Inject(void)
{
	if(xmouse == 0 && ymouse == 0) // if mouse is idle
		return;
	const uint32_t playerbase = (uint32_t)MEM_ReadInt(COD2BRO_playerbase);
	if(!playerbase) // if playerbase is invalid
		return;
	float camx = MEM_ReadFloat(playerbase + COD2BRO_camx);
	float camy = MEM_ReadFloat(playerbase + COD2BRO_camy);
	const float fov = MEM_ReadFloat(playerbase + COD2BRO_fov);
	if(fov > 0 && fov <= 1.f)
	{
		camx -= (float)xmouse / 10.f * ((float)sensitivity / 40.f) * (fov / 1.f); // normal calculation method for X
		camy += (float)(invertpitch ? -ymouse : ymouse) / 10.f * ((float)sensitivity / 40.f) * (fov / 1.f); // normal calculation method for Y
		if(camx < -360)
			camx += 360;
		else if(camx >= 360)
			camx -= 360;
		MEM_WriteFloat(playerbase + COD2BRO_camx, camx);
		MEM_WriteFloat(playerbase + COD2BRO_camy, camy);
	}
}