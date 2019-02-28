#include "BtreeHelper.hpp"


template <typename NodeType, typename Key>
void
BtreeHelper::change_bound_upwards(NodeType* node, const Key& old_key,const Key& new_key) const
{
    // TODO does the father class have access the Node member?
    node->father_->change_key(old_key, new_key);
}
