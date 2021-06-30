// libcachewriter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibVideoReader.h"


VideoReader::VideoReader()
{

}

VideoReader::~VideoReader()
{

}

Mp4Reader::Mp4Reader()
{
}

Mp4Reader::~Mp4Reader()
{

}

bool Mp4Reader::openFile(const char* name)
{
	return true;
}

bool Mp4Reader::close()
{
	return true;
}

bool Mp4Reader::readFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength)
{
	return true;
}

// 这是导出函数的一个示例。
LIBVIDEO_API VideoReader* createVideoReader(VideoType_e type)
{
	VideoReader* rader = nullptr;
	switch (type)
	{
	case KAV_FILE_MP4_WRITER:
	default:
		break;
	}
	return rader;
}
