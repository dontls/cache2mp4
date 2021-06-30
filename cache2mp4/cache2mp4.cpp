// cache2mp4.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "LibVideo.h"
#include <iostream>

void usage()
{
	std::cerr << "Usage\n";
	std::cerr << "\t[cache file]\n";
	std::cerr << "\t[output file]\n";
}

const char* CACHE_VERSION = "cache2mp4.0.0.3";

// ��ͷ 8
// ����С 4
// ֡ͷ 12 
// ֡����

#pragma pack(4)

typedef struct {
	char flag[8];
	int  size;
}PkgHeader;


typedef struct
{
	unsigned short channel;
	unsigned short type;
	long long timestamp;
}FrameHeader;

#pragma pack()

static int _frameMaxLength = 1024 * 1024;
const  int _pkgHeaderLength = sizeof(PkgHeader);
const  int _frameHeaderLength = sizeof(FrameHeader);


int main(int argc, char** argv)
{
	if (argc < 2) {
		usage();
		return 0;
	}
	//const char* cache = "F:\\workspace\\cache2mp4\\Debug\\x64\\CH01_20210629_104139_104339.cache";
	//const char* mp4name = "F:\\workspace\\cache2mp4\\Debug\\x64\\CH01_20210629_104139_104339.mp4";
	const char* cache = argv[1];
	const char* mp4name = argv[2];
	// �жϸ�ʽͷ
	FILE *fp = ::fopen(cache, "rb");
	if (NULL == fp)
		return -1;

	char cacheVesion[21] = {0};
	::fread(cacheVesion, 1, 20, fp);
	printf("cache version %s\n", cacheVesion);
	if (0 != strcmp(CACHE_VERSION, cacheVesion)) {
		printf("cache file format error\n");
		return -1;
	}
	VideoWriter* writer = createVideoWriter(KAV_FILE_MP4_WRITER);
	if(!writer->setFile(mp4name)) {
		printf("create cache writer error\n");
		return -1;
	}
	char* frame = new char[_frameMaxLength];
	long long lastvideoframestamp = 0;
	long long lastaudioframestamp = 0;
	for (;;)
	{
		PkgHeader pkg = {0};
		int size = (int)::fread(&pkg, 1, _pkgHeaderLength, fp);
		if (size != _pkgHeaderLength)
			break;

		if (pkg.size > _frameMaxLength) {
			_frameMaxLength = pkg.size;
			delete frame;
			frame == nullptr;
			frame = new char[_frameMaxLength];
		}
		size = (int)::fread(frame, 1, pkg.size, fp);
		if (size != pkg.size) 
			break;

		// ��������
		FrameHeader* header = (FrameHeader*)frame;
		char* frameBuf = frame + _frameHeaderLength;
		int frameLength = pkg.size - _frameHeaderLength;
		//printf("channel %d type %d timestamp %lld frameLength %d \n", header->channel, header->type, header->timestamp, frameLength);
		if(!writer->writeFrame(header->type, header->channel, header->timestamp, frameBuf, frameLength)) 
			break;
	}
	writer->close();
	::fclose(fp);
	printf("%s ok\n", mp4name);
	// �ְ���ȡ
    return 0;
}

