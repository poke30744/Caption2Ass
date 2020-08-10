//------------------------------------------------------------------------------
// ARIB8CharDecode.h
//------------------------------------------------------------------------------
#ifndef __ARIB8CHAR_DECODE_H__
#define __ARIB8CHAR_DECODE_H__

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <Windows.h>

#include "CommTypes.h"

extern map<string, string> dicHash_Char_map;
extern map<word_t, string> dicCharcode_Char_map;

//文字符号集合
//Gセット
#define MF_JIS_KANJI1   0x39    //JIS互換漢字1面
#define MF_JIS_KANJI2   0x3A    //JIS互換漢字2面
#define MF_KIGOU        0x3B    //追加記号
#define MF_ASCII        0x4A    //英数
#define MF_HIRA         0x30    //平仮名
#define MF_KANA         0x31    //片仮名
#define MF_KANJI        0x42    //漢字
#define MF_MOSAIC_A     0x32    //モザイクA
#define MF_MOSAIC_B     0x33    //モザイクB
#define MF_MOSAIC_C     0x34    //モザイクC
#define MF_MOSAIC_D     0x35    //モザイクD
#define MF_PROP_ASCII   0x36    //プロポーショナル英数
#define MF_PROP_HIRA    0x37    //プロポーショナル平仮名
#define MF_PROP_KANA    0x38    //プロポーショナル片仮名
#define MF_JISX_KANA    0x49    //JIX X0201片仮名
//DRCS
#define MF_DRCS_0   0x40    //DRCS-0
#define MF_DRCS_1   0x41    //DRCS-1
#define MF_DRCS_2   0x42    //DRCS-2
#define MF_DRCS_3   0x43    //DRCS-3
#define MF_DRCS_4   0x44    //DRCS-4
#define MF_DRCS_5   0x45    //DRCS-5
#define MF_DRCS_6   0x46    //DRCS-6
#define MF_DRCS_7   0x47    //DRCS-7
#define MF_DRCS_8   0x48    //DRCS-8
#define MF_DRCS_9   0x49    //DRCS-9
#define MF_DRCS_10  0x4A    //DRCS-10
#define MF_DRCS_11  0x4B    //DRCS-11
#define MF_DRCS_12  0x4C    //DRCS-12
#define MF_DRCS_13  0x4D    //DRCS-13
#define MF_DRCS_14  0x4E    //DRCS-14
#define MF_DRCS_15  0x4F    //DRCS-15
#define MF_MACRO    0x70    //マクロ

//符号集合の分類
#define MF_MODE_G       1   //Gセット
#define MF_MODE_DRCS    2   //DRCS
#define MF_MODE_OTHER   3   //その他

//文字サイズ
typedef enum{
    STR_SMALL = 0,  //SSZ
    STR_MEDIUM,     //MSZ
    STR_NORMAL,     //NSZ
    STR_MICRO,      //SZX 0x60
    STR_HIGH_W,     //SZX 0x41
    STR_WIDTH_W,    //SZX 0x44
    STR_W,          //SZX 0x45
    STR_SPECIAL_1,  //SZX 0x6B
    STR_SPECIAL_2,  //SZX 0x64
} STRING_SIZE;

typedef struct _CAPTION_CHAR_DATA {
    string strDecode;
    STRING_SIZE emCharSizeMode;

    CLUT_DAT stCharColor;
    CLUT_DAT stBackColor;
    CLUT_DAT stRasterColor;

    bool_t bUnderLine;
    bool_t bShadow;
    bool_t bBold;
    bool_t bItalic;
    byte_t bFlushMode;
    byte_t bHLC; //must ignore low 4bits

    word_t wCharW;
    word_t wCharH;
    word_t wCharHInterval;
    word_t wCharVInterval;
    //=オペレーターの処理
    _CAPTION_CHAR_DATA & operator= (const _CAPTION_CHAR_DATA & o) {
        strDecode = o.strDecode;
        emCharSizeMode = o.emCharSizeMode;
        stCharColor = o.stCharColor;
        stBackColor = o.stBackColor;
        stRasterColor = o.stRasterColor;
        bUnderLine = o.bUnderLine;
        bShadow = o.bShadow;
        bBold = o.bBold;
        bItalic = o.bItalic;
        bFlushMode = o.bFlushMode;
        bHLC = o.bHLC;
        wCharW = o.wCharH;
        wCharHInterval = o.wCharHInterval;
        wCharVInterval = o.wCharVInterval;
        return *this;
    };
} CAPTION_CHAR_DATA;

