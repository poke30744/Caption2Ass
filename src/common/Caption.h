//------------------------------------------------------------------------------
// Caption.h
//------------------------------------------------------------------------------
#ifndef __CAPTION_H__
#define __CAPTION_H__

#include <Windows.h>

#include "CommTypes.h"
#include "CaptionDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CAPTION_EXPORTS

//DLLの初期化
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI InitializeCP(void);

//DLLの初期化 UNICODE対応
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI InitializeUNICODE(void);

//DLLの開放
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI UnInitializeCP(void);

//188バイトTS1パケット
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI AddTSPacketCP(byte_t*pbPacket);

//内部データクリア
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI ClearCP(void);

//字幕情報取得
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI GetTagInfoCP(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount);

//字幕データ本文取得
//戻り値：エラーコード
__declspec(dllexport)
dword_t WINAPI GetCaptionDataCP(unsigned char ucLangTag, CAPTION_DATA_DLL **ppList, dword_t *pdwListCount);

#else /* CAPTION_EXPORTS */

typedef dword_t (WINAPI *InitializeCP)(void);

typedef dword_t (WINAPI *InitializeUNICODECP)(void);

typedef dword_t (WINAPI *UnInitializeCP)(void);

typedef dword_t (WINAPI *AddTSPacketCP)(byte_t*pbPacket);

typedef dword_t (WINAPI *ClearCP)(void);

typedef dword_t (WINAPI *GetTagInfoCP)(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount);

typedef dword_t (WINAPI *GetCaptionDataCP)(unsigned char ucLangTag, CAPTION_DATA_DLL **ppList, dword_t *pdwListCount);

#endif /* CAPTION_EXPORTS */

#ifdef __cplusplus
}
#endif

#endif // __CAPTION_H__
