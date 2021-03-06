//------------------------------------------------------------------------------
// CaptionMain.cpp
//------------------------------------------------------------------------------
#pragma warning(disable: 4100)

#include <map>

#include <Windows.h>

#include "CaptionDef.h"
#include "ColorDef.h"
#include "ARIB8CharDecode.h"
#include "IniFile.h"
#include "CaptionMain.h"
#include "packet_types.h"

// ARIBの追加記号 ＆ 追加漢字のテーブル定義
#define GLOBAL
#include "ARIBGaiji.h"

map<string, string> dicHash_Char_map;
map<word_t, string> dicCharcode_Char_map;

CCaptionMain::CCaptionMain(bool_t bUNICODE)
{
    m_iLastCounter = -1;
    m_bAnalyz = true;

    m_ucVersion = 0xFF;

    m_pLangList = 0;
    m_dwLangListCount = NULL;
    m_pCapList = 0;
    m_dwCapListCount = NULL;

    IniFile objIniFile;
    m_bUnicode = bUNICODE;
    if (bUNICODE) {
        objIniFile.ReadIniUNICODE();
        objIniFile.ReadIniARIBUNICODE();
    } else {
        objIniFile.ReadIni();
        objIniFile.ReadIniARIB();
    }
}

CCaptionMain::~CCaptionMain(void)
{
    for (int i = 0; i < (int)m_PayloadList.size(); i++)
        SAFE_DELETE(m_PayloadList[i]);

    m_PayloadList.clear();

    if (m_pLangList) {
        SAFE_DELETE_ARRAY(m_pLangList);
        m_dwLangListCount = 0;
    }
    if (m_pCapList) {
        for (dword_t i = 0; i < m_dwCapListCount; i++) {
            for (dword_t j = 0; j < m_pCapList[i].dwListCount; j++)
                SAFE_DELETE(m_pCapList[i].pstCharList[j].pszDecode);
            SAFE_DELETE_ARRAY(m_pCapList[i].pstCharList);
        }
        SAFE_DELETE_ARRAY(m_pCapList);
    }
}

dword_t CCaptionMain::Clear(void)
{
    for (int i = 0; i < (int)m_PayloadList.size(); i++)
        SAFE_DELETE(m_PayloadList[i]);

    m_PayloadList.clear();

    m_LangTagList.clear();
    m_CaptionMap.clear();
    m_ucVersion = 0xFF;

    m_iLastCounter = -1;
    m_bAnalyz = true;
    return 0;
}

