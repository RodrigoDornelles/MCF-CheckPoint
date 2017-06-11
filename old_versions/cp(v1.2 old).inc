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
*/
#if !defined Streamer_IncludeFileVersion
	#error [MCF CP] This include requires Streamer Plugin! 
#endif

#if !defined MCF_CP_VERSION
public MCF_CP_VERSION = 0x00002;
#pragma unused MCF_CP_VERSION
	

#if !defined MAX_CPS
	#define MAX_CPS (500)
#endif

#if !defined CP_DEBUG
	#define CP_DEBUG 0
#endif

enum MAX_CP_INFO
{
	CP_modelid,
	Float:CP_x,
	Float:CP_y,
	Float:CP_z,
	bool:CP_show,
	CP_forplayer,
	CP_worldid,
	CP_interiorid
}

//---------------------[VARIABLES]----------------------------------
new bool:CP_AreaIsCP[MAX_OBJECTS];									//verifica se area pertence ao checkpoint
new CP_AREACP[MAX_OBJECTS];											//armazena o id do checkpoint em tal area									

new i_CPCOUNT;														//conta quantos checkpoints foram criados
new i_CPOBJECT[MAX_CPS];											//Objeto se comporta como entidade do CP
new i_CPAREA[MAX_CPS];												//Area de interação do CP utilza streamer

new i_CPINFOS[MAX_CPS][MAX_CP_INFO];								//Armazena informações importantes dos checkpoints

new bool:i_PlayerInCP[MAX_PLAYERS][MAX_CPS];
//------------------------------------------------------------------

forward OnPlayerCPchange(playerid, CPID, toggle);
forward OnPlayerEnterCP(playerid, CPID);
forward OnPlayerLeaveCP(playerid, CPID);

forward i_OnPlayerCPchange(playerid, areaid, toggle);

/*
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||| F U N C T I O N S ||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*/


stock CreateCP(modelid=1, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, bool:show= true, worldid = -1, interiorid = -1, forplayer= -1)
{
	if((i_CPCOUNT + 1) > MAX_CPS)	return printf("[ERROR] max checkpoints loaded %d/%d", i_CPCOUNT + 1, MAX_CPS);

	new id=	i_CPCOUNT++;

	new Float:size;
	i_GetCPsize(modelid, size);

	new area = 		i_CPAREA[id]=	CreateDynamicCircle(x, y, size);
	new object = 	i_CPOBJECT[id]= CreateCPObj(i_GetModelCP(modelid), x, y, z, worldid, interiorid,show,forplayer);


	AttachDynamicAreaToObject(area, object);
	CP_AreaIsCP[area]= true;
	CP_AREACP[area]= i_CPCOUNT;

	i_CPINFOS[id][CP_modelid]= 		i_GetModelCP(modelid);
	i_CPINFOS[id][CP_show]=			show;
	i_CPINFOS[id][CP_worldid]=		worldid;
	i_CPINFOS[id][CP_interiorid]=	interiorid;
	i_CPINFOS[id][CP_forplayer]= 	forplayer;

	i_CPINFOS[id][CP_x]= 	x;
	i_CPINFOS[id][CP_y]= 	y;
	i_CPINFOS[id][CP_z]= 	z;

	return i_CPCOUNT;
}