typedef struct _CAPTION_DATA {
    bool_t bClear;
    word_t wSWFMode;
    word_t wClientX;
    word_t wClientY;
    word_t wClientW;
    word_t wClientH;
    word_t wPosX;
    word_t wPosY;
    vector<CAPTION_CHAR_DATA> CharList;
    dword_t dwWaitTime;
    //=オペレーターの処理
    _CAPTION_DATA & operator= (const _CAPTION_DATA & o) {
        bClear = o.bClear;
        wSWFMode = o.wSWFMode;
        wClientX = o.wClientX;
        wClientY = o.wClientY;
        wClientW = o.wClientW;
        wClientH = o.wClientH;
        wPosX = o.wPosX;
        wPosY = o.wPosY;
        CharList = o.CharList;
        dwWaitTime = o.dwWaitTime;
        return *this;
    };
} CAPTION_DATA;
//DRCS図形の縦横最大サイズ(運用規定より)
#define DRCS_SIZE_MAX 36

struct DRCS_PATTERN {
    word_t wDRCCode;
    word_t wGradation;
    BITMAPINFOHEADER bmiHeader;
    byte_t bBitmap[(DRCS_SIZE_MAX * 4 + 31) / 32 * 4 * DRCS_SIZE_MAX];
    DRCS_PATTERN() {}
};

class CARIB8CharDecode
{
public:
    CARIB8CharDecode(void);
    ~CARIB8CharDecode(void);

    //字幕を想定したSJISへの変換
    bool_t Caption(const byte_t *pbSrc, dword_t dwSrcSize, vector<CAPTION_DATA> *pCaptionList);
    //dllのパスの取得
    string GetAppPath(void)
    {
        char wkPath[_MAX_PATH], wkDrive[_MAX_DRIVE], wkDir[_MAX_DIR], wkFileName[_MAX_FNAME], wkExt[_MAX_EXT];
        dword_t dwRet = GetModuleFileNameA(NULL, wkPath, sizeof(wkPath));
        if (dwRet == 0) {
            //エラー処理など(省略)
        }
        _splitpath_s(wkPath, wkDrive, _MAX_DRIVE, wkDir, _MAX_DIR, wkFileName, _MAX_FNAME, wkExt, _MAX_EXT);
        string strAppPath = "";
        strAppPath += wkDrive;
        strAppPath += wkDir;
        return strAppPath;
    }
    bool_t CARIB8CharDecode::DRCSHeaderparse(const byte_t *pbSrc, dword_t dwSrcSize, bool_t bDRCS_0);

protected:
    typedef struct _MF_MODE {
        int iMF; //文字符号集合
        int iMode; //符号集合の分類
        int iByte; //読み込みバイト数
        //=オペレーターの処理
        _MF_MODE & operator= (const _MF_MODE & o) {
            iMF = o.iMF;
            iMode = o.iMode;
            iByte = o.iByte;
            return *this;
        }
    } MF_MODE;

    bool_t m_bPSI;

    MF_MODE  m_G0;
    MF_MODE  m_G1;
    MF_MODE  m_G2;
    MF_MODE  m_G3;
    MF_MODE *m_GL;
    MF_MODE *m_GR;

    bool_t m_bModGL;

    //デコードした文字列
    string m_strDecode;
    //文字サイズ
    STRING_SIZE m_emStrSize;