dword_t CCaptionMain::AddTSPacket(byte_t *pbPacket)
{
    if (!pbPacket) {
        Clear();
        return false;
    }
    byte_t bb[188];
    memcpy(bb, pbPacket, 188);

    Packet_Header ph;
    ph.Sync             = pbPacket[0];
    ph.TsErr            = (pbPacket[1] & 0x80) >> 7;
    ph.PayloadStartFlag = (pbPacket[1] & 0x40) >> 6;
    ph.Priority         = (pbPacket[1] & 0x20) >> 5;
    ph.PID              = ((unsigned short)(pbPacket[1] & 0x1F)) << 8 | pbPacket[2];
    ph.Scramble         = (pbPacket[3] & 0xC0) >> 6;
    ph.AdaptFlag        = (pbPacket[3] & 0x20) >> 5;
    ph.PayloadFlag      = (pbPacket[3] & 0x10) >> 4;
    ph.Counter          = (pbPacket[3] & 0x0F);

    if (ph.Sync != 0x47) {
        Clear();
        return ERR_CAN_NOT_ANALYZ;
    }

    //パケットカウンターチェック
    if (m_iLastCounter == -1)
        m_iLastCounter = (int)ph.Counter;
    else {
        if (ph.Counter == 0x00) {
            if (m_iLastCounter != 0x0F)
                Clear();
            else
                m_iLastCounter = (int)ph.Counter;
        } else {
            if (ph.Counter != m_iLastCounter + 1)
                Clear();
            else
                m_iLastCounter = (int)ph.Counter;
        }
    }

    unsigned char ucAdaptLength = 0;
    unsigned char ucPayloadOffset = 0;
    dword_t dwStart = 4;
    bool_t bBeforeParse = false;

    //アダプテーションフィールドは飛ばす
    if (ph.AdaptFlag == 1) {
        ucAdaptLength = pbPacket[4];
        dwStart++;
        dwStart += ucAdaptLength;
    }

    dword_t dwRet = NO_ERR;
    //ペイロード部分あり？
    if (ph.PayloadFlag == 1) {
        if (ph.PayloadStartFlag == 1) {
//          ucPayloadOffset = pbPacket[dwStart];
//          dwStart++;
        } else {
            if (m_PayloadList.size() == 0) {
                //リストに貯まっていないのに開始フラグのついたパケットじゃない
                Clear();
                return ERR_NOT_FIRST;
            }
        }
        if (ucPayloadOffset != 0 && m_PayloadList.size()>0) {
            //オフセットあるということは前のデータあり
            if (dwStart + ucPayloadOffset > 188)
                //サイズ188以上とかありえない
                return ERR_INVALID_PACKET;

            PAYLOAD_DATA *stData = new PAYLOAD_DATA;
            stData->wSize = ucPayloadOffset;
            memcpy(stData->bBuff, pbPacket + dwStart, stData->wSize);
            m_PayloadList.push_back(stData);
            //全部貯まったはずなので解析作業に入る

            dwRet = ParseListData();
            for (int i = 0; i < (int)m_PayloadList.size(); i++)
                SAFE_DELETE(m_PayloadList[i]);
            m_PayloadList.clear();

            if (dwRet == NO_ERR) {
                m_bAnalyz = true;
                bBeforeParse = true;
            }
        }
        dwStart += ucPayloadOffset;

        if ((m_bAnalyz == false || dwStart > 188) && (ph.PayloadStartFlag == 1)) {
            //解析してないのに次の開始パケットがきた
            //パケット飛んでる可能性あるのでエラー
            Clear();
            return ERR_INVALID_PACKET;
        }
        if (dwStart >= 188) {
            //アダプテーションか何かでペイロードなくなった？
            Clear();
            return dwRet;
        }
        m_bAnalyz = false;

        //とりあえずデータをリストに突っ込む
        PAYLOAD_DATA *stData = new PAYLOAD_DATA;
        stData->wSize = (word_t)(188 - dwStart);
        memcpy(stData->bBuff, pbPacket + dwStart, stData->wSize);
        m_PayloadList.push_back(stData);

        bool_t bNext = true;
        if (ph.PayloadStartFlag == 1) {
            //スタートフラグたってるのでこれだけでデータたまってるかチェック
            m_dwNowReadSize = 0;
            m_dwNeedSize = 0;
            while (m_dwNowReadSize + 3 < stData->wSize) {
                if (stData->bBuff[0] != 0x00
                 || stData->bBuff[1] != 0x00
                 || stData->bBuff[2] != 0x01) {
                    //PESじゃないのでエラー
                    Clear();
                    return ERR_INVALID_PACKET;
                }

                dword_t dwSecSize = 0;
                dwSecSize += (((dword_t)(stData->bBuff[4])) << 8 | stData->bBuff[5]) + 6;
                m_dwNeedSize += dwSecSize;
                if (stData->wSize < m_dwNeedSize)
                    m_dwNowReadSize = stData->wSize;
                else
                    m_dwNowReadSize += dwSecSize;

                //まだ続きあり？
                if (m_dwNeedSize<stData->wSize)
                    if (stData->bBuff[m_dwNeedSize] == 0xFF) {
                        //後はNULLデータ
                        bNext = false;
                        break;
                    }
            }
        } else
            m_dwNowReadSize += stData->wSize;

        if (m_dwNeedSize <= m_dwNowReadSize || bNext == false) {
            //全部貯まったので解析作業に入る
            dwRet = ParseListData();
            for (int i = 0; i < (int)m_PayloadList.size(); i++)
                SAFE_DELETE(m_PayloadList[i]);
            m_PayloadList.clear();
        } else {

            if (bBeforeParse == (bool_t)true) {
                dwRet = true;
                goto CREATE_DATA;
            }

            return ERR_NEED_NEXT_PACKET;
        }
    }

    if (dwRet == NO_ERR || dwRet >= 20)
        m_bAnalyz = true;

CREATE_DATA:

    return dwRet;
}

