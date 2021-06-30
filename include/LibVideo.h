// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LIBVIDEO_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LIBVIDEO_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
	// TODO:  在此添加您的方法。

	virtual ~VideoWriter();

	virtual bool setFile(const char* name) = 0;

	virtual bool close() = 0;

	virtual bool writeFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength) = 0;
};

class LIBVIDEO_API VideoReader
{
public:
	VideoReader(void);
	// TODO:  在此添加您的方法。

	virtual ~VideoReader();

	virtual bool openFile(const char* name) = 0;

	virtual bool close() = 0;

	virtual bool readFrame(unsigned short type, unsigned short channel, long long timestamp, char* frameBuf, int frameLength) = 0;
};

LIBVIDEO_API VideoWriter* createVideoWriter(VideoType_e type);

LIBVIDEO_API VideoReader* createVideoReader(VideoType_e type);