stock bool:IsPlayerInCP(playerid, CPID)
{
	if(i_PlayerInCP[playerid][CPID]) return true;
	return false;
}
stock SetCPpos( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0)					//new v2
{
	i_CPINFOS[id][CP_x]= 	x;
	i_CPINFOS[id][CP_y]= 	y;
	i_CPINFOS[id][CP_z]= 	z;

	return SetDynamicObjectPos(i_CPOBJECT[id], x, y, z);
}
stock MoveCP( CPID, Float:x=0.0, Float:y=0.0, Float:z=0.0, Float:speed= 2.0)	//new v2
{
	i_CPINFOS[id][CP_x]= 	x;
	i_CPINFOS[id][CP_y]= 	y;
	i_CPINFOS[id][CP_z]= 	z;

	return MoveDynamicObject(i_CPOBJECT[id], x, y, z, speed);
}
stock AttachCPtoObject( CPID, objectid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= i_CPOBJECT[CPID];
	return AttachDynamicObjectToObject(obj, objectid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoPlayer( CPID, playerid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= i_CPOBJECT[CPID];
	return AttachDynamicObjectToPlayer(obj, playerid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoVehicle( CPID, vehicleid, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= i_CPOBJECT[CPID];
	return AttachDynamicObjectToVehicle(obj, vehicleid, x, y, z, 0.0, 0.0, 0.0);
}
stock AttachCPtoCP( CPID, CPattached, Float:x=0.0, Float:y=0.0, Float:z=0.0)
{
	new obj= i_CPOBJECT[CPID];
	return AttachDynamicObjectToObject(obj, CPattached, x, y, z, 0.0, 0.0, 0.0);
}

stock SetCPInterior( CPID, interiorid)
{
	new id = CPID;
	i_CPINFOS[id][CP_interiorid]=interiorid;
	DestroyDynamicObject(i_CPOBJECT);

	new obj = i_CPOBJECT[CPID] = CreateCPObj(modelid, i_CPINFOS[id][CP_x],i_CPINFOS[id][CP_y],i_CPINFOS[id][CP_z],
		i_CPINFOS[id][CP_worldid],	i_CPINFOS[id][CP_interiorid],i_CPINFOS[id][CP_show];

	AttachDynamicAreaToObject(i_CPAREA[id], obj);
}
stock SetCPVirtualWord( CPID, worldid)
{
	i_CPINFOS[id][CP_worldid]=	worldid;
	DestroyDynamicObject(i_CPOBJECT);

	new obj = i_CPOBJECT[CPID] = CreateCPObj(modelid, i_CPINFOS[id][CP_x],i_CPINFOS[id][CP_y],i_CPINFOS[id][CP_z],
		i_CPINFOS[id][CP_worldid],	i_CPINFOS[id][CP_interiorid],i_CPINFOS[id][CP_show]);

	AttachDynamicAreaToObject(i_CPAREA[id], obj);
}

/*
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||| I N T E R N A L S ||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*/

//------------------INTERNAL CREATE CP OBJECT------------------------------
stock CreateCPObj(modelid, Float:x, Float:y, Float:z,worldid, interiorid, bool:show, forplayer)
{
	new Float:z_fix = z, worldid_fix = worldid;

	if(modelid == 19197) z_fix += 2.0;
	if(show == false)	worldid_fix = 	2147483640;

	new obj = CreateDynamicObject(modelid, x, y, z_fix, 0.0, 0.0, 0.0, worldid_fix, interiorid, forplayer);
	return obj;
}
//------------------INTERNAL CP SIZES & MODEL-------------------------------
enum CP_MODEL_INFO
{
	CP_Color,
	Float:CP_SizeModel
}
new i_CP_MODELS[][CP_MODEL_INFO]=
{
	{19605, 1.0},	//MIN CP RED
	{19606, 1.0},	//MIN CP GREEN
	{19607, 1.0}, 	//MIN CP BLUE
	{19197, 1.0},	//MIN CP YELLOW
	{19945, 8.0}, 	//BIG CP RED
	{19946, 8.0},	//BIG CP GREEN
	{19947, 8.0}   //BIG CP BLUE
};
stock i_GetModelCP(modelid)
{
	if(sizeof(i_CP_MODELS) < modelid || 0 > modelid) 
	{
		switch(modelid)
		{
			case 19605..19607,19197,19945..19947: return modelid;
			default:	return 19605;
		}
	}
	return i_CP_MODELS[modelid][CP_Color];
}
stock i_GetCPsize(modelid, &Float:CP_size)
{
	if(sizeof(i_CP_MODELS) < modelid || 0 > modelid) CP_size= 1.0;
	CP_size= i_CP_MODELS[modelid][CP_SizeModel];
	return 1;
}
//-------------------------------------------------------------------

//------------------INTERNAL CP CALLBACKS---------------------------
public i_OnPlayerCPchange(playerid, areaid, toggle)
{
	new Float:x,Float:y,Float:z;
	GetPlayerPos(playerid, x, y, z);
	new worldid=GetPlayerVirtualWorld(playerid);
	new interior=GetPlayerInterior(playerid);

	new id = CP_AREACP[areaid];

	
	if(toggle)
	{
		CallRemoteFunction("OnPlayerEnterCP","ii",playerid,id);
		i_PlayerInCP[playerid][id]=true;

		if(i_CPINFOS[id][CP_forplayer] != playerid && i_CPINFOS[id][CP_forplayer] != -1)	
			return 0;

		if(i_CPINFOS[id][CP_interiorid] != interior && i_CPINFOS[id][CP_interiorid] != -1)	
			return 0;

		if(i_CPINFOS[id][CP_worldid] !=	worldid && i_CPINFOS[id][CP_worldid] != -1)	
			return 0;

		switch(i_CPINFOS[id][CP_modelid])
		{
			case 19945..19947: if(i_CPINFOS[id][CP_z] < (z+64))	return 0;
			default: 
			{
				if(i_CPINFOS[id][CP_z] > z) 	return 0;
				if(i_CPINFOS[id][CP_z] < (z+6))	return 0;
			}		
		}
	}
	else
	{																	//new v2
		if(i_PlayerInCP[playerid][id] == false) return 0;				//Correção de BUG-> players passando por baixo do CP chamacallback
		CallRemoteFunction("OnPlayerLeaveCP","ii",playerid,id);
		i_PlayerInCP[playerid][id]=false;
	}
	CallRemoteFunction("OnPlayerCPchange","iii",playerid, id, toggle);
	return 1;
}
//------------------------------------------------------------------

//----------------Internal PLAYER CP CHANGE--------------------------
public CP_OnPlayerEnterDynArea(playerid, areaid)
{
	if(CP_AreaIsCP[areaid])
		i_OnPlayerCPchange(playerid, areaid, true);
	return 1;
}


public CP_OnPlayerLeaveDynArea(playerid, areaid)
{
	if(CP_AreaIsCP[areaid])
		i_OnPlayerCPchange(playerid, areaid, false);
	return 1;
}
//------------------------------------------------------------------




/*
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||||| H O O K S ||||||||||||||||||||||||||||||||||||||
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*/

//--------------[HOOK PLAYER ENTER DYNAMIC AREA]------------------------------
public OnPlayerEnterDynamicArea(playerid, areaid)
{
    #if defined CP_OnPlayerEnterDynArea
        return CP_OnPlayerEnterDynArea(playerid, areaid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerEnterDynamicArea
  #undef OnPlayerEnterDynamicArea
#else
#define _ALS_OnPlayerEnterDynamicArea
#endif

#define OnPlayerEnterDynamicArea CP_OnPlayerEnterDynArea


forward CP_OnPlayerEnterDynArea();
//----------------------------------------------------------------------------


//--------------[HOOK PLAYER LEAVE DYNAMIC AREA]------------------------------
public OnPlayerLeaveDynamicArea(playerid, areaid)
{
    #if defined CP_OnPlayerLeaveDynArea
        return CP_OnPlayerLeaveDynArea(playerid, areaid);
    #else
        return 1;
    #endif
}


#if defined _ALS_OnPlayerLeaveDynamicArea
  #undef OnPlayerLeaveDynamicArea
#else
#define _ALS_OnPlayerLeaveDynamicArea
#endif

#define OnPlayerLeaveDynamicArea CP_OnPlayerLeaveDynArea

forward CP_OnPlayerLeaveDynArea();
//----------------------------------------------------------------------------

#endif
