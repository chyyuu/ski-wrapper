#include "ski-race.h"
#include <sstream>
#include <fstream>

using namespace	std;

void load_trace(list<target_ulong>& trace, istream& is){
	string line;
	getline(is, line);
	stringstream ss(line);
	target_ulong addr;
	while(ss >> hex >> addr){
		trace.push_back(addr);
	}
}

list<ski_rd_race> load_race(const char* file){
	list<ski_rd_race> ret;
	string line;
	ifstream is(file);
	while(getline(is, line)){
		ski_rd_race race;
		int count = sscanf(line.c_str(),
				"T: %*s S: %*d I1: %*d I2: %*d IP1: %x IP2: %x PMA1: %x PMA2: %x CPU1: %d CPU2: %d R1: %d R2: %d L1: %d L2: %d IC1: %d IC2: %d",
				&race.m1.ip_address, &race.m2.ip_address, &race.m1.physical_memory_address, &race.m2.physical_memory_address,
				&race.m1.cpu, &race.m2.cpu, &race.m1.is_read, &race.m2.is_read, &race.m1.length, &race.m2.length, &race.m1.instruction_count, &race.m2.instruction_count);
		if(count != 12){
			continue;
		}
		load_trace(race.m1.trace, is);
		load_trace(race.m2.trace, is);
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
