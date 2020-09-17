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
            _cache->Clear();// 如何保证，通知清除缓存
            // 显式调用析构函数和局部变量自动析构的过程一样吗？
            // 是只调用了析构函数，而不调用成员变量的析构函数
            // 经实验，和自动析构的过程一样
            // 这里析构了，_cache 自身就不能析构了
            // 这里写这些的感觉循环引用的时候，需要某一方来手动控制析构

            // 这里析构了，里面的引用没有了，
            // 所以外面这里 _cache 的 shared_ptr 可以正常析构会调用析构函数了，。
            // 那么就相当于有调用了 _cache->~Cache();
            // 但因为析构函数没有参数，_cache 不知道这次不用实际操作，或者他自己内部可以用个变量
            // 标明进行过析构
            // 但这样不好，还是像上面那样说的，用个显式的、单独的函数好一些。
            // 这个显式的函数里只要把 shared_

            // 那些大项目是如何解决循环引用的问题呢，比如 Office、QT
        }
        else
        {
            /* code */
        }
        
    }
};
