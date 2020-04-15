#ifndef __SDMMC_HELPERS
#define __SDMMC_HELPERS

#include <Analyzer.h>

#define MMC_CMD_GO_IDLE_STATE           0
#define MMC_CMD_SEND_OP_COND            1
#define MMC_CMD_ALL_SEND_CID            2
#define MMC_CMD_SET_RELATIVE_ADDR       3
#define MMC_CMD_SET_DSR                 4
#define MMC_CMD_SWITCH                  6
#define MMC_CMD_SELECT_CARD             7
#define MMC_CMD_SEND_EXT_CSD            8
#define MMC_CMD_SEND_CSD                9
#define MMC_CMD_SEND_CID                10
#define MMC_CMD_STOP_TRANSMISSION       12
#define MMC_CMD_SEND_STATUS             13
#define MMC_CMD_SET_BLOCKLEN            16
#define MMC_CMD_READ_SINGLE_BLOCK       17
#define MMC_CMD_READ_MULTIPLE_BLOCK     18
#define MMC_CMD_SET_BLOCK_COUNT         23
#define MMC_CMD_WRITE_SINGLE_BLOCK      24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK    25
#define MMC_CMD_ERASE_GROUP_START       35
#define MMC_CMD_ERASE_GROUP_END         36
#define MMC_CMD_ERASE                   38
#define MMC_CMD_APP_CMD                 55
#define MMC_CMD_GEN_CMD			56
#define MMC_CMD_SPI_READ_OCR            58
#define MMC_CMD_SPI_CRC_ON_OFF          59
#define MMC_CMD_RES_MAN                 62

enum MMCResponseType {
	MMC_RSP_NONE,
	MMC_RSP_R1,
	MMC_RSP_R2_CID,
	MMC_RSP_R2_CSD,
	MMC_RSP_R3,
	MMC_RSP_R4,
	MMC_RSP_R5,
};

struct MMCResponse
{
	enum MMCResponseType mType;
	unsigned int mBits;
	int mTimeout;
	bool mBusy;
	bool mHasData;
};

class SDMMCHelpers
{
public:

public:
	static U8 crc7(const U8 *data, unsigned int size);
	static struct MMCResponse MMCCommandResponse(unsigned int index);

private:
	static U8 __crc7(U8 crc, U8 data);
	static U8 __crc7_finalize(U8 crc);
};

#endif
