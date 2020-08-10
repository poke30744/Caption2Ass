//------------------------------------------------------------------------------
// Caption.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//------------------------------------------------------------------------------
#pragma warning(disable: 4100)

#include "CaptionDef.h"
#include "ColorDef.h"
#include "ARIB8CharDecode.h"
#include "CaptionMain.h"
#include "Caption.h"

static CCaptionMain *g_sys = NULL;

static __inline dword_t initialize(bool_t bUNICODE)
{
    if (g_sys || (g_sys = new CCaptionMain(bUNICODE)) == NULL)
        return ERR_INIT;
    return NO_ERR;
}

static __inline void uninitialize(void)
{
    if (!g_sys)
        return;
    delete g_sys;
    g_sys = NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_sys = NULL;
        break;
    case DLL_PROCESS_DETACH:
        uninitialize();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }
    return true;
}

//DLL�̏�����
//�߂�l�F�G���[�R�[�h
dword_t WINAPI InitializeCP(void)
{
    return initialize(false);
}

//DLL�̏����� UNICODE�Ή�
//�߂�l�F�G���[�R�[�h
dword_t WINAPI InitializeUNICODE(void)
{
    return initialize(true);
}

//DLL�̊J��
//�߂�l�F�G���[�R�[�h
dword_t WINAPI UnInitializeCP(void)
{
    uninitialize();
    return NO_ERR;
}

dword_t WINAPI AddTSPacketCP(byte_t*pbPacket)
{
    if (!g_sys)
        return ERR_NOT_INIT;
    return g_sys->AddTSPacket(pbPacket);
}

dword_t WINAPI ClearCP(void)
{
    if (!g_sys)
        return ERR_NOT_INIT;
    return g_sys->Clear();
}

dword_t WINAPI GetTagInfoCP(LANG_TAG_INFO_DLL **ppList, dword_t *pdwListCount)
{
    if (!g_sys)
        return ERR_NOT_INIT;
    return g_sys->GetTagInfo(ppList, pdwListCount);
}

dword_t WINAPI GetCaptionDataCP(unsigned char ucLangTag, CAPTION_DATA_DLL **ppList, dword_t *pdwListCount)
{
    if (!g_sys)
        return ERR_NOT_INIT;
    return g_sys->GetCaptionData(ucLangTag, ppList, pdwListCount);
}
