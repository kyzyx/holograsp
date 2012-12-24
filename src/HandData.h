#pragma once
class HandData
{
public:
	HandData(void);
	HandData(const HandData& hd);
	~HandData(void);

	
	enum {
		LEFT_HAND = 0,
		RIGHT_HAND = 1,
		LEFT_INDEX,
		RIGHT_INDEX,
		LEFT_THUMB,
		RIGHT_THUMB,
		LEFT_MIDDLE,
		RIGHT_MIDDLE,
		LEFT_RING,
		RIGHT_RING,
		LEFT_PINKY,
		RIGHT_PINKY,
		LEFT_TIP,
		RIGHT_TIP,
		N_POINTS,
	};

	void setNoData(int d) {
		data[d][0] = ERR;
	}
	void setData(int d, float x, float y, float z) {
		data[d][0] = x; data[d][1] = y; data[d][2] = z;
	}
	void setData(int d, float* x) {
		data[d][0] = x[0]; data[d][1] = x[1]; data[d][2] = x[2];
	}
	bool getData(int d, float* x) const {
		if (data[d][0] == ERR) return false;
		if (!x) return true;
		x[0] = data[d][0]; x[1] = data[d][1]; x[2] = data[d][2];
		return true;
	}
	bool isOpen(int hand) const {
		return open[hand] > openthreshold;
	}
	float openness(int hand) const {
		return open[hand];
	}
	void setOpenness(int hand, float o) {
		open[hand] = o;
	}
	void setOpenThreshold(float t) {
		openthreshold = t;
	}
private:
	static const float ERR;

	float openthreshold;
	float open[2];
	float data[N_POINTS][3];
};
