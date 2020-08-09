//------------------------------------------------------------------------------
// CalcMD5.h
//------------------------------------------------------------------------------
#ifndef __CALC_MD5_H__
#define __CALC_MD5_H__

#include <string>

#define MD5_HASH_HEX_LENGTH     32

namespace CalcMD5
{
	std::string md5sum(void const* buffer, std::size_t byte_count);
}

#endif // __CALC_MD5_H__
