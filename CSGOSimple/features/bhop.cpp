#include "bhop.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
//#include "../valve_sdk/math/Vector4D.cpp"
//#include "../valve_sdk/math/Vector2D.cpp"

void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
  static bool jumped_last_tick = false;
  static bool should_fake_jump = false;

  if(!jumped_last_tick && should_fake_jump) {
    should_fake_jump = false;
    cmd->buttons |= IN_JUMP;
  } else if(cmd->buttons & IN_JUMP) {
    if(g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
      jumped_last_tick = true;
      should_fake_jump = true;
    } else {
      cmd->buttons &= ~IN_JUMP;
      jumped_last_tick = false;
    }
  } else {
    jumped_last_tick = false;
    should_fake_jump = false;
  }
}

void Edgejump::OnCreateMove(CUserCmd* pCmd)
{
	C_BasePlayer* LocalP = g_LocalPlayer;
	

	if (LocalP->m_nMoveType() == MOVETYPE_LADDER || LocalP->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	Vector Start, End;
	Start = LocalP->m_vecOrigin();

	VectorCopy(LocalP->m_vecOrigin(), Start);
	VectorCopy(Start, End);
	
	End.z -= 32;

	Ray_t ray;
	ray.Init(Start, End);

	trace_t trace;

	CTraceFilter filter;
	
	filter.pSkip = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trace); 

	//int EdgeJumpKey = GetAsyncKeyState(VK_MBUTTON);
	if (trace.fraction == 1.0f && GetAsyncKeyState(VK_MENU))  //&& EdgeJumpKey > 0 && GetKeyState(EdgeJumpKey)
	{
		pCmd->buttons |= IN_JUMP;
	}
}


//C_BaseEntity* LocalP = (C_BaseEntity*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());