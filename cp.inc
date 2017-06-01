/*

*************************************
**** MULTI COLERED FUNCTIONAL CP ****
*************************************
PT: Include que trazem inumeros checkpoints ao mesmo tempo, que pode ficar lado a lado
alem de podem ter cores diferentes, e ser anexado, entre outras funcionalidades!
--------------------------------
Actor:	Rodrigo Guigo Dornelles
Nick:	RazorGuigo
AGE: 	18
--------------------------------

Remade: v 1.3

Actor: Dayvison
AGE:		19

-------------------------------

2 contributors @RodrigoDornelles @Dayvison

*
 * MULTI COLERED FUNCTIONAL CP
## Changelog:
> - 
## API:
 * **Definitions:**
 	- 
 * **Callbacks:**
 	- 
 * **Enums:**
 	- CPType
 * **Tags:**
 	- 
 * **Functions:**
 	- CreatePlayerCheckpoint
 	- IsPlayerInAnyCheckpointEx
 	- IsPlayerInCheckpointEx
 * **Variables:**
	- 
 * **Commands:**
 	- 
 * **Macros:**
 	- CPEnter:
 	- CPExit:
## Internal:
 * **Functions:**
 	- GetNexCPID
 * **Callbacks:**
 	- OnPlayerEnterDynamicArea
 	- OnPlayerExitDynamicArea
 * **Enums:**
 	- E_CP_MODEL_DATA
 * **Tags:**
 	- 
 * **Functions:**
 	- 
 * **Variables:**
	- CP_MODEL_DATA
 * **Macros:**
 	- 
## Configuration:
 */

#if !defined MCF_CP_VERSION 					

//-----------STREAMER Required-------------
#if !defined Streamer_IncludeFileVersion
	#tryinclude <streamer>

	#if !defined Streamer_IncludeFileVersion
		#error [MCF CP] This include requires Streamer Plugin! 
	#endif
#endif
//------------------------------------------

//-----------Interator Required-------------
#if !defined foreach
	#tryinclude <YSI\y_iterate>

	#if !defined foreach
		#tryinclude <foreach>

		#if !defined foreach
			#error [MCF CP] This include requires semone interator include! 
		#endif
	#endif	
#endif
//------------------------------------------

public MCF_CP_VERSION = 0x00003;
#pragma unused MCF_CP_VERSION

#if defined mcp_included
 	#endinput
#endif
#define mcp_included

#define CP_STREAMER_IDENTIFIER 2147483646

#define CPEnter:%0(%1) forward cp1_%0(%1);public cp1_%0(%1)
#define CPExit:%0(%1)  forward cp2_%0(%1);public cp2_%0(%1)

#define CP_INVALID -1

enum CPType {
	CP_ARROW_RED,
	CP_ARROW_GREEN,
	CP_ARROW_BLUE,
	CP_ARROW_YELLOW,
	CP_CIRCLE_RED_BIG,
	CP_CIRCLE_GREEN_BIG,
	CP_CIRCLE_BLUE_BIG,
	CP_CIRCLE_RED,
	CP_CIRCLE_ORANGE,
	CP_CIRCLE_YELLOW,
	CP_CIRCLE_GREEN,
	CP_CIRCLE_BLUE,

};

#if __Pawn != 0x030A // != Zeex pawn
	enum E_CP_MODEL_DATA {
#else
	#if defined __PawnBuild // Zeex pawn 3.10.1
		static enum E_CP_MODEL_DATA {
	#else
		enum E_CP_MODEL_DATA {
	#endif
#endif
	CP_Model,
	Float:CP_ModelSize
};

static CP_MODEL_DATA[CPType][E_CP_MODEL_DATA]=
{
	{19605, 1.0}, // CP_ARROW_RED
	{19606, 1.0}, // CP_ARROW_GREEN
	{19607, 1.0}, // CP_ARROW_BLUE
	{19197, 1.0}, // CP_ARROW_YELLOW
	{19945, 8.0}, // CP_CIRCLE_RED_BIG
	{19946, 8.0}, // CP_CIRCLE_GREEN_BIG
	{19947, 8.0}, // CP_CIRCLE_BLUE_BIG
	{1317,	1.0}, // CP_CIRCLE_RED
	{1317,	1.0}, // CP_CIRCLE_ORANGE
	{1317,	1.0}, // CP_CIRCLE_YELLOW
	{1317,	1.0}, // CP_CIRCLE_GREEN
	{1317,	1.0}  // CP_CIRCLE_BLUE
};

#if defined OnPlayerEnterCheckpointEx
	forward OnPlayerEnterCheckpointEx(playerid, checkpoint[]);
#endif

#if defined OnPlayerExitCheckpointEx
	forward OnPlayerExitCheckpointEx(playerid, checkpoint[]);
#endif

/*
##INTERNAL
 * **Functions:**
 	- GetNexCPID
 * **Callbacks:**
 	- OnPlayerEnterDynamicArea
 	- OnPlayerExitDynamicArea
*/

stock GetNexCPID()                     
{
    foreach(new free : Iter_free)    
    {
        return free;
    }
    if(i_CountCP + 1 == MAX_CPS)
    {
        printf("[ERROR] max checkpoints loaded %d/%d", i_CountCP + 1, MAX_CPS);
        return INVALID_CP;
    }
    Iter_count++;                       
    return Iter_count;
}

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		#if defined OnPlayerEnterCheckpointEx
			OnPlayerEnterCheckpointEx(playerid, arr[1]);
		#endif
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			arr[0] = 'c';
			strins(arr[1], "p1_", 0);
			CallLocalFunction(arr, "d", playerid);
		}	
		return true;
	}
	#if defined cp_OnPlayerEnterDynamicArea
		return CP_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif
 
