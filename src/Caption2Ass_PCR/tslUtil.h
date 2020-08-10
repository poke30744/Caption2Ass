//------------------------------------------------------------------------------
// tslUtil.h
//------------------------------------------------------------------------------
#ifndef __TSL_UTIL_H__
#define __TSL_UTIL_H__

#include "CommTypes.h"

#include "packet_types.h"

extern bool_t FindStartOffset(FILE *fp);
extern bool_t resync(byte_t *pbPacket, FILE *fp);

extern long long GetPTS(byte_t *pbPacket);
extern void parse_PAT(byte_t *pbPacket, ushort_t *PMTPid);
extern void parse_PMT(byte_t *pbPacket, ushort_t *PCRPid, ushort_t *CaptionPid);
extern void parse_Packet_Header(Packet_Header *packet_header, byte_t *pbPacket);

#endif // __TSL_UTIL_H__
