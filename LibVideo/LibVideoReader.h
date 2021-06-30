#pragma once

#include "LibVideo.h"

class Mp4Reader : public VideoReader
{
public:
	Mp4Reader();
	virtual ~Mp4Reader();


	virtual bool openFile(const char* name) = 0;

	virtual bool close() = 0;

	virtual bool readFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength) = 0;

private:
};
