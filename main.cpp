#include <memory>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdio>

using namespace std;

map<uintptr_t, string> load_map(istream& is){
	map<uintptr_t, string> ret;
	string line;
	while(getline(is, line)){
		void* addr;
		char type;
		int pos;
		int count = sscanf(line.c_str(), "%p %c %n", &addr, &type, &pos);
		if(count!=2){
			continue;
		}
		ret[(uintptr_t)addr] = line.c_str() + pos;
	}
	return ret;
}
struct ski_rd_memory_access
{
	int cpu;
	unsigned physical_memory_address;
	unsigned ip_address;
	int length; // bits or bytes?
	int is_read;
	int instruction_count;
} ;

 struct ski_rd_race
{
	ski_rd_memory_access m1;
	ski_rd_memory_access m2;
} ;

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

void print_map(const map<uintptr_t, string>& sym_map, ostream& os){
	for(auto& e : sym_map){
		os << e.second << " "<< (void*)e.first << endl;
	}
}

static const uintptr_t task_size = 0xC000000UL;
void print_race_sym(const map<uintptr_t, string>& sym_map, const list<ski_rd_race>& races, ostream& os){
	const char* type[] = {"write", "read"};
	auto i=races.begin();
	auto j=races.end();
	for(size_t k =0;i!=j;++i,++k){
		auto& race = *i;
		if(race.m1.ip_address < task_size && race.m2.ip_address < task_size){
			continue;
		}
		auto p1 = sym_map.upper_bound(race.m1.ip_address);
		auto p2 = sym_map.upper_bound(race.m2.ip_address);
		--p1, --p2;
		os << dec << k << hex << ": " <<
				"[" << race.m1.cpu << "]" << p1->second << "+" << race.m1.ip_address - p1->first << "(" << race.m1.ip_address << ")" << ", " <<
				"[" << race.m2.cpu << "]" << p2->second << "+" << race.m2.ip_address - p2->first << "(" << race.m2.ip_address << ")" <<
				" mem1: " << race.m1.physical_memory_address << "(" << type[race.m1.is_read] << race.m1.length << ")" <<
				" mem2: " << race.m2.physical_memory_address << "(" << type[race.m2.is_read] << race.m2.length << ")" << endl;
	}
}

int main(int, char** argv){
	auto system_map = argv[1];
	ifstream if_system_map(system_map);
	auto sym_map = load_map(if_system_map);
	//print_map(sym_map, cout);
	auto race_log = argv[2];
	ifstream if_race_log(race_log);
	auto race_list = load_race(if_race_log);
	//print_race(race_list, cout);
	print_race_sym(sym_map, race_list, cout);
	return 0;
}
