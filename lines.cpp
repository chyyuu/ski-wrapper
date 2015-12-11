#include "config.h"
#include "sympp.h"

#include <fstream>
#include <sstream>
#include <pstreams/pstream.h>

#include <boost/format.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

#define ADDR2LINE_FORMAT "addr2line -e \"%s\" 0x" TARGET_ULONG_FMT

typedef list<pair<string, unsigned>> srclines;

srclines addr2line(const char* exe, target_ulong addr){
    srclines ret;
    int buf_num = snprintf(NULL, 0, ADDR2LINE_FORMAT, exe, addr);
    char cmdline[buf_num + 1];
    sprintf(cmdline, ADDR2LINE_FORMAT, exe, addr);
    cout << cmdline << endl;
    redi::ipstream in(cmdline);
    string line;
    while(getline(in, line)){
        cout << line << endl;
    }
    return ret;
}

int main(int, char** argv){
    auto sym = load_revsyms(argv[1]);

    set<calltrace> trace;
    ifstream trace_xml(argv[2]);
    boost::archive::xml_iarchive arch_trace(trace_xml);
    arch_trace >> BOOST_SERIALIZATION_NVP(trace);

    for(auto& t : trace){
        for(auto& c : t){
            assert(sym.find(c.first) != sym.end());
            addr2line(argv[3], sym[c.first] + c.second);
        }
    }
    return 0;
}