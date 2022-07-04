#ifndef PDQ_TIMINGS_H
#define PDQ_TIMINGS_H
#include <time.h>

struct timings {
	clock_t db_insert;
	double db_insert_tm;
	clock_t sig_comp;
	double sig_comp_tm;
};

#endif //PDQ_TIMINGS_H
