#pragma once
//#include "../Core/stdafx.h"
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"

#define NUM_OF_TICKS 12

struct StoredData
{
	float simtime;
	Vector hitboxPos;
};

class BackTrack : public Singleton<BackTrack>
{
	int nLatestTick;
	StoredData TimeWarpData[64][NUM_OF_TICKS];
public:
	//void CreateMove(CUserCmd* cmd);
	void OnCreateMove(CUserCmd* cmd);
};
