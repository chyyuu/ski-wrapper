#include "config.h"
#include "sympp.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <set>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

int main(int, char **argv) {
    set<calltrace> traces;
    string sym;
    target_ulong offset;
    calltrace trace;
    while (cin >> hex >> sym >> offset) {
        trace.push_back(make_pair(move(sym), offset));
    }
    cout << "--------------------------------------------------------------------------------" << endl;
    for (auto &e : trace) {
        cout << e.first << "+" << hex << e.second << endl;
    }
    traces.insert(move(trace));

    ofstream trace_xml(argv[1]);
    boost::archive::xml_oarchive arch_trace(trace_xml);

    arch_trace << BOOST_SERIALIZATION_NVP(traces);
    return 0;
}