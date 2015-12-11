#include "config.h"
#include "sympp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdint>
#include <iostream>

#include <boost/format.hpp>

using namespace std;

struct regs{
	target_ulong EAX;
	target_ulong ECX;
	target_ulong EDX;
	target_ulong EBX;
	target_ulong ESP;
	target_ulong EBP;
	target_ulong ESI;
	target_ulong EDI;
	target_ulong EIP;
};

typedef uint8_t stack[8192];

void print_trace(const stack& stk, const regs& reg, const ksyms& sym, ostream& os){
	target_ulong ebp = reg.EBP, base = reg.ESP & -sizeof(stack), raddr = reg.EIP;
	if(base < TASK_SIZE){
		return;
	}
	for(;;){
		auto isym = sym.upper_bound(raddr);
		--isym;
		os << boost::format("  %08x %s+%x") %
				raddr % isym->second.c_str() % (raddr - isym->first) << endl;
		if(ebp <= base || ebp > base + sizeof(stack) - sizeof(target_ulong) * 2){
			break;
		}
		auto prevbp = ebp - base + stk;
		ebp = *(target_ulong*)prevbp;
		raddr = *(target_ulong*)(prevbp + sizeof(target_ulong));
	}
}


int main(int argc, char** argv){
	int fd_stack = open(argv[1], O_RDONLY);
	if(fd_stack == -1){
		perror("open stack dump failed");
	}
	int fd_regs = open(argv[2], O_RDONLY);
	if(fd_regs == -1){
		perror("open register dump failed");
	}
	stack* p_stack = (stack*)mmap(NULL, sizeof(stack), PROT_READ, MAP_SHARED, fd_stack, 0);
	if(p_stack == (void*)-1){
		perror("map stack dump failed");
	}
	regs* p_regs = (regs*)mmap(NULL, sizeof(regs), PROT_READ, MAP_SHARED, fd_regs, 0);
	if(p_regs == (void*)-1){
		perror("map register dump failed");
	}
	printf("REGS: EIP=%08x\n"
			"EAX=%08x ECX=%08x EDX=%08x EBX=%08x\n"
			"ESP=%08x EBP=%08x ESI=%08x EDI=%08x\n",
			p_regs->EIP,
			p_regs->EAX, p_regs->ECX, p_regs->EDX, p_regs->EBX,
			p_regs->ESP, p_regs->EBP, p_regs->ESI, p_regs->EDI);
	auto sym = load_syms(argv[3]);
	print_trace(*p_stack, *p_regs, sym, cout);
	return 0;
}
