#include "stdafx.h"
#include "AVg726.h"
#include "g7xx/g726.h"
#include <stdio.h>

const int KFlagHisiLen = 4;

AVg726::AVg726(/* args */) {
	_stat = NULL;
}
AVg726::~AVg726() {}

//  16000 24000 32000 40000
//  G726_PACKING_NONE
void AVg726::init(int bitRate, int packing)
{
	_stat = (void*)g726_init(NULL, bitRate, G726_ENCODING_LINEAR, packing);
	switch (bitRate) {
	case 16000:
		_nDecLen = 40;
		break;
	case 24000:
		_nDecLen = 60;
		break;
	case 32000:
		_nDecLen = 80;
		break;
	case 40000:
		_nDecLen = 100;
		break;
	default:
		break;
	}
}

void AVg726::uninit()
{
	if (_stat) {
		g726_release((g726_state_t*)_stat);
	}
}

// HIG726 תAMP,
// ����aac���ݳ��ȣ�0 ʧ��
int AVg726::decodec(char* pSrcData, int nDataLen, char* pAmpData)
{
	if (!_stat) {
		return 0;
	}
	// ȥ����˼�ĸ��ֽ�ͷ
	char* g726Data = pSrcData + KFlagHisiLen;
	int   ampLen = g726_decode((g726_state_t*)_stat, (int16_t*)pAmpData, (const uint8_t*)g726Data, nDataLen - KFlagHisiLen);
	return ampLen * sizeof(int16_t);
}

int AVg726::decodec2(char* pInBuf, int nInLen, char* pOutBuf)
{
	int nOffset = 0;
	int nAudioFrameLen = 0;
	int nWriteOffset = 0;
	while (nOffset < nInLen) {
		nAudioFrameLen = 2 * pInBuf[2 + nOffset] + 4;
		if ((nAudioFrameLen + nOffset) > nInLen) {
			break;
		}

		int nDecOffset = 4;
		while (nDecOffset < nAudioFrameLen) {
			g726_decode((g726_state_t*)_stat, (int16_t*)(pOutBuf + nWriteOffset),
				(const uint8_t*)(pInBuf + nOffset + nDecOffset), _nDecLen);
			nWriteOffset += 320;
			nDecOffset += _nDecLen;
		}
		nOffset += nAudioFrameLen;
	}
	return nWriteOffset;
}

// AMP���룬����Ӻ�˼ͷ��Ϣ
int AVg726::encodec(char* pAmpData, int nDataLen, char* pG726Data)
{
	if (!_stat) {
		return 0;
	}
	// ��Ӻ�˼��ʶ
	pG726Data[0] = 0;
	pG726Data[1] = 0x01;

	char* g726Data = pG726Data + KFlagHisiLen;
	int   g726Len = g726_encode((g726_state_t*)_stat, (uint8_t*)g726Data, (int16_t*)pAmpData, nDataLen / (sizeof(int16_t)));

	// ��˼ͷ���Ȱ������ֽڼ���
	pG726Data[2] = g726Len / sizeof(int16_t);
	pG726Data[3] = 0;
	return g726Len + KFlagHisiLen;
}
