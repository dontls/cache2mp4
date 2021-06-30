#ifndef __AV_MP4_WRITER_H__
#define __AV_MP4_WRITER_H__

extern "C" {
#include <libavformat/avformat.h>
}

#include "../aac/AVg726ToAac.h"
#include "Sps.h"
#include <string>

class AVmp4Writer {
private:
	AVFormatContext* _pOfmtCtx;
	std::string      _pfpName;
	int              _nWidth, _nHeight, _nfps;

	uint32_t _nVideoStreamIndex;
	uint32_t _nAudioStreamIndex;

	// 扩展信息
	std::string _vExtraData;  // 视频

	long long _startTimestamp;
	long long _lastVtimestamp;
	long long _lastAtimestamp;
	bool               _isWaitKeyframe;
	enum AVCodecID     _nframeType;

	AVg726ToAac _g726ToAac;

public:
	AVmp4Writer(/* args */);
	~AVmp4Writer();

	void fpname(const char* fp);

	bool writeVideoframe(char* frame, int len, bool iskey, unsigned long long pts);

	bool writeAudioframe(char* frame, int len, unsigned long long pts);

	void close();
private:
	bool initfp();

	// H264sps信息解析
	void doAvcExtraData(NaluUint_t* nalu);

	// H265sps信息解析
	void doHevcExtraData(NaluUint_t* nalu);

	bool doParseExtraData(char* frame, int length);

	char* getSliceData(char* frame, int length, int& sliceLength);

	void newAVStream();
};

#endif
