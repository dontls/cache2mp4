
#include "stdafx.h"
#include "AVg726ToAac.h"

const int KAmpBufSize = 2048;

AVg726ToAac::AVg726ToAac(/* args */) : _avg726(new AVg726), _avfaac(new AVfaac)
{
    _pAmpBuffer = NULL;
}
AVg726ToAac::~AVg726ToAac() {}

void AVg726ToAac::init(int nOfmt)
{
    _avg726->init(40000, 2);
    _avfaac->init(8000, 1, 16, nOfmt);
    _pAmpBuffer = new char[KAmpBufSize * 2];
    _nAmpOffset = 0;
}

void AVg726ToAac::uinit()
{
    _avg726->uninit();
    if (_avg726) {
        delete _avg726;
        _avg726 = nullptr;
    }
    _avfaac->uninit();

    if (_avfaac) {
        delete _avfaac;
        _avfaac = nullptr;
    }
    if (_pAmpBuffer) {
        delete[] _pAmpBuffer;
        _pAmpBuffer = NULL;
    }
}

unsigned char* AVg726ToAac::aacSpecialData(int* len)
{
    return _avfaac->specialData(len);
}

unsigned char* AVg726ToAac::toAacEncodec(char* data, int len, int& aacLen)
{
    aacLen = 0;
    int   reLen = KAmpBufSize - _nAmpOffset;
    char* offsetAmpBuf = _pAmpBuffer + _nAmpOffset;
    int   pcmLen = _avg726->decodec2(data, len, offsetAmpBuf);
    if (pcmLen != 640) {
        return NULL;
    }

    // data not enought
    _nAmpOffset += pcmLen;
    if (_nAmpOffset < KAmpBufSize) {
        return NULL;
    }

        
    unsigned char* aac = _avfaac->encode(( unsigned char* )_pAmpBuffer, KAmpBufSize, aacLen);
    if (aacLen == 0) {
        return NULL;
    }
        
    int freeAmpLen = _nAmpOffset - KAmpBufSize;  
    if (freeAmpLen > 0) {
        ::memmove(_pAmpBuffer, _pAmpBuffer + KAmpBufSize, freeAmpLen);
    }
    _nAmpOffset = freeAmpLen;
    return aac;
}