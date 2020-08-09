//------------------------------------------------------------------------------
// CalcMD5.cpp
//------------------------------------------------------------------------------

#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

#include "CalcMD5.h"

std::string CalcMD5::md5sum(void const* buffer, std::size_t byte_count)
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;

    hash.process_bytes(buffer, byte_count);
    hash.get_digest(digest);

    std::string result;
    boost::algorithm::hex(digest, digest + _countof(digest), std::back_inserter(result));
    return result;
}
