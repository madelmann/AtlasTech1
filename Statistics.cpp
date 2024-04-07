
// Library includes

// Project includes
#include "Main.h"
#include "Statistics.h"


CStatistic::CStatistic()
{
	sprintf(cName, "");
	sprintf(cText, "");
	iDeaths = 0;
	iID = -1;
	iKills = 0;
	iLatency = 0;
	iTeam = -1;
}

CStatistic::~CStatistic()
{
	sprintf(cName, "");
	sprintf(cText, "");
	iDeaths = 0;
	iID = -1;
	iKills = 0;
	iLatency = 0;
	iTeam = -1;
}



CStatisticsManager::CStatisticsManager()
{
	Count = 0;
	iSortColumn = 3;
}

CStatisticsManager::~CStatisticsManager()
{
	Count = 0;
	iSortColumn = 3;
}

void CStatisticsManager::Manage()
{
	if(g_Manager_Player->Count <= 0)
		return;

	Count = g_Manager_Player->Count;

	SortByColumn(iSortColumn);
}

void CStatisticsManager::SortByColumn(int Column)
{
	iSortColumn = Column;

	for(int i = 0; i < Count; i += 1)
	{
		sprintf(Statistic[i].cName, "%s", g_Manager_Player->Player[i].getName());
		Statistic[i].iDeaths = g_Manager_Player->Player[i].iDeaths;
		Statistic[i].iID = g_Manager_Player->Player[i].getId();
		Statistic[i].iKills = g_Manager_Player->Player[i].iKills;
		Statistic[i].iLatency = i;
	}

	if(Count == 1)
	{
		sprintf(Statistic[0].cText, "| %s        %i        %i        %i        %i        %i |", Statistic[0].cName, Statistic[0].iKills, Statistic[0].iDeaths, Statistic[0].iTeam, Statistic[0].iID, Statistic[0].iLatency);
		return;
	}

	switch(iSortColumn)
	{
	case 0:
		// Name
		break;
	case 1:
		// Deaths
		for(int i = 0; i < Count; i += 1)
		{
			for(int j = 1; j < Count - i; j += 1)
				if(Statistic[j].iDeaths > Statistic[j - 1].iDeaths)
					Swap(Statistic[j], Statistic[j - 1]);
		}
		break;
	case 2:
		// ID
		for(int i = 0; i < Count; i += 1)
		{
			for(int j = 1; j < Count - i; j += 1)
				if(Statistic[j].iLatency > Statistic[j - 1].iLatency)
					Swap(Statistic[j], Statistic[j - 1]);
		}
		break;
	case 3:
		// Kills
		for(int i = 0; i < Count; i += 1)
		{
			for(int j = 1; j < Count - i; j += 1)
				if(Statistic[j].iKills > Statistic[j - 1].iKills)
					Swap(Statistic[j], Statistic[j - 1]);
		}
		break;
	case 4:
		// Latency
		for(int i = 0; i < Count; i += 1)
		{
			for(int j = 1; j < Count - i; j += 1)
				if(Statistic[j].iLatency > Statistic[j - 1].iLatency)
					Swap(Statistic[j], Statistic[j - 1]);
		}
		break;
	case 5:
		// Team & Kills
		for(int i = 0; i < Count; i += 1)
		{
			for(int j = 1; j < Count - i; j += 1)
				if(Statistic[j].iTeam > Statistic[j - 1].iTeam)
					Swap(Statistic[j], Statistic[j - 1]);
				else
					if(Statistic[j].iKills > Statistic[j - 1].iKills)
						Swap(Statistic[j], Statistic[j - 1]);
		}
		break;
	}

	for(int i = 0; i < Count; i += 1)
	{
		sprintf(Statistic[i].cText, "| %s        %i        %i        %i        %i        %i |", Statistic[i].cName, Statistic[i].iKills, Statistic[i].iDeaths, Statistic[i].iTeam, Statistic[i].iID, Statistic[i].iLatency);
	}
}

void CStatisticsManager::Swap(CStatistic &a, CStatistic &b)
{
	CStatistic z = a;
	a = b;
	b = z;
}