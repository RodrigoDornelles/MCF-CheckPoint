/*

*************************************
**** MULTI COLERED FUNCTIONAL CP ****
*************************************
PT: Include que trazem inumeros checkpoints ao mesmo tempo, que pode ficar lado a lado
alem de podem ter cores diferentes, e ser anexado, entre outras funcionalidades!
--------------------------------



2 contributors @RodrigoDornelles @Dayvison

-------------------------------
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
## 
 */
//================================================================================================
//Configuration:
#if defined MCF_CP_VERSION 					
 	#endinput
#endif

//------------------------------------------
#define CPEnter:%0(%1) 		forward cp1_%0(%1);public cp1_%0(%1)	//on player enter CP
#define CPExit:%0(%1)  		forward cp2_%0(%1);public cp2_%0(%1)	//on player exit CP
#define CPKeys:%0(%1) 		forward cp3_%0(%1);public cp3_%0(%1)	//on Player pres key in CP
#define CPText:%0(%1) 		forward cp4_%0(%1);public cp4_%0(%1)	//On Player CMD in CP
#define MCF_INTERNAL%0(%1)	forward%0(%1);public%0(%1)				//for internal functions	
#define MCF_Print(%0)		if(MCF_GlobalInfo[CP_Debug])printf(%0)	//Debug Printf
#define MCF_ERROR(%0?%1)	if(%0)printf(%1)						//Error PrintF	
#define MCF_CallExist(%0) 	(funcidx(%0)!=-1)						//Verify Callback
#define MCF_Callback												//List Callbacks
#define MCF_Function												//List Functions
//------------------------------------------
#define CP_STREAMER_IDENTIFIER 	0xA7 								// §
#define INVALID_CP 				0x00 								// CP NULL
#define CP_Z_FIX				0.40								//FIZ FOOT POS
#define MAX_CP_NAME				0029								// Name Limit
#define CPType:					Float:								// 
#define CP_MODE_CYLINDER		false 								//CP_MODE [GAMBI]
#define CP_MODE_SPHERE			true 								//CP_MODE [COFIABLE]

#define CP_ARROW 				Float:0xFFFFF1						//TYPE 
#define	CP_CIRCLE_BIG			Float:0xFFFFF2						//TYPE
#define	CP_BIG_CIRCLE			Float:0xFFFFF2						//TYPE
#define	CP_CIRCLE 				Float:0xFFFFF3						//TYPE
#define	CP_INVISIBLE: 			Float:								//TYPE:size
#define CP_INFINITE_CIRCLE 		Float:0xFFFFF5						//Type

#define INTERNAL_ARROW			0000								//CP_ARROW 
#define INTERNAL_CIRCLE_BIG		0001								//CP_CIRCLE_BIG
#define INTERNAL_CIRCLE 		0002								//CP_CIRCLE
#define INTERNAL_INVISIBLE		0003								//CP_INVISIBLE	
                     
#if !defined DEFAULT_CP_MODE              							//
    #define DEFAULT_CP_MODE		CP_MODE_CYLINDER					//CP_MODE
#endif																//

#if !defined MCFCP_DEBUG                  							//
    #define MCFCP_DEBUG	 		false								//debug mode default
#endif																//

#if !defined MAX_MCFCPS												//
    #define MAX_MCFCPS 			500									//Limit checkpoint
#endif																//

#if !defined MAX_MCFAREAS											//
    #define MAX_MCFAREAS 		1000								//Internal Arrays Area
#endif																//
//------------------------------------------
#if !defined Streamer_IncludeFileVersion							//
	#error [MCF CP] This include requires Streamer Plugin! 			//Need Streamer Check
#endif																//
//================================================================================================
//ENUMURATORS:
enum ENUM_MCF_GLOBAL
{
	bool:CPG_Debug,
	CPG_Count,
	CPG_Loads,
	bool:CPG_AreaMode
}
enum ENUM_MCF_CP
{
	bool:CP_use,
	bool:CP_forplayer,
	Float:CP_Model,
	Float:CP_Size,
	CP_Object,
	CP_AreaId,
	CP_Color,
	CP_Interior,
	CP_World,
	CP_Name[MAX_CP_NAME]
}
enum ENUM_MCF_AREA
{
	CPA_CPId,
	CPA_Count,
	Float:CPA_minz,
	Float:CPA_maxz
}
enum ENUM_MCF_PLAYER
{
	bool:CPP_InAnyCP,
	CPP_LastCP
}
enum ENUM_MCF_SIZES
{
	CPZ_Model,
	Float:CPZ_Cylinder,
	Float:CPZ_Sphere,
}
//================================================================================================
//Variables
static MCF_MODELSIZES[4][ENUM_MCF_SIZES]=
{
	{19605, 	1.0,	1.3}, // CP_ARROW
	{19945, 	9.0,	9.0}, // CP_CIRCLE_BIG
	{1317,		1.0,	1.3}, // CP_CIRCLE
	{19300,		0.0,	0.0}  // CP_INVISIBLE
};

