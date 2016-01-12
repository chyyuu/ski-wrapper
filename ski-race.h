#ifndef SKI_RACE_H_
#define SKI_RACE_H_

#include "config.h"
#include <list>
#include <map>
#include <vector>
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

typedef std::pair<target_ulong, size_t> var_stat;

typedef std::list<target_ulong> st_entry;
typedef std::list<st_entry> st_list;
typedef std::pair<size_t, st_list> var_st_list_val;
typedef std::pair<target_ulong, var_st_list_val> var_st_list_entry;
typedef std::list<var_st_list_entry> ski_stktrace;

ski_stktrace load_stktrace(const char*);
std::list<ski_rd_race> load_race(const char*);
void print_race(const std::list<ski_rd_race>& races, std::ostream& os);

#endif /* SKI_RACE_H_ */
