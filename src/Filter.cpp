#include "Filter.h"


Filter::Filter(void)
{
	len = 1;
	next = NULL;
}


Filter::Filter(int w)
{
	len = w;
	next = NULL;
}
Filter::Filter(Filter* f)
{
	next = f;
}
Filter::Filter(int w, Filter* f)
{
	len = w;
	next = f;
}


Filter::~Filter(void)
{
}
