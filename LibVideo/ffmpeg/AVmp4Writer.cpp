#include "stdafx.h"
#include "AVmp4Writer.h"

const char nalucode[5] = { 0x00, 0x00, 0x00, 0x01 };
const int nalucodelength = 4;

AVmp4Writer::AVmp4Writer(/* args */)
{
	_pOfmtCtx = NULL;
	_nfps = 0;
	_isWaitKeyframe = true;
	_nframeType = AV_CODEC_ID_NONE;
	_g726ToAac.init(AAC_STREAM_RAW);
	_lastVtimestamp = 0;
	_lastAtimestamp = 0;
}

AVmp4Writer::~AVmp4Writer()
{

}

void AVmp4Writer::fpname(const char* fp)
{
	_pfpName = fp;
	if (!strstr(fp, ".mp4")) 
		_pfpName.append(".mp4");
}

void AVmp4Writer::close()
{
	if (NULL != _pOfmtCtx) {
		av_write_trailer(_pOfmtCtx);
		avio_closep(&_pOfmtCtx->pb);
		avformat_free_context(_pOfmtCtx);
		_pOfmtCtx = NULL;
	}
	_g726ToAac.uinit();
}

bool AVmp4Writer::writeVideoframe(char* frame, int len, bool iskey, unsigned long long pts)
{
	// 关键帧
	if (_isWaitKeyframe && iskey) {
		// 解析视频数据格式
		unsigned char type = frame[nalucodelength];
		// 0x01 slice 0x05 idr 0x06 sei 0x07 sps 0x08 pps
		if (type == 0x40 || type == 0x42 || type == 0x44 || type == 0x4E || type == 0x26 || type == 0x02) 
			_nframeType = AV_CODEC_ID_HEVC;
		else
			_nframeType = AV_CODEC_ID_H264;

		if (!doParseExtraData(frame, len))
			return false;
		
		initfp();
		_isWaitKeyframe = false;
		_startTimestamp = pts;
	}
	if (_nframeType == AV_CODEC_ID_NONE || _isWaitKeyframe || pts < _lastVtimestamp) 
		return false;

	//int slicelength = 0;
	//char* slice = getSliceData(frame, len, slicelength);
	//if (slicelength <= 0) 
	//	return false;
	
	uint32_t dts = (pts - _startTimestamp) / 1000;
	AVPacket pkt;
	av_init_packet(&pkt);
	if (iskey) 
		pkt.flags |= AV_PKT_FLAG_KEY;

	pkt.stream_index = _nVideoStreamIndex;
	pkt.data = (uint8_t*)frame;
	pkt.size = len;

	//当前相对于第一包数据的毫秒数
	pkt.dts = dts;
	pkt.pts = dts;
	//_pOfmtCtx里面的time_base 和
	//_pIfmtCtx里面的time_base不一样，一定要做下面的转换，否则，就会出现视频时长不正确，帧率不一样的错误
	av_packet_rescale_ts(&pkt, av_make_q(1, 1000), _pOfmtCtx->streams[pkt.stream_index]->time_base);
	int ret = av_interleaved_write_frame(_pOfmtCtx, &pkt);
	avio_flush(_pOfmtCtx->pb);
	av_packet_unref(&pkt);
	_lastVtimestamp = pts;
	return true;
}

bool AVmp4Writer::writeAudioframe(char* frame, int len, unsigned long long pts)
{
	if (_isWaitKeyframe || pts < _lastAtimestamp)  //等待主帧
		return false;

	//当前时间戳比第一包时间戳早,则不处理
	int            aacLen = 0;
	unsigned char* aac = _g726ToAac.toAacEncodec(frame, len, aacLen);
	if (aacLen == 0) 
		return false;

	uint32_t dts = (pts - _startTimestamp) / 1000;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.stream_index = _nAudioStreamIndex;
	pkt.data = (uint8_t*)aac;
	pkt.size = (int)aacLen;

	//当前相对于第一包数据的毫秒数
	pkt.dts = dts;
	pkt.pts = dts;

	//_pOfmtCtx里面的time_base 和
	//_pIfmtCtx里面的time_base不一样，一定要做下面的转换，否则，就会出现视频时长不正确，帧率不一样的错误
	av_packet_rescale_ts(&pkt, av_make_q(1, 1000), _pOfmtCtx->streams[pkt.stream_index]->time_base);
	int ret = av_interleaved_write_frame(_pOfmtCtx, &pkt);
	avio_flush(_pOfmtCtx->pb);
	av_packet_unref(&pkt);
	_lastAtimestamp = pts;
	return true;
}

bool AVmp4Writer::initfp()
{
	av_register_all();
	int code = avformat_alloc_output_context2(&_pOfmtCtx, NULL, NULL, _pfpName.c_str());
	if (code != 0 || NULL == _pOfmtCtx) 
		return false;
	
	_pOfmtCtx->debug |= FF_FDEBUG_TS;
	newAVStream();
	//需要在open之前avformat_new_stream,否则可能会导致文件不正确
	code = avio_open(&(_pOfmtCtx->pb), _pfpName.c_str(), AVIO_FLAG_WRITE);
	if (0 != code) 
		return false;
	
	// 写入流信息
	avformat_write_header(_pOfmtCtx, NULL);
	return true;
}

