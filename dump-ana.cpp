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
	uint32_t EAX;
	uint32_t ECX;
	uint32_t EDX;
	uint32_t EBX;
	uint32_t ESP;
	uint32_t EBP;
	uint32_t ESI;
	uint32_t EDI;
	uint32_t EIP;
};

typedef uint8_t stack[8192];

#define TASK_SIZE 0xc0000000UL

void print_trace(const stack& stk, const regs& reg, const ksyms& sym, ostream& os){
	uint32_t ebp = reg.EBP, base = reg.ESP & -sizeof(stack), raddr = reg.EIP;
	if(base < TASK_SIZE){
		return;
	}
	for(;;){
		auto isym = sym.upper_bound(raddr);
		--isym;
		os << boost::format("  %08x %s+%x") %
				raddr % isym->second.c_str() % (raddr - isym->first) << endl;
		if(ebp <= base || ebp > base + sizeof(stack) - sizeof(uint32_t) * 2){
			break;
		}
		auto prevbp = ebp - base + stk;
		ebp = *(uint32_t*)prevbp;
		raddr = *(uint32_t*)(prevbp + sizeof(uint32_t));
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
