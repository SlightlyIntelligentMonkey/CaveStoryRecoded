#pragma once
#include "common.h"

const char* musicList[];

extern HMODULE orgModule;

typedef int(__stdcall *pstartOrganya)(HWND);
typedef int(__stdcall *ploadOrganya)(const char *);
typedef int(__stdcall *psetOrganyaPosition)(int);
typedef int(__stdcall *pgetOrganyaPosition)(void);
typedef int(__stdcall *pplayOrganyaMusic)(void);
typedef int(__stdcall *pchangeOrganyaVolume)(int);
typedef int(__stdcall *pstopOrganyaMusic)(void);
typedef int(__stdcall *pendOrganya)(void);

extern pstartOrganya startOrganya;
extern ploadOrganya loadOrganya;
extern psetOrganyaPosition setOrganyaPosition;
extern pgetOrganyaPosition getOrganyaPosition;
extern pplayOrganyaMusic playOrganyaMusic;
extern pchangeOrganyaVolume changeOrganyaVolume;
extern pstopOrganyaMusic stopOrganyaMusic;
extern pendOrganya endOrganya;

void playOrg(int id);