// H264sps信息解析
void AVmp4Writer::doAvcExtraData(NaluUint_t* nalu)
{
	int nut = nalu->type & 0x1F;
	switch (nut) {
	case avc::NALU_TYPE_SPS: {
		std::string sps = std::string(nalu->data + nalucodelength, nalu->size - nalucodelength);
		avc::decode_sps((unsigned char*)sps.c_str(), sps.length(), _nWidth, _nHeight, _nfps);
	}
	case avc::NALU_TYPE_PPS:
		_vExtraData.append(nalu->data, nalu->size);
		break;
	default:
		break;
	}
}

// H265sps信息解析
void AVmp4Writer::doHevcExtraData(NaluUint_t* nalu)
{
	int nut = (nalu->type & 0x7E) >> 1;
	switch (nut) {
	case hevc::NAL_UNIT_SPS:{
		std::string sps = std::string(nalu->data + nalucodelength, nalu->size - nalucodelength);
		avc::decode_sps((unsigned char*)sps.c_str(), sps.length(), _nWidth, _nHeight, _nfps);
	}
	case hevc::NAL_UNIT_PPS:
	case hevc::NAL_UNIT_VPS:
		_vExtraData.append(nalu->data, nalu->size);
		break;
	case hevc::NAL_UNIT_CODED_SLICE_TRAIL_R:
	case hevc::NAL_UNIT_CODED_SLICE_IDR:
		break;
	default:
		break;
	}
}

bool AVmp4Writer::doParseExtraData(char* frame, int length)
{
	_vExtraData = "";
	char* pNaluData = frame;
	int resize = length;
	NaluUint_t nalu = { 0 };
	for (;;)
	{
		pNaluData = nalUnitGet(&nalu, nalucodelength, pNaluData, resize);
		_nframeType == AV_CODEC_ID_H264 ? doAvcExtraData(&nalu) : doHevcExtraData(&nalu);
		if (resize <= 0)
			break;
	}
	if (_vExtraData.length() > 0)
		return true;

	return false;
}

char* AVmp4Writer::getSliceData(char* frame, int length, int& sliceLength)
{
	char* pNaluData = frame;
	int resize = length;
	NaluUint_t nalu = { 0 };
	for (;;)
	{
		pNaluData = nalUnitGet(&nalu, nalucodelength, pNaluData, resize);
		if (_nframeType == AV_CODEC_ID_H264) {
			int nut = nalu.type & 0x1F;
			if (nut == avc::NALU_TYPE_IDR || nut == avc::NALU_TYPE_SLICE) 
				break;
		} else {
			int nut = (nalu.type & 0x7E) >> 1;
			if (nut == hevc::NAL_UNIT_CODED_SLICE_TRAIL_R || nut == hevc::NAL_UNIT_CODED_SLICE_IDR)
				break;
		}

		if (resize <= 0)
			break;
	}
	sliceLength = nalu.size;
	return nalu.data;
}

void AVmp4Writer::newAVStream()
{
	{
		AVStream* pAvStream = avformat_new_stream(_pOfmtCtx, NULL);
		if (pAvStream == NULL)
			return;

		_nVideoStreamIndex = _pOfmtCtx->nb_streams - 1;
		pAvStream->codecpar->width = _nWidth;
		pAvStream->codecpar->height = _nHeight;
		pAvStream->codecpar->format = AV_PIX_FMT_YUV420P;
		pAvStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
		pAvStream->codecpar->codec_id = _nframeType;
		pAvStream->avg_frame_rate = av_make_q(_nfps, 1);
		pAvStream->r_frame_rate = av_make_q(_nfps, 1);

		//此处需要按照该规则来创建对应的缓冲区,否则调用avformat_free_context会报错
		//也可以采用自己的缓冲区,然后再avformat_free_context之前把这两个变量置空
		// 需要把Sps/Pps/Vps写入扩展信息中
		int videoExtraLength = _vExtraData.length();
		pAvStream->codecpar->extradata = (uint8_t*)av_malloc(videoExtraLength + AV_INPUT_BUFFER_PADDING_SIZE);
		::memcpy(pAvStream->codecpar->extradata, _vExtraData.c_str(), videoExtraLength);
		pAvStream->codecpar->extradata_size = videoExtraLength;
	}
	//  创建音频输入格式
	{
		AVStream* pAvStream = avformat_new_stream(_pOfmtCtx, NULL);
		if (pAvStream == NULL) 
			return;

		_nAudioStreamIndex = _pOfmtCtx->nb_streams - 1;
		pAvStream->codecpar->channels = 1;
		pAvStream->codecpar->channel_layout = av_get_default_channel_layout(pAvStream->codecpar->channels);
		pAvStream->codecpar->sample_rate = 8000;
		pAvStream->codecpar->format = AV_SAMPLE_FMT_S16;
		pAvStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
		pAvStream->codecpar->codec_id = AV_CODEC_ID_AAC;
		pAvStream->codecpar->frame_size = 1024;

		pAvStream->codecpar->bit_rate = 16000;

		int            specialLen = 0;
		unsigned char* pSpecialData = _g726ToAac.aacSpecialData(&specialLen);

		pAvStream->codecpar->extradata = (uint8_t*)av_malloc(specialLen + AV_INPUT_BUFFER_PADDING_SIZE);
		::memcpy(pAvStream->codecpar->extradata, pSpecialData, specialLen);
		pAvStream->codecpar->extradata_size = specialLen;
	}
}