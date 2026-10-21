#pragma once

#include "aya/Boost.hpp"


namespace Aya
{
class PartInstance;
}


namespace boost
{
std::size_t hash_value(const shared_ptr<Aya::PartInstance>& b);
}
