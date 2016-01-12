#include "config.h"
#include "sympp.h"
#include "ski-race.h"

#include <memory>
#include <fstream>

#include <boost/format.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

void print_traces(const calltrace& traces, ostream& os){
	for(auto& e : traces){
		os << hex << e.first << "+" << e.second << endl;
	}
	os << "--------------------------------------------------------------------------------" << endl;
}

void stktrace2trace(const ski_stktrace& stktrace, const ksyms& sym, set<calltrace>& reads, ostream& os){
	for(auto& var : stktrace){
		os << boost::format("var = " TARGET_ULONG_FMT " cnt = %u") % var.first % var.second.first << endl << endl;
		for(auto& st : var.second.second){
			auto it = reads.insert(convert_addr(st, sym));
			print_traces(*it.first, os);
		}
	}
}

int main(int, char** argv){
	auto sym = load_syms(argv[1]);
	//print_map(sym_map, cout);
	auto stktrace = load_stktrace(argv[2]);

	set<calltrace> read_trace;

	stktrace2trace(stktrace, sym, read_trace, cout);

	ofstream readtrace_xml(argv[3]);
	boost::archive::xml_oarchive arch_readtrace(readtrace_xml);

	arch_readtrace << BOOST_SERIALIZATION_NVP(read_trace);

	return 0;
}