dword_t CCaptionMain::ParseListData(void)
{
    //まずバッファを作る
    byte_t *pbBuff = NULL;
    dword_t dwBuffSize = 0;
    for (int i = 0; i < (int)m_PayloadList.size(); i++)
        dwBuffSize += m_PayloadList[i]->wSize;
    pbBuff = new byte_t[dwBuffSize];
    dword_t dwReadBuff = 0;
    for (int i = 0; i < (int)m_PayloadList.size(); i++) {
        memcpy(pbBuff + dwReadBuff, m_PayloadList[i]->bBuff, m_PayloadList[i]->wSize);
        dwReadBuff += m_PayloadList[i]->wSize;
    }

    word_t wPesSize = ((word_t)(pbBuff[4])) << 8 | pbBuff[5];
    word_t wHeadSize = pbBuff[8];
    word_t wDataSize = wPesSize - wHeadSize - 3;
    word_t wStartPos = 8 + wHeadSize + 1;

    dword_t dwRet = ParseCaption(pbBuff + wStartPos, wDataSize);

    delete[] pbBuff;

    return dwRet;
}

dword_t CCaptionMain::ParseCaption(byte_t *pbBuff, dword_t dwSize)
{
    if (!pbBuff || dwSize < 3)
        return ERR_INVALID_PACKET;

    if (pbBuff[0] != 0x80 && pbBuff[0] != 0x81)
        return ERR_INVALID_PACKET;

    if (pbBuff[1] != 0xFF)
        return ERR_INVALID_PACKET;

    unsigned char ucHeadSize = pbBuff[2] & 0x0F;

    dword_t dwStartPos = 3 + ucHeadSize;

    unsigned char ucDataGroupID = pbBuff[dwStartPos] >> 2;
    unsigned char ucVersion = pbBuff[dwStartPos] & 0x03;
    dwStartPos += 3;
    unsigned short usDataGroupSize = ((unsigned short)(pbBuff[dwStartPos])) << 8 | pbBuff[dwStartPos + 1];
    dwStartPos += 2;

    bool_t bChg = false;
    if (ucVersion != m_ucVersion) {
        m_LangTagList.clear();
        m_ucVersion = ucVersion;
        bChg = true;
    }

    dword_t dwRet = NO_ERR;
    if (ucDataGroupID == 0x00 || ucDataGroupID == 0x20) {
        //字幕管理
        vector<CAPTION_DATA> CaptionList;
        dwRet = ParseCaptionManagementData(pbBuff + dwStartPos, usDataGroupSize - 2, &CaptionList);
        if (CaptionList.size() > 0)
            InsertCaptionList(ucDataGroupID,&CaptionList);
        if (dwRet == NO_ERR)
            dwRet = NO_ERR_TAG_INFO;
    } else {
        //字幕データ
        vector<CAPTION_DATA> CaptionList;
        dwRet = ParseCaptionData(pbBuff + dwStartPos, usDataGroupSize - 2, &CaptionList);
        if (CaptionList.size() > 0)
            InsertCaptionList(ucDataGroupID,&CaptionList);
        if (dwRet == NO_ERR)
            dwRet = NO_ERR_CAPTION;
    }
    return (bChg == (bool_t)true) ? CHANGE_VERSION : dwRet;
}


