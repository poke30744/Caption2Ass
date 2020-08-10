//------------------------------------------------------------------------------
// packet_types.h
//------------------------------------------------------------------------------
#ifndef __PACKET_TYPES_H__
#define __PACKET_TYPES_H__

#include "CommTypes.h"

typedef struct Packet_Header {
    byte_t    Sync;
    byte_t    TsErr;
    byte_t    PayloadStartFlag;
    byte_t    Priority;
    word_t    PID;
    byte_t    Scramble;
    byte_t    AdaptFlag;
    byte_t    PayloadFlag;
    byte_t    Counter;
} Packet_Header;

#pragma pack(push, 1)

typedef struct _Packet_Header {
    byte_t    Sync;
    word_t    PID;
    byte_t    Counter;
} _Packet_Header;

typedef struct PMT_PID {
    word_t        program_id;     // 0-> PMT_PID = NetwokPID
    word_t        PID;            // &0x1FFF
} PMT_PID;

typedef struct PAT_HEADER {
    byte_t        table_id;       //0x00
    word_t        section_length; // &0x0FFF
    word_t        transport_id;
    byte_t        version;        //version>>5 &0x1F
    byte_t        section_number;
    byte_t        last_section_number;
    PMT_PID     PMT_Array[1];
} PAT_HEADER;

typedef struct PMT_HEADER {
    byte_t        table_id;
    word_t        section_length; // &0x0FFF
    word_t        program_id;
    byte_t        version;        //version>>5 &0x1F
    byte_t        section_number;
    byte_t        last_section_number;
    word_t        pcrpid;         //13bit & 0x1fff
    word_t        program_info_length;    // &0x0FFF
    /*
    byte_t program_info[program_info_length];
    desc_records {
        byte_t    StreamTypeID;
        word_t    EsPID;              // +1,2
        word_t    DescLen;            // +3,4
        byte_t    Desc[DescLen]
    }
    */
} PMT_HEADER;

typedef struct PMT_PID_Desc {
    byte_t    StreamTypeID;
    word_t    EsPID;                  // +1,2
    word_t    DescLen;                // +3,4
    byte_t    descriptor_tag;
    byte_t    descriptor_length;
    byte_t    component_tag;
} PMT_PI_Desc;

#pragma pack(pop)

#define swap16(x) ((((word_t)(x) & 0xFF00) >> 8) | (((word_t)(x) & 0x00FF) << 8))

#endif // __PACKET_TYPES_H__
