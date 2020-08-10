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

//DLL�̏�����
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI InitializeCP(void);

//DLL�̏����� UNICODE�Ή�
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI InitializeUNICODE(void);

//DLL�̊J��
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI UnInitializeCP(void);

//188�o�C�gTS1�p�P�b�g
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI AddTSPacketCP(byte_t*pbPacket);

//�����f�[�^�N���A
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI ClearCP(void);

//�������擾
//�߂�l�F�G���[�R�[�h
__declspec(dllexport)
dword_t WINAPI GetTagInfoCP(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount);

//�����f�[�^�{���擾
//�߂�l�F�G���[�R�[�h
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
