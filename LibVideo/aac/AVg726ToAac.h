#ifndef __AV_G716TOAAC_H__
#define __AV_G716TOAAC_H__

#include "AVfaac.h"
#include "AVg726.h"

class AVg726ToAac {
private:
    AVg726* _avg726;
    AVfaac* _avfaac;
    char*   _pAmpBuffer;
    int     _nAmpOffset;

public:
	AVg726ToAac(/* args */);
	~AVg726ToAac();

	void init(int nOfmt = AAC_STREAM_RAW);

	void uinit();

	unsigned char* aacSpecialData(int* len);

	unsigned char* toAacEncodec(char* data, int len, int& aacLen);
};

#endif