static MCF_GlobalInfo[ENUM_MCF_GLOBAL]={MCFCP_DEBUG,DEFAULT_CP_MODE,0};	//Global 		Var
static MCF_CPInfo[MAX_MCFCPS][ENUM_MCF_CP];								//Per CP 		Var
static MCF_AreaInfo[MAX_MCFAREAS][ENUM_MCF_AREA];						//Per Area 	 	Var
static MCF_PlayerInfo[MAX_PLAYERS][ENUM_MCF_PLAYER];					//Per Player 	Var
static bool:MCF_PlayerInCP[MAX_PLAYERS][MAX_MCFCPS];					//Per Player 	Var

//================================================================================================
//Features:
forward MCF_Callback OnPlayerEnterCheckpointEx(playerid, checkpoint[], CPID);
forward MCF_Callback OnPlayerExitCheckpointEx(playerid, checkpoint[], CPID);
forward MCF_Callback OnCheckpointCreate(CPID, name[29], type, color, Float:x, Float:y, Float:z, worldid, interiorid, forplayer);
//Create
forward MCF_Function DestroyCheckPoint( CPID);
forward MCF_Function CreatePlayerCheckPoint(playerid, name[29] = "", CPType:type= CP_CIRCLE, color= 0x99FF0000, Float:x=0.0, Float:y=0.0, Float:z=0.0, worldid = -1, interiorid = -1);
forward MCF_Function CreateCheckPoint(name[29] = "", Float:type = CP_CIRCLE, color= 0x99FF0000, Float:x=0.0, Float:y=0.0, Float:z=0.0, worldid = -1, interiorid = -1, forplayer= -1);
//Name
forward MCF_Function GetCPName( CPID);
forward MCF_Function bool:SetNameCP(CPID, name[]);
forward MCF_Function RenameCP(oldname[], newname[]);
//Pos:
forward MCF_Function SetCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0);
forward MCF_Function MoveCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0, Float:speed= 2.0);
forward MCF_Function AttachCPtoObject( CPID, objectid, Float:x=0.0, Float:y=0.0, Float:z=0.0);
forward MCF_Function AttachCPtoPlayer( CPID, playerid, Float:x=0.0, Float:y=0.0, Float:z=0.0);
forward MCF_Function AttachCPtoVehicle( CPID, vehicleid, Float:x=0.0, Float:y=0.0, Float:z=0.0);
forward MCF_Function AttachCPtoCP( CPID, CPattached, Float:x=0.0, Float:y=0.0, Float:z=0.0);
//================================================================================================
//Internal:
MCF_INTERNAL bool:GetCPAreaMode()
	return MCF_GlobalInfo[CPG_AreaMode];

MCF_INTERNAL CPCount()
	return MCF_GlobalInfo[CPG_Count];

MCF_INTERNAL bool:IsValidCPID( CPID)
	return 0 < CPID < MAX_MCFCPS ? true:false;

MCF_INTERNAL GetAreaidCP( CPID)
	return IsValidCPID(CPID)? MCF_CPInfo[CPID][CP_AreaId]:INVALID_CP;

MCF_INTERNAL bool:IsCPEnabled( CPID)
	return IsValidCPID(CPID)? MCF_CPInfo[CPID][CP_use]:false;

