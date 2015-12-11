#include "converter.h"

using namespace std;

void race2trace(const list<ski_rd_race>& races, const ksyms& sym, set<calltrace>& read, set<calltrace>& write){
	for(auto& race : races){
		if(race.m1.is_read){
			read.insert(convert_addr(race.m1.trace, sym));
		}else{
			write.insert(convert_addr(race.m1.trace, sym));
		}
		if(race.m2.is_read){
			read.insert(convert_addr(race.m2.trace, sym));
		}else{
			write.insert(convert_addr(race.m2.trace, sym));
		}
	}
}