dword_t CCaptionMain::ParseCaptionManagementData(byte_t *pbBuff, dword_t dwSize, vector<CAPTION_DATA> *pCaptionList)
{
    if (!pbBuff)
        return ERR_INVALID_PACKET;

    dword_t dwRet = NO_ERR;
    dword_t dwPos = 0;
    unsigned char ucTMD = pbBuff[dwPos] >> 6;
    dwPos++;
    unsigned char ucOTMHH = 0;
    unsigned char ucOTMMM = 0;
    unsigned char ucOTMSS = 0;
    unsigned char ucOTMSSS = 0;
    if (ucTMD == 0x02) {
        //OTM
        ucOTMHH = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMMM = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMSS = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMSSS = (pbBuff[dwPos] & 0xF0 >> 4) * 100 + (pbBuff[dwPos] & 0x0F) * 10 + (pbBuff[dwPos + 1] & 0xF0 >> 4);
        dwPos += 2;
    }
    unsigned char ucLangNum = pbBuff[dwPos];
    dwPos++;

    for (unsigned char i = 0; i < ucLangNum; i++) {
        LANG_TAG_INFO Item;
        Item.ucLangTag = pbBuff[dwPos] >> 5;
        Item.ucDMF = pbBuff[dwPos] & 0x0F;
        dwPos++;
        if (Item.ucDMF == 0x0C || Item.ucDMF == 0x0D || Item.ucDMF == 0x0E) {
            Item.ucDC = pbBuff[dwPos];
            dwPos++;
        }
        Item.szISOLangCode[0] = pbBuff[dwPos];
        Item.szISOLangCode[1] = pbBuff[dwPos + 1];
        Item.szISOLangCode[2] = pbBuff[dwPos + 2];
        Item.szISOLangCode[3] = 0x00;
        dwPos += 3;
        Item.ucFormat = pbBuff[dwPos] >> 4;
        Item.ucTCS = (pbBuff[dwPos] & 0x0C) >> 2;
        Item.ucRollupMode = pbBuff[dwPos] & 0x03;
        dwPos++;

        m_LangTagList.insert(pair<word_t, LANG_TAG_INFO>((word_t)Item.ucLangTag, Item));
    }
    uint_t uiUnitSize = ((uint_t)(pbBuff[dwPos])) << 16 | ((uint_t)(pbBuff[dwPos + 1])) << 8 | pbBuff[dwPos + 2];
    dwPos += 3;
    if (uiUnitSize > 0) {
        //字幕データ
        dword_t dwReadSize = 0;
        while (dwReadSize < uiUnitSize) {
            dword_t dwSize = 0;
            dwRet = ParseUnitData(pbBuff + dwPos + dwReadSize, uiUnitSize - dwReadSize, &dwSize, pCaptionList);
            dwReadSize += dwSize;
        }
    }
    return dwRet;
}

dword_t CCaptionMain::ParseCaptionData(byte_t *pbBuff, dword_t dwSize, vector<CAPTION_DATA> *pCaptionList)
{
    if (!pbBuff)
        return ERR_INVALID_PACKET;

    dword_t dwRet = NO_ERR;
    dword_t dwPos = 0;
    unsigned char ucTMD = pbBuff[dwPos] >> 6;
    dwPos++;

    unsigned char ucOTMHH = 0;
    unsigned char ucOTMMM = 0;
    unsigned char ucOTMSS = 0;
    unsigned char ucOTMSSS = 0;

    if (ucTMD == 0x01 || ucTMD == 0x02) {
        //STM
        ucOTMHH = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMMM = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMSS = (pbBuff[dwPos] & 0xF0 >> 4) * 10 + (pbBuff[dwPos] & 0x0F);
        dwPos++;
        ucOTMSSS = (pbBuff[dwPos] & 0xF0 >> 4) * 100 + (pbBuff[dwPos] & 0x0F) * 10 + (pbBuff[dwPos + 1] & 0xF0 >> 4);
        dwPos += 2;
    }
    uint_t uiUnitSize = ((uint_t)(pbBuff[dwPos])) << 16 | ((uint_t)(pbBuff[dwPos + 1])) << 8 | pbBuff[dwPos + 2];
    dwPos += 3;
    if (uiUnitSize > 0) {
        //字幕データ
        dword_t dwReadSize = 0;
        while (dwReadSize < uiUnitSize) {
            dword_t dwSize = 0;
            dwRet = ParseUnitData(pbBuff + dwPos + dwReadSize, uiUnitSize - dwReadSize, &dwSize, pCaptionList);
            dwReadSize += dwSize;
        }
    }
    return dwRet;
}

dword_t CCaptionMain::ParseUnitData(byte_t *pbBuff, dword_t dwSize, dword_t *pdwReadSize, vector<CAPTION_DATA> *pCaptionList)
{
    if (!pbBuff || dwSize < 5 || !pdwReadSize)
        return ERR_FALSE;

    if (pbBuff[0] != 0x1F)
        return ERR_FALSE;

    uint_t uiUnitSize = ((uint_t)(pbBuff[2])) << 16 | ((uint_t)(pbBuff[3])) << 8 | pbBuff[4];
    if (dwSize < 5 + uiUnitSize)
        return ERR_FALSE;

    if (pbBuff[1] != 0x20) {
        //字幕文(本文)以外
        if (pbBuff[1] == 0x30 || pbBuff[1] == 0x31)
            //DRCS処理
            if (uiUnitSize > 0) {
                CARIB8CharDecode cDec;
                cDec.DRCSHeaderparse(pbBuff + 5, uiUnitSize, (pbBuff[1] == 0x31) ? true : false);
            }
        *pdwReadSize = uiUnitSize + 5;
        return NO_ERR;
    }

    if (uiUnitSize > 0) {
        CARIB8CharDecode cDec;
        cDec.Caption(pbBuff + 5, uiUnitSize, pCaptionList);
    }
    *pdwReadSize = 5 + uiUnitSize;

    return NO_ERR;
}

