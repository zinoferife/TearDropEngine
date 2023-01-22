//main class for pre and post functions call

#include <vector>
#include <functional>


namespace td{
class Main {
    public:
                typedef std::function<void(void)> func_t;
                
                static void AddInitFunc(const func_t& Ifunc);
                static void AddTermFunc(const func_t& Tfunc);
                static void Init();
                static void Term();
                
                
    private:
             static std::vector<func_t> mInitArray;
             static std::vector<func_t> mTerminateArray;
                
                
};
}