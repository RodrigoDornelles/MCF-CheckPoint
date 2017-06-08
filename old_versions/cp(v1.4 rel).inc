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
 	- CreateCheckPoint
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
 *	- GetNexCPID
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


#if defined mCP_included
 	#endinput
#endif
#define mCP_included

#define CP_STREAMER_IDENTIFIER 2147483646

#define CPEnter:%0(%1) forward cp1_%0(%1);public cp1_%0(%1)
#define CPExit:%0(%1)  forward cp2_%0(%1);public cp2_%0(%1)


#if !defined MAX_CPS
    #define MAX_CPS (500)
#endif

#if !defined MAX_AREAS
    #define MAX_AREAS (1000)
#endif

#define INVALID_CP -1

static Iterator:Iter_free<MAX_CPS>;
static Iterator:Iter_CPS<MAX_CPS>;
static i_CountCP;

static CPObject[MAX_CPS];               //CPID -> to -> dynamicObject
static CPArea[MAX_CPS];					//CPID -> to -> AREAID
static AreaCP[MAX_AREAS];				//AREAID -> to -> CPID

static bool:InCP[MAX_PLAYERS][MAX_CPS];				//Player está no CP

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
	CP_INVISIBLE
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
	{1317,	1.0}, // CP_CIRCLE_BLUE
	{19300,	1.0}  // CP_INVISIBLE
};

#if defined OnPlayerEnterCheckpointEx
	forward OnPlayerEnterCheckpointEx(playerid, checkpoint[], CPID);
#endif

#if defined OnPlayerExitCheckpointEx
	forward OnPlayerExitCheckpointEx(playerid, checkpoint[], CPID);
#endif

stock CreatePlayerCheckPoint(playerid, name[29] = "", CPType:type, Float:x, Float:y, Float:z, worldid = -1, interiorid = -1)
{
	return CreateCheckPoint(name, type, x, y, z, worldid, interiorid, playerid);
}

