#pragma once

#include "LibVideo.h"
#include "ffmpeg/AVmp4Writer.h"

class Mp4Writer : public VideoWriter
{
public:
	Mp4Writer();
	virtual ~Mp4Writer();

	virtual bool setFile(const char* name);

	virtual bool close();

	virtual bool writeFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength);

private:
	AVmp4Writer* _writer;
};