bool_t CCaptionMain::InsertCaptionList(word_t wGroupID, vector<CAPTION_DATA> *pCaptionList)
{
    word_t wID = wGroupID & 0x0F;
    map<word_t, CAPTION_LIST>::iterator itr;
    itr  = m_CaptionMap.find(wID);
    if (itr != m_CaptionMap.end())
        m_CaptionMap.erase(itr);
    CAPTION_LIST Item;
    Item.wGroupID = wID;
    Item.CaptionList = *pCaptionList;
    m_CaptionMap.insert(pair<word_t, CAPTION_LIST>(wID, Item));
    return true;
}

dword_t CCaptionMain::GetTagInfo(vector<CCaptionMain::LANG_TAG_INFO> *pList)
{
    map<word_t, LANG_TAG_INFO>::iterator itr;
    for (itr = m_LangTagList.begin(); itr != m_LangTagList.end(); itr++)
        pList->push_back(itr->second);
    return true;
}

dword_t CCaptionMain::GetCaptionData(unsigned char ucLangTag, vector<CAPTION_DATA> *pList)
{
    map<word_t, CAPTION_LIST>::iterator itr;
    itr = m_CaptionMap.find(ucLangTag + 1);
    if (itr != m_CaptionMap.end())
        *pList = itr->second.CaptionList;
    return true;
}

dword_t CCaptionMain::GetTagInfo(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount)
{
    if (m_pLangList) {
        SAFE_DELETE_ARRAY(m_pLangList);
        m_dwLangListCount = 0;
    }
    if (!ppList || !pdwListCount)
        return false;

    vector<CCaptionMain::LANG_TAG_INFO> List;
    dword_t dwRet = GetTagInfo(&List);
    if (List.size() > 0) {
        m_dwLangListCount = (dword_t)List.size();
        m_pLangList = new LANG_TAG_INFO_DLL[m_dwLangListCount];
        for (int i = 0; i < (int)List.size(); i++) {
            m_pLangList[i].ucLangTag=List[i].ucLangTag;
            m_pLangList[i].ucDMF = List[i].ucDMF;
            m_pLangList[i].ucDC = List[i].ucDC;
            memcpy(m_pLangList[i].szISOLangCode, List[i].szISOLangCode, 4);
            m_pLangList[i].ucFormat = List[i].ucFormat;
            m_pLangList[i].ucTCS = List[i].ucTCS;
            m_pLangList[i].ucRollupMode = List[i].ucRollupMode;
        }
        *pdwListCount = m_dwLangListCount;
        *ppList = m_pLangList;

        return dwRet;
    }
    return false;
}

