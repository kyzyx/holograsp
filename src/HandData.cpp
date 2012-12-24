#include "HandData.h"


const float HandData::ERR = -1e20f;

HandData::HandData(void)
{
	openthreshold = 0.3f;
	for (int i = 0; i < N_POINTS; ++i) data[i][0] = ERR;
	open[0] = -1; open[1] = -1;
}

HandData::HandData(const HandData& h)
{
	openthreshold = h.openthreshold;
	for (int i = 0; i < N_POINTS; ++i) {
		for (int j = 0; j < 3; ++j) data[i][j] = h.data[i][j];
	}
	open[0] = h.open[0];
	open[1] = h.open[1];
}

HandData::~HandData(void)
{
}
