#include <vector>
#include "StaticConfig.hpp"
#include "VectorUtil.hpp"

namespace FuncLib::Store
{
    using ::std::vector;

	class TakeVerifications
    {
    private:
        vector<pos_label> _labels;
    public:
        auto begin() const { return _labels.begin(); }
        auto end() const { return _labels.end(); }
        auto begin() { return _labels.begin(); }
        auto end() { return _labels.end(); }
        void Verified(pos_label label)
        {
            for (size_t i = 0; i < _labels.size(); ++i)
            {
                if (_labels[i] == label)
                {
                    Erase(i, _labels);
                    return;
                }
            }
        }
    };
}
