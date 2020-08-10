//------------------------------------------------------------------------------
// CaptionMain.h
//------------------------------------------------------------------------------
#ifndef __CAPTION_MAIN_H__
#define __CAPTION_MAIN_H__

#include "CommTypes.h"

#include "CaptionDef.h"

#define RESET_JD_SYSTEMTIME(p)   { if (p) { *p.wYear = 1991; *p.wMonth = 12; *p.wDay = 31; *p.wHour = 0; *p.wMinute = 0; *p.wSecond = 0; *p.wMilliseconds = 0; *p.wDayOfWeek = 0; } }
#define RESET_MJD_SYSTEMTIME(p)  { if (p) { *p.wYear = 1858; *p.wMonth = 11; *p.wDay = 17; *p.wHour = 0; *p.wMinute = 0; *p.wSecond = 0; *p.wMilliseconds = 0; *p.wDayOfWeek = 0; } }

class CCaptionMain
{
public:
    typedef struct _LANG_TAG_INFO {
        unsigned char ucLangTag;
        unsigned char ucDMF;
        unsigned char ucDC;
        char szISOLangCode[4];
        unsigned char ucFormat;
        unsigned char ucTCS;
        unsigned char ucRollupMode;
        //=オペレーターの処理
        _LANG_TAG_INFO & operator= (const _LANG_TAG_INFO & o) {
            ucLangTag = o.ucLangTag;
            ucDMF = o.ucDMF;
            ucDC = o.ucDC;
            memcpy(szISOLangCode, o.szISOLangCode, 4);
            ucFormat = o.ucFormat;
            ucTCS = o.ucTCS;
            ucRollupMode = o.ucRollupMode;
            return *this;
        };
    } LANG_TAG_INFO;

public:
    CCaptionMain(bool_t bUNICODE);
    ~CCaptionMain(void);

    dword_t AddTSPacket(byte_t *pbPacket);
    dword_t Clear(void);

    dword_t GetTagInfo(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount);
    dword_t GetCaptionData(unsigned char ucLangTag, CAPTION_DATA_DLL **ppList, dword_t *pdwListCount);

protected:
    typedef struct _PAYLOAD_DATA {
        byte_t bBuff[188];
        word_t wSize;
    } PAYLOAD_DATA;
    vector<PAYLOAD_DATA*> m_PayloadList;

    map<word_t, LANG_TAG_INFO> m_LangTagList;

    typedef struct _CAPTION_LIST {
        word_t wGroupID;
        vector<CAPTION_DATA> CaptionList;
    } CAPTION_LIST;
    map<word_t, CAPTION_LIST> m_CaptionMap;

    unsigned char m_ucVersion;

    int m_iLastCounter;
    bool_t m_bAnalyz;
    dword_t m_dwNowReadSize;
    dword_t m_dwNeedSize;

    LANG_TAG_INFO_DLL *m_pLangList;
    dword_t m_dwLangListCount;
    CAPTION_DATA_DLL *m_pCapList;
    dword_t m_dwCapListCount;

protected:
    dword_t GetTagInfo(vector<CCaptionMain::LANG_TAG_INFO> *pList);
    dword_t GetCaptionData(unsigned char ucLangTag, vector<CAPTION_DATA> *pList);

    dword_t ParseListData(void);
    dword_t ParseCaption(byte_t *pbBuff, dword_t dwSize);
    dword_t ParseCaptionManagementData(byte_t *pbBuff, dword_t dwSize, vector<CAPTION_DATA> *pCaptionList);
    dword_t ParseCaptionData(byte_t *pbBuff, dword_t dwSize, vector<CAPTION_DATA> *pCaptionList);
    dword_t ParseUnitData(byte_t *pbBuff, dword_t dwSize, dword_t *pdwReadSize, vector<CAPTION_DATA> *pCaptionList);

    bool_t InsertCaptionList(word_t wGroupID, vector<CAPTION_DATA> *pCaptionList);
};

#endif // __CAPTION_MAIN_H__