stock CreateCheckPoint(name[29] = "", CPType:type, Float:x, Float:y, Float:z, worldid = -1, interiorid = -1, forplayer= -1)
{
	if(!(CP_ARROW_RED <= type <= CP_INVISIBLE))
		return INVALID_CP;

	if(forplayer != -1)
	{
		if(!(0 <= forplayer > MAX_PLAYERS))
			return INVALID_CP;
	}

	new id= GetNexCPID();

	if(type == CP_ARROW_YELLOW) z += 1.4;
	if(CP_CIRCLE_RED <= type <= CP_CIRCLE_BLUE) z -= 0.4;


	new object= CPObject[id]= 	CreateDynamicObject(CP_MODEL_DATA[type][CP_Model], x, y, z, 0.0, 0.0, 0.0, worldid, interiorid, forplayer);
	new area=	CPArea[id]= 	CreateDynamicSphere(x, y, z, CP_MODEL_DATA[type][CP_ModelSize], worldid, interiorid, forplayer);

	AreaCP[area]= id;

	switch(type)
	{
		case CP_CIRCLE_RED:		SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "red-4", 0x99FF0000);
		case CP_CIRCLE_ORANGE:	SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "orange", 0x99FF8000);
		case CP_CIRCLE_YELLOW:	SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "yellow", 0x99FFFF00);
		case CP_CIRCLE_GREEN:	SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "green", 0x99008000);
		case CP_CIRCLE_BLUE:	SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "blue", 0x990000FF);
	}

	new arr[32];

	Iter_Add(Iter_CPS, id);

	arr[0] = CP_STREAMER_IDENTIFIER;
	strcat(arr, name);

	Streamer_SetArrayData(STREAMER_TYPE_AREA, area, E_STREAMER_EXTRA_ID, arr);

	AttachDynamicAreaToObject(area, object);
	return id;
}
stock DestroyCheckPoint( CPID)
{
	DestroyDynamicArea(CPArea[CPID]);
	DestroyDynamicObject(CPObject[CPID]);
	Iter_Add(Iter_free, CPID);
	Iter_Remove(Iter_CPS, CPID);
}
stock SetCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return SetDynamicObjectPos(obj, x, y, z);
}
stock MoveCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0, Float:speed= 2.0)
{
	new obj= CPObject[CPID];
	return MoveDynamicObject(obj, x, y, z, speed);
}
stock AttachCPtoObject( CPID, objectid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToObject(obj, objectid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoPlayer( CPID, playerid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToPlayer(obj, playerid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoVehicle( CPID, vehicleid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToVehicle(obj, vehicleid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoCP( CPID, CPattached, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToObject(obj, CPObject[CPattached], x, y, z, 0.0, 0.0, 0.0);
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
		Streamer_GetArrayData(STREAMER_TYPE_AREA, i, E_STREAMER_EXTRA_ID, arr);
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
		Streamer_GetArrayData(STREAMER_TYPE_AREA, i, E_STREAMER_EXTRA_ID, arr);
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
/*--------------------------------------------------------------------------------------------------------------
## Internal:
 * **Functions:**
 *	- GetNexCPID
 * **Callbacks:**
 	- OnPlayerEnterDynamicArea
 	- OnPlayerExitDynamicArea
*/
stock GetNexCPID()                        //capta o ID, do proximo CP a ser criado!
{
    foreach(new free : Iter_free)        //recupera o ID de um CP deletado
    {
    	Iter_Remove(Iter_free, free);
        return free;
    }
    if(i_CountCP + 1 == MAX_CPS)
    {
        printf("[ERROR] max checkpoints loaded %d/%d", i_CountCP + 1, MAX_CPS);
        return INVALID_CP;
    }
    i_CountCP++;                         //Caso não haja CP deletados, ele cria um novo ID
    return  i_CountCP;
}
//-------------------------------------------------------------------------------------------------------------------------
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	foreach(new CPID : Iter_CPS)
	{
		if(InCP[playerid][CPID])
		{
			new arr[32];
			Streamer_GetArrayData(STREAMER_TYPE_AREA, AreaCP[CPID], E_STREAMER_EXTRA_ID, arr);
			#if defined OnPlayerKeyStateChangeInCP
				OnPlayerKeyStateChangeInCP(playerid, arr, CPID, newkeys, oldkeys);
			#endif
		} 
	}
	#if defined CP_OnPlayerKeyStateChange
		return CP_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerKeyStateChange
	#undef OnPlayerKeyStateChange
#else
	#define _ALS_OnPlayerKeyStateChange
#endif
 
#define OnPlayerEnterDynamicArea CP_OnPlayerEnterDynamicArea
#if defined CP_OnPlayerKeyStateChange
	forward CP_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif
#if defined OnPlayerKeyStateChangeInCP
	forward OnPlayerKeyStateChangeInCP(playerid, name[], CPID, newkeys, oldkeys);
#endif

//-------------------------------------------------------------------------------------------------------------------------

public OnPlayerText(playerid, text[])
{
	foreach(new CPID : Iter_CPS)
	{
		if(InCP[playerid][CPID])
		{
			new arr[32];
			Streamer_GetArrayData(STREAMER_TYPE_AREA, AreaCP[CPID], E_STREAMER_EXTRA_ID, arr);

			#if defined OnPlayerTextInCP
				OnPlayerTextInCP(playerid, arr, CPID, text);
			#endif
		} 
	}
	#if defined CP_OnPlayerText
		return CP_OnPlayerText(playerid, text);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerText
	#undef OnPlayerText
#else
	#define _ALS_OnPlayerText
#endif
 
#define OnPlayerText CP_OnPlayerText
#if defined CP_OnPlayerText
	forward CP_OnPlayerText(playerid, text[]));
#endif
#if defined OnPlayerTextInCP
	forward OnPlayerTextInCP(playerid, name[], CPID, text[]);
#endif

//-------------------------------------------------------------------------------------------------------------------------


public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		new CPID= AreaCP[areaid];
		#pragma unused CPID

		InCP[playerid][CPID]= true;

		#if defined OnPlayerEnterCheckpointEx
			OnPlayerEnterCheckpointEx(playerid, arr[1], CPID);
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
	#if defined CP_OnPlayerEnterDynamicArea
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

//-------------------------------------------------------------------------------------------------------------------------
public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		new CPID= AreaCP[areaid];
		#pragma unused CPID

		InCP[playerid][CPID]= false;

		#if defined OnPlayerExitCheckpointEx
			OnPlayerExitCheckpointEx(playerid, arr[1], CPID);
		#endif
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			arr[0] = 'c';
			strins(arr[1], "p2_", 0);
			CallLocalFunction(arr, "d", playerid);
		}
		new obj = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_ATTACHED_OBJECT);
		return true;
	}
	#if defined CP_OnPlayerLeaveDynamicArea
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
//-------------------------------------------------------------------------------------------------------------------------
#endif //Final da include