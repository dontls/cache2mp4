// libcachewriter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibVideoWriter.h"


VideoWriter::VideoWriter()
{

}

VideoWriter::~VideoWriter()
{

}

Mp4Writer::Mp4Writer()
{
}

Mp4Writer::~Mp4Writer()
{
	if (_writer) {
		delete _writer;
		_writer = nullptr;
	}
}

bool Mp4Writer::setFile(const char* name)
{
	_writer = new AVmp4Writer;
	if (_writer == nullptr) {
		return false;
	}
	_writer->fpname(name);
	return true;
}

bool Mp4Writer::close()
{
	if (_writer == nullptr) {
		return false;
	}
	_writer->close();
	return true;
}

bool Mp4Writer::writeFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength)
{
	if (_writer == nullptr)
		return false;

	if (type == 1)
		_writer->writeVideoframe(frameBuf, frameLength, true, timestamp);
	else if (type == 2)
		_writer->writeVideoframe(frameBuf, frameLength, false, timestamp);
	else if (type == 3)
		_writer->writeAudioframe(frameBuf, frameLength, timestamp);
	return true;
}


// 这是导出函数的一个示例。
LIBVIDEO_API VideoWriter* createVideoWriter(VideoType_e type)
{
	VideoWriter* writer = nullptr;
	switch (type)
	{
	case KAV_FILE_MP4_WRITER:
		writer = new Mp4Writer();
	default:
		break;
	}
	return writer;
}
