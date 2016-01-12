#include "ski-race.h"
#include <sstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <fstream>

#include <boost/format.hpp>

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

template<class T>
void myfread(FILE* fp, T* p, size_t n=1){
	size_t ret = fread(p, sizeof(T), n, fp);
	if(ret!=n) {
		throw runtime_error("db is invalid");
	}
}

ski_stktrace load_stktrace(const char* file){
	ski_stktrace ret;
	unique_ptr<FILE, decltype(ptr_fun(fclose))> fp(fopen(file, "rb"), ptr_fun(fclose));
	uint32_t total_vars;
	myfread(fp.get(), &total_vars);
	for(uint32_t i=0;i!=total_vars;++i){
		target_ulong var_addr;
		myfread(fp.get(), &var_addr);
		uint32_t var_race_count;
		myfread(fp.get(), &var_race_count);
		uint32_t total_sts;
		myfread(fp.get(), &total_sts);
		var_st_list_entry entry{var_addr, var_st_list_val{var_race_count, st_list{}}};
		for(uint32_t j=0;j!=total_sts;++j){
			uint32_t total_raddrs;
			myfread(fp.get(), &total_raddrs);
			target_ulong raddrs[total_raddrs];
			myfread(fp.get(), raddrs, total_raddrs);
			entry.second.second.push_back(st_entry(raddrs + 0, raddrs + total_raddrs));
		}
		ret.push_back(move(entry));
	}
	try{
		char last;
		myfread(fp.get(), &last);
	}
	catch (...){
		return ret;
	}
	//extra bytes in db
	throw runtime_error("db is invalid");
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
			cerr << boost::format("line [%s] ignored (invalid format)") % line.c_str() << endl;
			continue;
		}
		list<target_ulong> tmp1;
		list<target_ulong> tmp2;

		auto ptrace1 = &tmp1;
		auto ptrace2 = &tmp2;

		if(race.m1.ip_address < TASK_SIZE || race.m2.ip_address < TASK_SIZE){
			cerr << boost::format("ins pair " TARGET_ULONG_FMT " - " TARGET_ULONG_FMT " ignored (not in kernel space)")
					% race.m1.ip_address % race.m2.ip_address << endl;
		}
		else{
			ptrace1 = &race.m1.trace;
			ptrace2 = &race.m2.trace;
		}
		load_trace(*ptrace1, is);
		load_trace(*ptrace2, is);
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
