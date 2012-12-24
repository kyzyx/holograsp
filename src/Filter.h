#pragma once
#include <queue>

class Filter
{
public:
	Filter(void);
	Filter(Filter* f);
	Filter(int w);
	Filter(int w, Filter* f);
	~Filter(void);

	void update(double v) {
		if (next) {
			next->update(v);
			doUpdate(next->get());
		} else {
			doUpdate(v);
		}
	}
	void reset() {
		q.clear();
		if (next) next->reset();
		doReset();
	}

	virtual double get() = 0;
	virtual double predict(int k) = 0;
protected:
	virtual void doUpdate(double v) = 0;
	virtual void doReset() = 0;
	int len;
	std::deque<double> q;
	Filter* next;
};


class ForecastFilter : public Filter {
public:
	ForecastFilter(int n, Filter* f) : Filter(f), time(n) {;}
	~ForecastFilter() {;}
	virtual double get() {
		return next->predict(time);
	}
	virtual double predict(int k) {
		return next->predict(k + time);
	}
protected:
	virtual void doUpdate(double v) {;}
	virtual void doReset() {;}
	ForecastFilter(void);
	int time;
};

// ------------------------------------------
// Averaging Filters
// ------------------------------------------
class AvgFilter : public Filter {
public:
	AvgFilter(void) : Filter() {cursum = 0;}
	AvgFilter(Filter* f) : Filter(f) {cursum = 0;}
	AvgFilter(int w) : Filter(w) {cursum = 0;}
	AvgFilter(int w, Filter* f) : Filter(w, f) {cursum = 0;}
	~AvgFilter() {;}

	virtual double get() {
		if (q.size() == 0) return 0;
		return cursum/q.size();
	}
	virtual double predict(int k) {
		return get();
	}
protected:
	virtual void doUpdate(double v) {
		if (q.size() == len) {
			cursum -= q.back();
			q.pop_back();
		}
		cursum += v;
		q.push_front(v);
	}
	virtual void doReset() {
		cursum = 0;
	}
	double cursum;
};

class DoubleAvgFilter : public AvgFilter {
public:
	DoubleAvgFilter(void) : AvgFilter() {curmasum = 0;}
	DoubleAvgFilter(Filter* f) : AvgFilter(f) {curmasum = 0;}
	DoubleAvgFilter(int w) : AvgFilter(w) {curmasum = 0;}
	DoubleAvgFilter(int w, Filter* f) : AvgFilter(w, f) {curmasum = 0;}

	~DoubleAvgFilter() {;}

	virtual double get() {
		if (q.size() == 0) return 0;
		return (2*cursum - curmasum)/q.size();
	}
	virtual double predict(int k) {
		if (q.size() < 3) get();
		return get() + 2*k*(cursum - curmasum)/(q.size() - 2);
	}
	
protected:
	virtual void doUpdate(double v) {
		if (q.size() == len) {
			cursum -= q.back();
			curmasum -= qma.back();
			q.pop_back();
			qma.pop_back();
		}
		cursum += v;
		q.push_front(v);
		curmasum += cursum/q.size();
		qma.push_front(cursum/q.size());
	}
	virtual void doReset() {
		AvgFilter::doReset();
		curmasum = 0;
		qma.clear();
	}
	double curmasum;
	std::deque<double> qma;
};

class ExpFilter : public Filter {
public:
	ExpFilter(void) : Filter(), a(0.8) {cursum = 0;}
	ExpFilter(double alpha) : Filter(), a(alpha) {cursum = 0;}
	ExpFilter(Filter* f) : Filter(f), a(0.8) {cursum = 0;}
	ExpFilter(double alpha, Filter* f) : Filter(f), a(alpha) {cursum = 0;}
	~ExpFilter() {;}

	virtual double get() {
		return cursum;
	}
	virtual double predict(int k) {
		return get();
	}
protected:
	virtual void doUpdate(double v) {
		cursum = a*v + (1-a)*cursum;
	}
	virtual void doReset() {
		cursum = 0;
	}
	double a;
	double cursum;
};

class DoubleExpFilter : public ExpFilter {
public:
	DoubleExpFilter(void) : ExpFilter(), y(0.7), trend(0) {;}
	DoubleExpFilter(double alpha, double gamma) : ExpFilter(alpha), y(gamma), trend(0) {;}
	DoubleExpFilter(Filter* f) : ExpFilter(f), y(0.7), trend(0) {;}
	DoubleExpFilter(double alpha, double gamma, Filter* f) : ExpFilter(alpha, f), y(0.7), trend(0) {;}
	~DoubleExpFilter() {;}

	virtual double get() {
		return trend;
	}
	virtual double predict(int k) {
		return get() + k*trend;
	}
protected:
	virtual void doUpdate(double v) {
		double oldtrend = trend;
		trend = y*(v - cursum) + (1-y)*trend;
		cursum = a*v + (1-a)*(cursum + oldtrend);
	}
	virtual void doReset() {
		cursum = 0;
		trend = 0;
	}
	double y;
	double trend;
};

class AdaptiveDoubleExpFilter : public DoubleExpFilter {
public:
	AdaptiveDoubleExpFilter(double alow, double ahigh, double ylow, double yhigh, double vlow, double vhigh) :
		a1(alow), a2(ahigh), y1(ylow), y2(yhigh), v1(vlow), v2(vhigh), DoubleExpFilter(ahigh, yhigh) {;}
	AdaptiveDoubleExpFilter(double alow, double ahigh, double ylow, double yhigh, double vlow, double vhigh, Filter* f) :
		a1(alow), a2(ahigh), y1(ylow), y2(yhigh), v1(vlow), v2(vhigh), DoubleExpFilter(ahigh, yhigh, f) {;}

protected:
	virtual void doUpdate(double v) {
		DoubleExpFilter::doUpdate(v);
		if (q.size() == len) {
			q.pop_back();
		} else if (q.size() == 0) {
			q.push_front(v);
		} else {
			double diff = v - q.front();
			q.push_front(v);
			if (diff < v1) {
				a = a1; y = y1;
			} else if (diff > v2) {
				a = a2; y = y2;
			} else {
				a = a2 + (diff - v2)*(a1 - a2)/(v1 - v2);
				y = y2 + (diff - v2)*(y1 - y2)/(v1 - v2);
			}
		}
	}
	double a1, a2, v1, v2, y1, y2;
};
/*
TODO:
Jitter Removal
Adaptive Double Exponential
*/