#define OnPlayerEnterDynamicArea CP_OnPlayerEnterDynamicArea
#if defined CP_OnPlayerEnterDynamicArea
	forward CP_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif


public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		#if defined OnPlayerExitCheckpointEx
			OnPlayerExitCheckpointEx(playerid, arr[1]);
		#endif
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			arr[0] = 'c';
			strins(arr[1], "p2_", 0);
			CallLocalFunction(arr, "d", playerid);
		}
		new obj = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_ATTACHED_OBJECT);
		DestroyDynamicObject(obj);
		DestroyDynamicArea(areaid);		
		return true;
	}
	#if defined cp_OnPlayerLeaveDynamicArea
		return CP_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerLeaveDynamicArea
	#undef OnPlayerLeaveDynamicArea
#else
	#define _ALS_OnPlayerLeaveDynamicArea
#endif
 
#define OnPlayerLeaveDynamicArea CP_OnPlayerLeaveDynamicArea
#if defined CP_OnPlayerLeaveDynamicArea
	forward CP_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif

#if defined OnPlayerEnterCheckpointEx
	forward OnPlayerEnterCheckpointEx(playerid, checkpoint[]);
#endif

#if defined OnPlayerExitCheckpointEx
	forward OnPlayerExitCheckpointEx(playerid, checkpoint[]);
#endif

stock CreatePlayerCheckpoint(playerid, name[29] = "", CPType:type, Float:x, Float:y, Float:z, worldid = -1, interiorid = -1)
{
	if(!(CP_ARROW_RED <= type <= CP_CIRCLE_BLUE))
		return 	CP_INVALID;

	if(!(0 <= playerid > MAX_PLAYERS))
		return 	CP_INVALID;

	if(CP_ARROW_RED <= type <= CP_ARROW_YELLOW) z += 2.0;

	new object = CreateDynamicObject(CP_MODEL_DATA[type], x, y, z, 0.0, 0.0, 0.0, worldid, interiorid, playerid);
	new area = CreateDynamicSphere(x, y, z, CP_MODEL_DATA[type][CP_ModelSize], worldid, interiorid, playerid);

	new arr[32];

	arr[0] = CP_STREAMER_IDENTIFIER;
	strcat(arr, name);

	Streamer_SetArrayData(STREAMER_TYPE_AREA, area, E_STREAMER_EXTRA_ID, arr);

	AttachDynamicAreaToObject(area, object);
	return true;
}

stock IsPlayerInAnyCheckpointEx(playerid)
{
	static STREAMER_TAG_AREA areas[500];

	new 
		STREAMER_TAG_AREA num = STREAMER_TAG_AREA GetPlayerDynamicAreas(playerid, areas),
		arr[32]
	;

	for(new STREAMER_TAG_AREA i; i < num; ++i)
	{
		Streamer_GetArrayData(STREAMER_TYPE_AREA, area, E_STREAMER_EXTRA_ID, arr);
		if(arr[0] == CP_STREAMER_IDENTIFIER)
		{
			return true;
		}
	}
	return false;
}

stock IsPlayerInCheckpointEx(playerid, name[])
{
	// isnull
	if((!(name[0])) || (((name[0]) == '\1') && (!(name[1]))))
		return false;

	static STREAMER_TAG_AREA areas[500];

	new 
		STREAMER_TAG_AREA num = STREAMER_TAG_AREA GetPlayerDynamicAreas(playerid, areas),
		arr[32]
	;

	for(new STREAMER_TAG_AREA i; i < num; ++i)
	{
		Streamer_GetArrayData(STREAMER_TYPE_AREA, area, E_STREAMER_EXTRA_ID, arr);
		if(arr[0] == CP_STREAMER_IDENTIFIER)
		{
			// isnull
			if((!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
				continue;

			if(!strcmp(arr[1], name))
				return true;
		}
	}
	return false;
}

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		#if defined OnPlayerEnterCheckpointEx
			OnPlayerEnterCheckpointEx(playerid, arr[1]);
		#endif
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			arr[0] = 'c';
			strins(arr[1], "p1_", 0);
			CallLocalFunction(arr, "d", playerid);
		}
		return true;
	}
	#if defined cp_OnPlayerEnterDynamicArea
		return CP_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif
 
#define OnPlayerEnterDynamicArea CP_OnPlayerEnterDynamicArea
#if defined CP_OnPlayerEnterDynamicArea
	forward CP_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif


public OnPlayerExitDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		#if defined OnPlayerExitCheckpointEx
			OnPlayerExitCheckpointEx(playerid, arr[1]);
		#endif
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			arr[0] = 'c';
			strins(arr[1], "p2_", 0);
			CallLocalFunction(arr, "d", playerid);
		}
		return true;
	}
	#if defined cp_OnPlayerExitDynamicArea
		return CP_OnPlayerExitDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerExitDynamicArea
	#undef OnPlayerExitDynamicArea
#else
	#define _ALS_OnPlayerExitDynamicArea
#endif
 
#define OnPlayerExitDynamicArea CP_OnPlayerExitDynamicArea
#if defined CP_OnPlayerExitDynamicArea
	forward CP_OnPlayerExitDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif

#endif