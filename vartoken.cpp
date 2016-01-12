#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>
#include <map>
#include <iterator>
#include <algorithm>
#include <cassert>

using namespace std;

static const char* linestr;
static unsigned long lineno;

pair<string, unsigned long> get_line_no(char* line){
    auto len = strlen(line);
    if(!len || line[0] != '#'){
        return make_pair("", 0);
    }
    auto q1 = find(line, line + len, '\"');
    assert(q1 != line + len);
    reverse_iterator<char*> ri(line + len), rj(line);
    auto rq = find(ri, rj, '\"');
    assert(rq.base() != line);
    auto q2 = rq.base() - 1;
    *q1 = '\0';
    *q2 = '\0';
    return make_pair(q1+1, strtoul(line + 1, nullptr, 0));
};

char* get_target_line(FILE* fp){
    ssize_t read;
    char* line = nullptr;
    size_t len = 0;
    pair<string, unsigned long> it("", 0);
    while((read = getline(&line, &len, fp)) != -1){
        auto ret = get_line_no(line);
        if(!ret.first.empty()){
            it = move(ret);
        }else{
            assert(ret.second == 0);
            if(lineno == it.second && !strcmp(linestr, it.first.c_str())){
                return line;
            }
            ++it.second;
        }

    }
    free(line);
    return nullptr;
}

list<string> tokenize(char* str){
    list<string> ret;
    size_t begin = 0, len = strlen(str);
    for(size_t i=0;i!=len;++i){
        if(!isalpha(str[i])){
            if(begin != i){
                str[i] = '\0';
                ret.push_back(str+begin);
            }
            begin = i+1;
        }
    }
    if(begin != len){
        ret.push_back(str + begin);
    }
    return ret;
}

int main(int, char** argv){
    linestr = argv[1];
    lineno = strtoul(argv[2], nullptr, 0);
    char* l = get_target_line(stdin);
    fprintf(stderr, "line = %s", l);
    auto tokens = tokenize(l);
    for(auto& t : tokens){
        fprintf(stderr, "%s ", t.c_str());
    }
    fputc('\n', stderr);
    free(l);
    return 0;
}