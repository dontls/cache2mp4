#ifndef __AVFAAC_H__
#define __AVFAAC_H__

#include "libfaac/faac.h"
#include <string.h>

enum {
	AAC_STREAM_RAW = 0,
	AAC_STREAM_ADTS = 1,
};

class AVfaac {
private:
	faacEncHandle  _faacEnc;
	unsigned long  _maxOutputBytes;
	unsigned long  _inputSamples;
	int            _nBitPerSample;
	int            _nSpecialLen;
	char           _nDuration;
	unsigned char* _pOutputBytes;
	unsigned char  _cSpecialData[8];

public:
	AVfaac(/* args */);
	~AVfaac();

	// 8000 1 16
	bool init(int nSamplePerSec, int nChn, int nBitsPerSample, int nOfmt);
	void uninit();

	// 编码 返回编码后数据长度
	unsigned char* encode(unsigned char* pcmData, long dataLen, int& accLen);

	// 播放时间周期
	int duration();

	unsigned char* specialData(int* len);
};

#endif