MCF_INTERNAL GetNexCPID()
{
	MCF_GlobalInfo[CPG_Count]++;
   	for(new CPID=1;CPID <= MCF_GlobalInfo[CPG_Count]; CPID++)
   	{
   		if(MCF_CPInfo[CPID][CP_use])
   			continue;

   		if(MCF_GlobalInfo[CPG_Count] != CPID)
   			MCF_GlobalInfo[CPG_Count]--;

   		MCF_CPInfo[CPID][CP_use]=true;
   		return CPID;
   	}
    return INVALID_CP;
}
MCF_INTERNAL InCylinder(playerid, areaid)
{
	if(GetCPAreaMode())
		return 1;//sphere

	new Float:z,Float:tmp;
	GetPlayerPos(playerid, tmp, tmp, z);

	z-= 0.5; //foot

	if(MCF_AreaInfo[areaid][CPA_minz] <= z < MCF_AreaInfo[areaid][CPA_maxz])
		return 1;

	return 0;
}
MCF_INTERNAL GetInternalCPType( Float:type)
{
	if(type == CP_ARROW)
		return 0;
	if(type == CP_CIRCLE_BIG)
		return 1;
	if(type == CP_CIRCLE)
		return 2;
	return 3;
}
MCF_INTERNAL GetMCFModelType(t)
{
	if(0 < t < 4)
		return MCF_MODELSIZES[t][CPZ_Model];
	return MCF_MODELSIZES[0][CPZ_Model];
}
MCF_INTERNAL Float:GetMCFSizeType(t)
{
	if(0 < t < 4)
		return MCF_MODELSIZES[t][CPZ_Cylinder];
	return MCF_MODELSIZES[0][CPZ_Cylinder];
}
MCF_INTERNAL SetCylinderFix(areaid, t, Float:z)
{
	new Float:zmax;

	if(t == INTERNAL_CIRCLE_BIG)
		zmax= (z+63.0);
	else
		zmax= (z+1.5);

	MCF_AreaInfo[areaid][CPA_minz]= z - 2.0;
	MCF_AreaInfo[areaid][CPA_maxz]= zmax;
}

MCF_INTERNAL DetectCylinderPerZ(playerid, STREAMER_TAG_AREA areaid)
{
	if(!IsPlayerInDynamicArea(playerid, areaid))
		return 1;//exit to area 

	if(!InCylinder(playerid, areaid))//redetect
		return SetTimerEx("DetectCylinderPerZ", 1000, false, "dd", playerid, areaid);

	return 1;
}
//================================================================================================
//Functions:

