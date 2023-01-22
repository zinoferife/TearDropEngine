#include "Main.h"


//class contructor called premain, not part of init
std::vector<td::Main::func_t> td::Main::mInitArray{};
std::vector<td::Main::func_t> td::Main::mTerminateArray{};


void td::Main::AddInitFunc(const td::Main::func_t& Ifunc)
{
    mInitArray.emplace_back(Ifunc);
}

void td::Main::AddTermFunc(const td::Main::func_t& Tfunc)
{
    mTerminateArray.emplace_back(Tfunc);
}

void td::Main::Init()
{
    //objects should not be used 
    for(auto& func: mInitArray){
        //call the init functions
        func();
    }
    mInitArray.clear();
}
void td::Main::Term()
{
    for(auto& func: mTerminateArray){
        //call term functions;
        func();
    }
    mTerminateArray.clear();
}