    //CLUTのインデックス
    byte_t m_bCharColorIndex;
    byte_t m_bBackColorIndex;
    byte_t m_bRasterColorIndex;
    byte_t m_bDefPalette;

    bool_t m_bUnderLine;
    bool_t m_bShadow;
    bool_t m_bBold;
    bool_t m_bItalic;
    byte_t m_bFlushMode;
    byte_t m_bHLC;
    //map<string, string> dicHash_Char_map;
    //map<int, string> dicCharcode_Char_map;
    word_t m_wMaxPosX;
    word_t m_wTmpPosX;
    // RPC対応
    bool_t m_bRPC;
    word_t m_wRPC;
    // DRCSとARIBの代用文字用iniファィルのUnicode
//  bool_t m_bUnicode;
    bool_t m_bGaiji;

    //表示書式
    word_t m_wSWFMode;
    word_t m_wClientX;
    word_t m_wClientY;
    word_t m_wClientW;
    word_t m_wClientH;
    word_t m_wPosX;
    word_t m_wPosY;
    word_t m_wCharW;
    word_t m_wCharH;
    word_t m_wCharHInterval;
    word_t m_wCharVInterval;
    word_t m_wMaxChar;

    dword_t m_dwWaitTime;

    vector<CAPTION_DATA> *m_pCaptionList;
protected:
    void InitCaption(void);
    bool_t Analyze(const byte_t *pbSrc, dword_t dwSrcSize, dword_t *pdwReadSize);

    bool_t IsSmallCharMode(void);
    bool_t IsChgPos(void);
    void CreateCaptionData(CAPTION_DATA *pItem);
    void CreateCaptionCharData(CAPTION_CHAR_DATA *pItem);
    void CheckModify(void);

    //制御符号
    bool_t C0(const byte_t *pbSrc, dword_t *pdwReadSize);
    bool_t C1(const byte_t *pbSrc, dword_t *pdwReadSize);
    bool_t GL(const byte_t *pbSrc, dword_t *pdwReadSize);
    bool_t GR(const byte_t *pbSrc, dword_t *pdwReadSize);
    //シングルシフト
    bool_t SS2(const byte_t *pbSrc, dword_t *pdwReadSize);
    bool_t SS3(const byte_t *pbSrc, dword_t *pdwReadSize);
    //エスケープシーケンス
    bool_t ESC(const byte_t *pbSrc, dword_t *pdwReadSize);
    //２バイト文字変換
    bool_t ToSJIS(const byte_t bFirst, const byte_t bSecond);
    bool_t ToCustomFont(const byte_t bFirst, const byte_t bSecond);

    bool_t CSI(const byte_t *pbSrc, dword_t *pdwReadSize);

    bool_t AddToString(const char *cDec, bool_t m_bGaiji);

public:
    string Get_dicCharcode_Char(word_t DRCSCharCode)
    {
        string strRet = "NF";
        map<word_t, string>::iterator iter = dicCharcode_Char_map.find(DRCSCharCode);
        if (iter != dicCharcode_Char_map.end()) {
            strRet = iter->second;
        }
        return strRet;
    }
    void Add_dicCharcode_Char(word_t DRCSCharCode, string gaijichar)
    {
        dicCharcode_Char_map.insert(std::make_pair(DRCSCharCode, map<word_t, string>::mapped_type())).first->second = gaijichar;
    }
    string Get_dicHash_Char(string hash)
    {
        string strRet = "NF";
        map<string, string>::iterator iter = dicHash_Char_map.find(hash);
        if (iter != dicHash_Char_map.end()) {
            strRet = iter->second;
        }
        return strRet;
    }
    void Add_dicHash_Char(string hash, string gaijichar)
    {
        dicHash_Char_map.insert(map<string, string>::value_type(hash, gaijichar));
    }
};

#endif // __ARIB8CHAR_DECODE_H__
