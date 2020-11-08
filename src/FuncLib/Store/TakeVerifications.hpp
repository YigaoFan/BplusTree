#include <vector>
#include "StaticConfig.hpp"
#include "VectorUtil.hpp"

namespace FuncLib::Store
{
    using ::std::vector;

	class TakeVerifications
    {
    private:
        vector<pos_lable> _lables;
    public:
        auto begin() const { return _lables.begin(); }
        auto end() const { return _lables.end(); }
        auto begin() { return _lables.begin(); }
        auto end() { return _lables.end(); }
        void Verified(pos_lable lable)
        {
            for (size_t i = 0; i < _lables.size(); ++i)
            {
                if (_lables[i] == lable)
                {
                    Erase(i, _lables);
                    return;
                }
            }
        }
    };
}
