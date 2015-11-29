#include <memory>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cstdio>

#include "sympp.h"

#include <boost/format.hpp>

using namespace std;

typedef uint32_t target_ulong;
#define TARGET_ULONG_FMT "%08X"

struct ski_rd_memory_access
{
	int cpu;
	target_ulong physical_memory_address;
	target_ulong ip_address;
	int length; // bits or bytes?
	int is_read;
	int instruction_count;
	list<target_ulong> trace;
} ;

struct ski_rd_race
{
	ski_rd_memory_access m1;
	ski_rd_memory_access m2;
};

void load_trace(istream& is, list<target_ulong>& trace){
	string line;
	getline(is, line);
	stringstream ss(line);
	target_ulong addr;
	while(ss >> hex >> addr){
		trace.push_back(addr);
	}
}

void print_trace_sym(const ksyms& sym_map, const ski_rd_memory_access& acc, ostream& os){
	static const char* rw_type[] = {"write", "read"};
	os << boost::format("%s %08x len = %d") % rw_type[!!acc.is_read] % acc.physical_memory_address %
			acc.length << endl;
	for(auto& raddr : acc.trace){
		auto isym = sym_map.upper_bound(raddr);
		--isym;
		os << boost::format("%08x %s+%x") % raddr % isym->second.c_str() % (raddr - isym->first) << endl;
	}
	os << endl;
}

list<ski_rd_race> load_race(istream& is){
	list<ski_rd_race> ret;
	string line;
	while(getline(is, line)){
		ski_rd_race race;
		int count = sscanf(line.c_str(),
				"T: %*s S: %*d I1: %*d I2: %*d IP1: %x IP2: %x PMA1: %x PMA2: %x CPU1: %d CPU2: %d R1: %d R2: %d L1: %d L2: %d IC1: %d IC2: %d",
				&race.m1.ip_address, &race.m2.ip_address, &race.m1.physical_memory_address, &race.m2.physical_memory_address,
				&race.m1.cpu, &race.m2.cpu, &race.m1.is_read, &race.m2.is_read, &race.m1.length, &race.m2.length, &race.m1.instruction_count, &race.m2.instruction_count);
		if(count != 12){
			continue;
		}
		load_trace(is, race.m1.trace);
		load_trace(is, race.m2.trace);
		ret.push_back(race);
	}
	return ret;
}

void print_race(const list<ski_rd_race>& races, ostream& os){
	for(auto& e : races){
		auto& m1 = e.m1;
		auto& m2 = e.m2;
		os << hex << "IP1: " << m1.ip_address << " IP2: " << m2.ip_address <<
				" PMA1: " << m1.physical_memory_address << " PMA2: " << m2.physical_memory_address <<
				" CPU1: " << m1.cpu << " CPU2: " << m2.cpu << " R1: " << m1.is_read << " R2: " << m2.is_read <<
				" L1: " << m1.length << " L2: " << m2.length << dec << " IC1: " << m1.instruction_count << " IC2: " << m2.instruction_count <<endl;
	}
}

void print_map(const map<target_ulong, string>& sym_map, ostream& os){
	for(auto& e : sym_map){
		os << boost::format("%s " TARGET_ULONG_FMT) % e.second.c_str() % e.first << endl;
	}
}

static const target_ulong task_size = 0xC000000UL;
void print_race_sym(const ksyms& sym, const list<ski_rd_race>& races, ostream& os){
	//const char* type[] = {"write", "read"};
	auto i=races.begin();
	auto j=races.end();
	for(size_t k =0;i!=j;++i,++k){
		auto& race = *i;
		if(race.m1.ip_address < task_size && race.m2.ip_address < task_size){
			continue;
		}
		/*auto p1 = sym_map.upper_bound(race.m1.ip_address);
		auto p2 = sym_map.upper_bound(race.m2.ip_address);
		--p1, --p2;
		os << dec << k << hex << ": " <<
				"[" << race.m1.cpu << "]" << p1->second << "+" << race.m1.ip_address - p1->first << "(" << race.m1.ip_address << ")" << ", " <<
				"[" << race.m2.cpu << "]" << p2->second << "+" << race.m2.ip_address - p2->first << "(" << race.m2.ip_address << ")" <<
				" mem1: " << race.m1.physical_memory_address << "(" << type[race.m1.is_read] << race.m1.length << ")" <<
				" mem2: " << race.m2.physical_memory_address << "(" << type[race.m2.is_read] << race.m2.length << ")" << endl;
		*/
		print_trace_sym(sym, race.m1, os);
		print_trace_sym(sym, race.m2, os);
		os << "--------------------------------------------------------------------------------" << endl;
	}
}

int main(int, char** argv){
	auto sym = load_syms(argv[1]);
	//print_map(sym_map, cout);
	ifstream race_log(argv[2]);
	auto race_list = load_race(race_log);
	//print_race(race_list, cout);
	print_race_sym(sym, race_list, cout);
	return 0;
}
