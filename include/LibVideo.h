// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LIBVIDEO_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LIBVIDEO_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LIBVIDEO_EXPORTS
#define LIBVIDEO_API __declspec(dllexport)
#else
#define LIBVIDEO_API __declspec(dllimport)
#endif

typedef enum
{
	KAV_FILE_UNKNOW,
	KAV_FILE_MP4_WRITER,
	KAV_FILE_MP4_READER,
} VideoType_e;

class LIBVIDEO_API VideoWriter
{
public:
	VideoWriter(void);
	// TODO:  �ڴ�������ķ�����

	virtual ~VideoWriter();

	virtual bool setFile(const char* name) = 0;

	virtual bool close() = 0;

	virtual bool writeFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength) = 0;
};

class LIBVIDEO_API VideoReader
{
public:
	VideoReader(void);
	// TODO:  �ڴ�������ķ�����

	virtual ~VideoReader();

	virtual bool openFile(const char* name) = 0;

	virtual bool close() = 0;

	virtual bool readFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength) = 0;
};

LIBVIDEO_API VideoWriter* createVideoWriter(VideoType_e type);

LIBVIDEO_API VideoReader* createVideoReader(VideoType_e type);