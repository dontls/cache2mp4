#ifndef __AV_G726_H__
#define __AV_G726_H__

#pragma once

#if defined(_WINDOWS) || defined(_WIN32) || defined(WIN32)
#define DllExport __declspec(dllexport)
#else
#define DllExport extern "C"
#endif

class DllExport AVg726 {
private:
    void* _stat;
    int   _nDecLen = 0;

public:
	AVg726(/* args */);
	~AVg726();

    //  16000 24000 32000 40000
    //  G726_PACKING_NONE
	void init(int bitRate, int packing);

	void uninit();

    // HIG726 转AMP,
    // 返回aac数据长度，0 失败
	int decodec(char* pSrcData, int nDataLen, char* pAmpData);

	int decodec2(char* pInBuf, int nInLen, char* pOutBuf);
    // AMP编码，并添加海思头信息
	int encodec(char* pAmpData, int nDataLen, char* pG726Data);
};

#endif