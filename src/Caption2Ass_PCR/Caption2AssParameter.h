//------------------------------------------------------------------------------
// Caption2AssParameter.h
//------------------------------------------------------------------------------
#ifndef __CAPTION2ASS_PARAMETER_H__
#define __CAPTION2ASS_PARAMETER_H__

#include "CommTypes.h"

typedef struct {
    ushort_t      PMTPid;
    ushort_t      CaptionPid;
    ushort_t      PCRPid;
} pid_information_t;

typedef struct {
    dword_t       format;
    long        DelayTime;
    bool_t        keepInterval;
    byte_t        HLCmode;
    bool_t        srtornament;
    bool_t        norubi;
    bool_t        LogMode;
    dword_t       detectLength;
    TCHAR      *ass_type;
    TCHAR      *FileName;
    TCHAR      *TargetFileName;
    TCHAR      *LogFileName;
	bool_t		alignCenter;
} cli_parameter_t;

typedef struct {
    long        SWF0offset;
    long        SWF5offset;
    long        SWF7offset;
    long        SWF9offset;
    long        SWF11offset;
    char      *Comment1;
    char      *Comment2;
    char      *Comment3;
    long        PlayResX;
    long        PlayResY;
    char      *DefaultFontname;
    long        DefaultFontsize;
    char      *DefaultStyle;
    char      *BoxFontname;
    long        BoxFontsize;
    char      *BoxStyle;
    char      *RubiFontname;
    long        RubiFontsize;
    char      *RubiStyle;
	bool		aligncenter;
} ass_setting_t;

class CCaption2AssParameter
{
public:
    size_t              string_length;

protected:
    pid_information_t   pid_information;
    cli_parameter_t     cli_parameter;
    ass_setting_t       ass_setting;

protected:
    void Initialize(void);
    void Free(void);

public:
    CCaption2AssParameter(void);
    CCaption2AssParameter(size_t _string_length);
    ~CCaption2AssParameter(void);

    int Allocate(size_t string_length = 0);

    pid_information_t *get_pid_information(void){ return &pid_information; }
    cli_parameter_t *get_cli_parameter(void){ return &cli_parameter; }
    ass_setting_t *get_ass_setting(void){ return &ass_setting; }
};

#endif // __CAPTION2ASS_PARAMETER_H__
