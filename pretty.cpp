#include "config.h"
#include "sympp.h"
#include "ski-race.h"
#include "converter.h"

#include <memory>
#include <fstream>

#include <boost/format.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

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


void print_race_sym(const ksyms& sym, const list<ski_rd_race>& races, ostream& os){
	//const char* type[] = {"write", "read"};
	auto i=races.begin();
	auto j=races.end();
	for(size_t k =0;i!=j;++i,++k){
		auto& race = *i;
		/*if(race.m1.ip_address < task_size && race.m2.ip_address < task_size){
			continue;
		}*/
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
	auto race_list = load_race(argv[2]);
	//print_race(race_list, cout);
	//print_race_sym(sym, race_list, cout);
	set<calltrace> read_trace;
	set<calltrace> write_trace;
	race2trace(race_list, sym, read_trace, write_trace);

	ofstream readtrace_xml(argv[3]);
	ofstream writetrace_xml(argv[4]);
	boost::archive::xml_oarchive arch_readtrace(readtrace_xml);
	boost::archive::xml_oarchive arch_writetrace(writetrace_xml);

	arch_readtrace << BOOST_SERIALIZATION_NVP(read_trace);
	arch_writetrace << BOOST_SERIALIZATION_NVP(write_trace);

	return 0;
}