dword_t CCaptionMain::GetCaptionData(unsigned char ucLangTag, CAPTION_DATA_DLL **ppList, dword_t *pdwListCount)
{
    if (m_pCapList) {
        for (dword_t i = 0; i < m_dwCapListCount; i++) {
            for (dword_t j = 0; j < m_pCapList[i].dwListCount; j++)
                SAFE_DELETE(m_pCapList[i].pstCharList[j].pszDecode);
            SAFE_DELETE_ARRAY(m_pCapList[i].pstCharList);
        }
        SAFE_DELETE_ARRAY(m_pCapList);
        m_dwCapListCount = 0;
    }
    if (!ppList || !pdwListCount)
        return false;

    vector<CAPTION_DATA> List;
    dword_t dwRet = GetCaptionData(ucLangTag, &List);
    if (List.size() > 0) {
        m_dwCapListCount = (dword_t)List.size();
        m_pCapList = new CAPTION_DATA_DLL[m_dwCapListCount];
        for (int i = 0; i < (int)List.size(); i++) {
            m_pCapList[i].dwListCount = (dword_t)List[i].CharList.size();
            m_pCapList[i].pstCharList = new CAPTION_CHAR_DATA_DLL[m_pCapList[i].dwListCount];
            for (int j = 0; j < (int)List[i].CharList.size(); j++) {
                if (List[i].CharList[j].strDecode.length() > 0) {
                    m_pCapList[i].pstCharList[j].pszDecode = new char[List[i].CharList[j].strDecode.length() + 1];
                    ZeroMemory(m_pCapList[i].pstCharList[j].pszDecode, List[i].CharList[j].strDecode.length() + 1);
                    strcpy_s(m_pCapList[i].pstCharList[j].pszDecode, List[i].CharList[j].strDecode.length() + 1, List[i].CharList[j].strDecode.c_str());
                } else
                    m_pCapList[i].pstCharList[j].pszDecode = NULL;

                m_pCapList[i].pstCharList[j].wCharSizeMode = (dword_t)List[i].CharList[j].emCharSizeMode;
                m_pCapList[i].pstCharList[j].stCharColor.ucAlpha = List[i].CharList[j].stCharColor.ucAlpha;
                m_pCapList[i].pstCharList[j].stCharColor.ucB = List[i].CharList[j].stCharColor.ucB;
                m_pCapList[i].pstCharList[j].stCharColor.ucG = List[i].CharList[j].stCharColor.ucG;
                m_pCapList[i].pstCharList[j].stCharColor.ucR = List[i].CharList[j].stCharColor.ucR;
                m_pCapList[i].pstCharList[j].stBackColor.ucAlpha = List[i].CharList[j].stBackColor.ucAlpha;
                m_pCapList[i].pstCharList[j].stBackColor.ucB = List[i].CharList[j].stBackColor.ucB;
                m_pCapList[i].pstCharList[j].stBackColor.ucG = List[i].CharList[j].stBackColor.ucG;
                m_pCapList[i].pstCharList[j].stBackColor.ucR = List[i].CharList[j].stBackColor.ucR;
                m_pCapList[i].pstCharList[j].stRasterColor.ucAlpha = List[i].CharList[j].stRasterColor.ucAlpha;
                m_pCapList[i].pstCharList[j].stRasterColor.ucB = List[i].CharList[j].stRasterColor.ucB;
                m_pCapList[i].pstCharList[j].stRasterColor.ucG = List[i].CharList[j].stRasterColor.ucG;
                m_pCapList[i].pstCharList[j].stRasterColor.ucR = List[i].CharList[j].stRasterColor.ucR;
                m_pCapList[i].pstCharList[j].bUnderLine = List[i].CharList[j].bUnderLine;
                m_pCapList[i].pstCharList[j].bShadow = List[i].CharList[j].bShadow;
                m_pCapList[i].pstCharList[j].bBold = List[i].CharList[j].bBold;
                m_pCapList[i].pstCharList[j].bItalic = List[i].CharList[j].bItalic;
                m_pCapList[i].pstCharList[j].bFlushMode = List[i].CharList[j].bFlushMode;
                m_pCapList[i].pstCharList[j].bHLC = List[i].CharList[j].bHLC;
                m_pCapList[i].pstCharList[j].wCharW = List[i].CharList[j].wCharW;
                m_pCapList[i].pstCharList[j].wCharH = List[i].CharList[j].wCharH;
                m_pCapList[i].pstCharList[j].wCharHInterval = List[i].CharList[j].wCharHInterval;
                m_pCapList[i].pstCharList[j].wCharVInterval = List[i].CharList[j].wCharVInterval;
            }

            m_pCapList[i].bClear = List[i].bClear;
            m_pCapList[i].wSWFMode = List[i].wSWFMode;
            m_pCapList[i].wClientX = List[i].wClientX;
            m_pCapList[i].wClientY = List[i].wClientY;
            m_pCapList[i].wClientW = List[i].wClientW;
            m_pCapList[i].wClientH = List[i].wClientH;
            m_pCapList[i].wPosX = List[i].wPosX;
            m_pCapList[i].wPosY = List[i].wPosY;
            m_pCapList[i].dwWaitTime = List[i].dwWaitTime;
        }
        *pdwListCount = m_dwCapListCount;
        *ppList = m_pCapList;

        return dwRet;
    }
    return false;
}
