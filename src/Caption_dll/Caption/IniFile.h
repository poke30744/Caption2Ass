//------------------------------------------------------------------------------
// IniFile.h
//------------------------------------------------------------------------------
#ifndef __INI_FILE_H__
#define __INI_FILE_H__

#include "CommTypes.h"

//ref class IniFile
class IniFile
{

public:
    bool_t IniFile::ReadIni(void);
    bool_t IniFile::ReadIniARIB(void);
    bool_t IniFile::ReadIniUNICODE(void);
    bool_t IniFile::ReadIniARIBUNICODE(void);

};

#endif // __INI_FILE_H__
