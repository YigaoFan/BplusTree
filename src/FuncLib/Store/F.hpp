#include <memory>
using namespace std;

class Cache
{

};

class F
{
private:
    bool _outsideRef;
    shared_ptr<Cache> _cache;
public:
    F();
    ~F()
    {
        // 假设外面只有一个 ref，所以这里析构了，就要把 _cache 析构了
        if (_outsideRef)
        {
            // 下面这个是不是可以显式的写为一个函数呢
            _cache->~Cache();// 如何保证
            // 显式调用析构函数和局部变量自动析构的过程一样吗？
            // 是只调用了析构函数，而不调用成员变量的析构函数
            // 经实验，和自动析构的过程一样
            // 这里析构了，_cache 自身就不能析构了
            // 这里写这些的感觉循环引用的时候，需要某一方来手动控制析构

            // 这里析构了，里面的引用没有了，
            // 所以外面这里 _cache 的 shared_ptr 可以正常析构了，
            // 但这时也不需要他析构了。
        }
        else
        {
            /* code */
        }
        
    }
};
