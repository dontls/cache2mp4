// libcachewriter.cpp : ���� DLL Ӧ�ó���ĵ���������
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

// ���ǵ���������һ��ʾ����
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
