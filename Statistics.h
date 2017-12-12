#ifndef _STATISTICMANAGER_H
#define _STATISTICMANAGER_H


#include "Main.h"


class CStatistic
{
public:
	CStatistic();
	~CStatistic();

	char cName[255];
	char cText[255];
	int	 iDeaths;
	int	 iID;
	int	 iKills;
	int	 iLatency;
	int	 iTeam;
};

class CStatisticsManager
{
public:
	CStatisticsManager();
	~CStatisticsManager();
	void Manage();
	void SortByColumn(int Column);
	void Swap(CStatistic &a, CStatistic &b);

	int			Count;
	int			iSortColumn;
	CStatistic	Statistic[MAX_CLIENTS];
};

#endif