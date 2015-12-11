#ifndef SKI_RACE_H_
#define SKI_RACE_H_

#include "config.h"
#include <list>
#include <istream>
#include <ostream>
#include <cstdint>

struct ski_rd_memory_access {
	int cpu;
	target_ulong physical_memory_address;
	target_ulong ip_address;
	int length; // bits or bytes?
	int is_read;
	int instruction_count;
	std::list<target_ulong> trace;
};

struct ski_rd_race {
	ski_rd_memory_access m1;
	ski_rd_memory_access m2;
};

void load_trace(std::list<target_ulong>& trace, std::istream& is);
std::list<ski_rd_race> load_race(const char*);
void print_race(const std::list<ski_rd_race>& races, std::ostream& os);

#endif /* SKI_RACE_H_ */