stock DestroyCheckPoint( CPID)
{
	DestroyDynamicObject(MCF_CPInfo[CPID][CP_Object]);
	DestroyDynamicArea(MCF_CPInfo[CPID][CP_AreaId]);
	MCF_CPInfo[CPID][CP_use]=false;
	return INVALID_CP;
}
stock DestroyCheckPointEx( name[])
{
	// isnull
	if((!(name[0])) || (((name[0]) == '\1') && (!(name[1]))))
		return 0;

	new count=0;
	for(new CPID=1; CPID <= CPCount(); CPID++)
	{
		if(!IsCPEnabled(CPID))
			continue;

		if(strcmp(name, GetCPName( CPID))
			continue;
		
		DestroyCheckPoint( CPID);
		count++;
	}
	return count;
}
stock CreatePlayerCheckPoint(playerid, name[29] = "", Float:type= CP_CIRCLE, color= 0x99FF0000, Float:x=0.0, Float:y=0.0, Float:z=0.0, worldid = -1, interiorid = -1)
	return CreateCheckPoint(name, type, color, x, y, z, worldid, interiorid, playerid);


stock CreateCheckPoint(name[29] = "", Float:type = CP_CIRCLE, color= 0x99FF0000, Float:x=0.0, Float:y=0.0, Float:z=0.0, worldid = -1, interiorid = -1, forplayer= -1)
{
	if(forplayer != -1)						//Per player
		if(!IsPlayerConnected(forplayer))	//Verify Player
			return INVALID_CP;				//Player invalid

	new CPID = GetNexCPID(),				//CheckPointID
		IT=		GetInternalCPType(type);	//Internal Type

	if(CPID == INVALID_CP)					//Player not conected
		return INVALID_CP;					//Player not conected

	if(funcidx("OnCheckpointCreate")!=-1)
		if(!CallLocalFunction("OnCheckpointCreate","dsddfffddd",CPID,name, IT, color, x, y,z, worldid, interiorid, forplayer))
			return DestroyCheckPoint(CPID); //returning 0 destroy the checkpoint

	printf("CP Create-> %d", CPID);

	new obj, 	//objectid
		mdl, 	//Modelid
		area,	//areaid
		arr[32],//Streamer extra ID
		Float:size;

	z-= 	INTERNAL_CIRCLE != IT ? CP_Z_FIX : (CP_Z_FIX * CP_Z_FIX) + 0.4;
	mdl=	GetMCFModelType(IT);
	size=	GetMCFSizeType(IT);

	if(size == 0.0)							//CP_INVISIBLE
		size=type;							//Size per CP

	obj=CreateDynamicObject(mdl, x, y, z - CP_Z_FIX, 0.0, 0.0, 0.0, worldid, interiorid, forplayer);
	if(GetCPAreaMode() == CP_MODE_SPHERE)
		CreateDynamicSphere(x, y, z,size, worldid, interiorid, forplayer);
	else 
	{
		area=CreateDynamicCircle(x,y,size, worldid, interiorid, forplayer);
		SetCylinderFix(area,IT,z);
	}

	if(IT == INTERNAL_CIRCLE)
		SetDynamicObjectMaterial(obj, 0, 18646, "matcolours", "white", color);
	else
		SetDynamicObjectMaterialText(obj, 0, " ", OBJECT_MATERIAL_SIZE_32x32,"Arial", 8, 1, 0x55555555, color);
	
	format(arr, 32, "%c%s", CP_STREAMER_IDENTIFIER, name);
	Streamer_SetArrayData(STREAMER_TYPE_AREA, area, E_STREAMER_EXTRA_ID, arr);

	AttachDynamicAreaToObject(area, obj);

	format(MCF_CPInfo[CPID][CP_Name], MAX_CP_NAME, "%s", name); 
	MCF_CPInfo[CPID][CP_forplayer]	=forplayer!=-1? true:false;
	MCF_CPInfo[CPID][CP_Model]		=mdl;
	MCF_CPInfo[CPID][CP_Size]		=size;
	MCF_CPInfo[CPID][CP_Object]		=obj;
	MCF_CPInfo[CPID][CP_AreaId]		=area;
	MCF_CPInfo[CPID][CP_Color]		=color;
	MCF_CPInfo[CPID][CP_Interior]	=interiorid;
	MCF_CPInfo[CPID][CP_World]		=worldid;
	
	MCF_AreaInfo[area][CPA_CPId]=CPID;
	MCF_AreaInfo[area][CPA_Count]++;
	return CPID;
}
//Names: ----------------------------------------------------------------------------------------------------
stock GetCPName( CPID)
{
	new arr[MAX_CP_NAME]="";
	if(IsValidCPID(CPID))
		strcat(arr, MCF_CPInfo[CPID][CP_Name]);
	return arr;
}
stock bool:SetNameCP(CPID, name[])
{
	if(!IsValidCPID(CPID))
		return false;
	new arr[32];

	format(arr, 32, "%c%s", CP_STREAMER_IDENTIFIER, name);
	Streamer_SetArrayData(STREAMER_TYPE_AREA, GetAreaidCP(CPID), E_STREAMER_EXTRA_ID, arr);
	return true;
}
stock RenameCP(oldname[], newname[])
{
	new arr[32],name[32],cpcounts;
	format(name, 32, "%c%s", CP_STREAMER_IDENTIFIER, name)

	for(new i=1; i <= CPCount(), i++)
	{
		if(!IsCPEnabled(CPID))
			continue;

		Streamer_GetArrayData(STREAMER_TYPE_AREA, i, E_STREAMER_EXTRA_ID, arr);

		// isnull
		if((!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
			continue;
		strdel(arr, 0, 1);
		if(strcmp(arr, oldname)
			continue;

		cpcounts++;
		Streamer_SetArrayData(STREAMER_TYPE_AREA, i, E_STREAMER_EXTRA_ID, name);
	}
	return cpcounts;
}
//Pos: ----------------------------------------------------------------------------------------------------
/*stock SetCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return SetDynamicObjectPos(obj, x, y, z - CP_Z_FIX);
}
stock MoveCPtoPos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0, Float:speed= 2.0)
{
	new obj= CPObject[CPID];
	return MoveDynamicObject(obj, x, y, z, speed);
}
stock AttachCPtoObject( CPID, objectid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToObject(obj, objectid, x, y, z - CP_Z_FIX, 0.0, 0.0, 0.0);
}
stock AttachCPtoPlayer( CPID, playerid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToPlayer(obj, playerid, x, y, z - CP_Z_FIX, 0.0, 0.0, 0.0);
}
stock AttachCPtoVehicle( CPID, vehicleid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToVehicle(obj, vehicleid, x, y, z - CP_Z_FIX, 0.0, 0.0, 0.0);
}
stock AttachCPtoCP( CPID, CPattached, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= CPObject[CPID];
	return AttachDynamicObjectToObject(obj, CPObject[CPattached], x, y, z - CP_Z_FIX, 0.0, 0.0, 0.0);
}
stock SetCheckPointColor(CPID, color=0x99FF0000)
{
	if(CP_CIRCLE == CPmodel[CPID])
		SetDynamicObjectMaterial(object, 0, 18646, "matcolours", "white", color);
	else
		SetDynamicObjectMaterialText(object, 0, " ", OBJECT_MATERIAL_SIZE_32x32,"Arial", 8, 1, 0x55555555, color);
}
*/
stock bool:IsPlayerInCP(playerid, CPID)
	return IsPlayerConnected(playerid) && IsValidCPID(CPID) ? MCF_PlayerInCP[playerid][CPID]:false;

stock IsPlayerInAnyCheckpointEx(playerid)
	return IsPlayerConnected(playerid) ? MCF_PlayerInfo[playerid][CPP_InAnyCP]:false;


stock bool:IsPlayerInCheckpointEx(playerid, name[])
{
	// isnull
	if((!(name[0])) || (((name[0]) == '\1') && (!(name[1]))))
		return false;

	for(new CPID=1; CPID <= CPCount(); CPID++)
	{
		if(!IsCPEnabled(CPID))
				continue;

		if(!MCF_PlayerInCP[playerid][CPID])
				continue;	

		if(!strcmp(name, GetCPName( CPID))
			return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(IsPlayerInAnyCheckpointEx(playerid))
	{
		for(new CPID=1; CPID <= CPCount(); CPID++)
		{
			if(!IsCPEnabled(CPID))
				continue;

			if(!MCF_PlayerInCP[playerid][CPID])
				continue;	

			new arr[32];
			format(arr, 32, "cp3_%s",  GetCPName( CPID));
			if(funcidx(arr)!=-1)
				CallLocalFunction(arr, "dddd", playerid, CPID, newkeys, oldkeys);

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
 
#define OnPlayerKeyStateChange CP_OnPlayerKeyStateChange
forward CP_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);


//-------------------------------------------------------------------------------------------------------------------------
public OnPlayerText(playerid, text[])
{
	if(IsPlayerInAnyCheckpointEx(playerid))
	{
		for(new CPID=1; CPID <= CPCount(); CPID++)
		{
			if(!IsCPEnabled(CPID))
				continue;

			if(!MCF_PlayerInCP[playerid][CPID])
				continue;	

			new arr[32];
			format(arr, 32, "cp4_%s",  GetCPName( CPID));
			if(funcidx(arr)!=-1)
				CallLocalFunction(arr, "dds", playerid, CPID, text);

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
forward CP_OnPlayerText(playerid, text[]);


//-------------------------------------------------------------------------------------------------------------------------


public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		if(!InCylinder(playerid, areaid))
			return SetTimerEx("DetectCylinderPerZ", 1000, false, "dd", playerid, areaid);

		strdel(arr, 0, 1);

		new CPID=	MCF_AreaInfo[areaid][CPA_CPId];
		
		MCF_PlayerInCP[playerid][CPID]=true;
		MCF_PlayerInfo[playerid][CPP_InAnyCP]=true;		

		//if(funcidx("OnPlayerEnterCheckpointEx")!=-1)
		//	if(!CallLocalFunction("OnPlayerEnterCheckpointEx","dsd",playerid, arr, CPID))
		//		return 1;//não chamar processador!
		
		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			strins(arr, "cp1_", 0);
				if(funcidx(arr)!=-1)
					CallLocalFunction(arr, "dd", playerid, CPID);
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
forward CP_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);

//-------------------------------------------------------------------------------------------------------------------------
public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new arr[32];
	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);
	if(arr[0] == CP_STREAMER_IDENTIFIER)
	{
		strdel(arr, 0, 1);

		new CPID= MCF_AreaInfo[areaid][CPA_CPId];
		
		MCF_PlayerInCP[playerid][CPID]=false;
		MCF_PlayerInfo[playerid][CPP_InAnyCP]=false;

		for(new i=1; i <= CPCount(); i++)
			if(MCF_PlayerInCP[playerid][CPID])	
		{
			MCF_PlayerInfo[playerid][CPP_InAnyCP]=true;	
			break;
		}

		//if(funcidx("OnPlayerExitCheckpointEx")!=-1)
		//	if(!CallLocalFunction("OnPlayerExitCheckpointEx","dsd",playerid, arr, CPID))
		//		return 1;//não chamar processador!

		// !isnull
		if(!(!(arr[1])) || (((arr[1]) == '\1') && (!(arr[2]))))
		{
			strins(arr, "cp2_", 0);
				if(funcidx(arr)!=-1)
					CallLocalFunction(arr, "dd", playerid, CPID);
		}
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
forward CP_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid);

//-------------------------------------------------------------------------------------------------------------------------
