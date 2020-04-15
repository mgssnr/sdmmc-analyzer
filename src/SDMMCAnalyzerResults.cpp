#include "SDMMCAnalyzerResults.h"
#include "SDMMCAnalyzer.h"
#include "SDMMCAnalyzerSettings.h"
#include "SDMMCHelpers.h"
#include <string>
#include <iostream>
#include <sstream>

SDMMCAnalyzerResults::SDMMCAnalyzerResults(SDMMCAnalyzer* analyzer, SDMMCAnalyzerSettings* settings)
:	AnalyzerResults(),
	mSettings(settings),
	mAnalyzer(analyzer)
{
}

SDMMCAnalyzerResults::~SDMMCAnalyzerResults()
{
}

static std::string mmc_cmd_name(unsigned cmd)
{
	switch (cmd)
	{
	case MMC_CMD_GO_IDLE_STATE:
		return "GO_IDLE_STATE";
	case MMC_CMD_SEND_OP_COND:
		return "SEND_OP_COND";
	case MMC_CMD_ALL_SEND_CID:
		return "ALL_SEND_CID";
	case MMC_CMD_SET_RELATIVE_ADDR:
		return "SET_RELATIVE_ADDR";
	case MMC_CMD_SET_DSR:
		return "SET_DSR";
	case MMC_CMD_SWITCH:
		return "SWITCH";
	case MMC_CMD_SELECT_CARD:
		return "SELECT_CARD";
	case MMC_CMD_SEND_EXT_CSD:
		return "SEND_EXT_CSD";
	case MMC_CMD_SEND_CSD:
		return "SEND_CSD";
	case MMC_CMD_SEND_CID:
		return "SEND_CID";
	case MMC_CMD_STOP_TRANSMISSION:
		return "STOP_TRANSMISSION";
	case MMC_CMD_SEND_STATUS:
		return "SEND_STATUS";
	case MMC_CMD_SET_BLOCKLEN:
		return "SET_BLOCKLEN";
	case MMC_CMD_READ_SINGLE_BLOCK:
		return "READ_SINGLE_BLOCK";
	case MMC_CMD_READ_MULTIPLE_BLOCK:
		return "READ_MULTIPLE_BLOCK";
	case MMC_CMD_SET_BLOCK_COUNT:
		return "SET_BLOCK_COUNT";
	case MMC_CMD_WRITE_SINGLE_BLOCK:
		return "WRITE_SINGLE_BLOCK";
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
		return "WRITE_MULTIPLE_BLOCK";
	case MMC_CMD_ERASE_GROUP_START:
		return "ERASE_GROUP_START";
	case MMC_CMD_ERASE_GROUP_END:
		return "ERASE_GROUP_END";
	case MMC_CMD_ERASE:
		return "CMD_ERASE";
	case MMC_CMD_APP_CMD:
		return "APP_CMD";
	case MMC_CMD_SPI_READ_OCR:
		return "SPI_READ_OCR";
	case MMC_CMD_SPI_CRC_ON_OFF:
		return "SPI_CRC_ON_OFF";
	case MMC_CMD_RES_MAN:
		return "RES_MAN";
	default:
		return "<unused>";
	}
}

static std::string convert_R1_state(unsigned int state)
{
	switch (state)
	{
	case 0:  return "Idle"; break;
	case 1:  return "Ready"; break;
	case 2:  return "Ident"; break;
	case 3:  return "Stby"; break;
	case 4:  return "Tran"; break;
	case 5:  return "Data"; break;
	case 6:  return "Rcv"; break;
	case 7:  return "Prg"; break;
	case 8:  return "Dis"; break;
	case 9:  return "Btst"; break;
	case 10: return "Slp "; break;
	case 11: return "Rsvd 11"; break;
	case 12: return "Rsvd 12"; break;
	case 13: return "Rsvd 13"; break;
	case 14: return "Rsvd 14"; break;
	case 15: return "Rsvd 15"; break;
	}

}


void SDMMCAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base)
{
	ClearResultStrings();
	Frame frame = GetFrame(frame_index);

	switch (frame.mType) {
	case FRAMETYPE_HEADER:
		if (frame.mData1 == 1)
			AddResultString("Host sending");
		else
			AddResultString("Card sending");
		break;

	case FRAMETYPE_COMMAND:
	{
		char str_cmd[4];
		char str_arg[33];

		char str_crc[8];
		U64 crc = frame.mData2 & 0x7F;
		std::string str_flags("");

		frame.mData2 >>= 7;

		AnalyzerHelpers::GetNumberString(frame.mData1, Decimal, 6, str_cmd, sizeof(str_cmd));
		AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 32, str_arg, sizeof(str_arg));

		AnalyzerHelpers::GetNumberString(crc, Hexadecimal, 7, str_crc, sizeof(str_crc));

		AddResultString("CMD");
		AddResultString("CMD", str_cmd);

		str_flags += "CMD";
		str_flags += str_cmd;
		str_flags += " ";
		str_flags += mmc_cmd_name(frame.mData1);
		str_flags += ", arg=";
		str_flags += str_arg;
		str_flags += ", CRC=",
		str_flags += str_crc;

		AddResultString(str_flags.c_str());
		break;
	}

	case FRAMETYPE_RESPONSE:
	{
		char str_32[33];
		char str_crc[8];

		switch (frame.mFlags) {
		case MMC_RSP_R1:
		{
			std::string str_flags("");
			std::string str_state;

			str_state = convert_R1_state((frame.mData1 >> 9) & 0xf);

			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 32, str_32, sizeof(str_32));

			AddResultString("R1");
			AddResultString("R1, ", str_state.c_str());
			AddResultString("R1, ", str_state.c_str(), ", rsp=", str_32);

			if (frame.mData1 & (1 << 31))
					str_flags += " ADDRESS_OUT_OF_RANGE";
			if (frame.mData1 & (1 << 30))
					str_flags += " ADDRESS_MISALIGN";
			if (frame.mData1 & (1 << 29))
					str_flags += " BLOCK_LEN_ERROR";
			if (frame.mData1 & (1 << 28))
					str_flags += " ERASE_SEQ_ERROR";
			if (frame.mData1 & (1 << 27))
					str_flags += " ERASE_PARAM";
			if (frame.mData1 & (1 << 26))
					str_flags += " WP_VIOLATION";
			if (frame.mData1 & (1 << 25))
					str_flags += " CARD_IS_LOCKED";
			if (frame.mData1 & (1 << 24))
					str_flags += " LOCK_UNLOCK_FAILED";
			if (frame.mData1 & (1 << 23))
					str_flags += " COM_CRC_ERROR";
			if (frame.mData1 & (1 << 22))
					str_flags += " ILLEGAL_COMMAND";
			if (frame.mData1 & (1 << 21))
					str_flags += " CARD_ECC_FAILED";
			if (frame.mData1 & (1 << 20))
					str_flags += " CC_ERROR";
			if (frame.mData1 & (1 << 19))
					str_flags += " ERROR";
			if (frame.mData1 & (1 << 18))
					str_flags += " UNDERRUN";
			if (frame.mData1 & (1 << 17))
					str_flags += " OVERRUN";
			if (frame.mData1 & (1 << 16))
					str_flags += " CID/CSD_OVERWRITE";
			if (frame.mData1 & (1 << 15))
					str_flags += " WP_ERASE_SKIP";
			if (frame.mData1 & (1 << 13))
					str_flags += " ERASE_RESET";
			if (frame.mData1 & (1 << 8))
					str_flags += " READY_FOR_DATA";
			if (frame.mData1 & (1 << 7))
					str_flags += " SWITCH_ERROR";
			if (frame.mData1 & (1 << 6))
					str_flags += " URGENT_BKOPS";
			if (frame.mData1 & (1 << 5))
					str_flags += " APP_CMD";

			/* crc for responses is now in mData2 */
			AnalyzerHelpers::GetNumberString(frame.mData2, Hexadecimal, 7, str_crc, sizeof(str_crc));

			str_flags += ", CRC= ";
			str_flags += str_crc;

			if (str_flags.length() > 0)
				AddResultString("R1, ", str_state.c_str(), ", rsp=", str_32, str_flags.c_str());

			break;
		}

		case MMC_RSP_R2_CID:
		{
			std::string res("R2");
			char pname[7], prv_str[4], psn_str[12];
			char rsp_str[64];

			AddResultString(res.c_str());

			res += " [CID]";
			AddResultString(res.c_str());

			res += " rsp=";
			AnalyzerHelpers::GetNumberString(frame.mData1 >> 32, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData1 & 0xffffffffull, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData2 >> 32, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData2 & 0xffffffffull, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			AddResultString(res.c_str());

			pname[0] = (frame.mData1 >> 32) & 0xff;
			pname[1] = (frame.mData1 >> 24) & 0xff;
			pname[2] = (frame.mData1 >> 16) & 0xff;
			pname[3] = (frame.mData1 >>  8) & 0xff;
			pname[4] = (frame.mData1 >>  0) & 0xff;
			pname[5] = (frame.mData2 >> 56) & 0xff;
			pname[6] = 0;

			unsigned prv = (unsigned)((frame.mData2 >> 48) & 0xff);
			prv_str[0] = '0' + ((prv >> 4) & 0x0f);
			prv_str[1] = '.';
			prv_str[2] = '0' + (prv & 0x0f);
			prv_str[3] = 0;

			unsigned psn = (unsigned)((frame.mData2 >> 16) & 0xfffffffful);
			AnalyzerHelpers::GetNumberString(psn, Decimal, 32, psn_str, sizeof(psn_str));

			res += " pnm='";
			res += pname;
			res += "' prv=";
			res += prv_str;
			res += " psn=";
			res += psn_str;
			AddResultString(res.c_str());

			break;
		}

		case MMC_RSP_R2_CSD:
		{
			std::string res("R2");
			char rsp_str[64];

			AddResultString(res.c_str());

			res += " [CSD]";
			AddResultString(res.c_str());

			res += " rsp=";
			AnalyzerHelpers::GetNumberString(frame.mData1 >> 32, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData1 & 0xffffffffull, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData2 >> 32, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			res += " ";
			AnalyzerHelpers::GetNumberString(frame.mData2 & 0xffffffffull, display_base, 32, rsp_str, sizeof(rsp_str));
			res += rsp_str;
			AddResultString(res.c_str());

			break;
		}

		case MMC_RSP_R3:
		{
			std::string str_flags("");
			AnalyzerHelpers::GetNumberString(frame.mData1, Hexadecimal, 32, str_32, sizeof(str_32));
			AnalyzerHelpers::GetNumberString(frame.mData2, Hexadecimal, 7, str_crc, sizeof(str_crc));

			str_flags = "R3, ocr=";
			str_flags += str_32;
			str_flags += ", CRC= ";
			str_flags += str_crc;
			AddResultString("R3");
			AddResultString("R3, ocr=", str_flags.c_str());
			break;
		}

		case MMC_RSP_R4:
		{
			std::string str_flags("");
			AnalyzerHelpers::GetNumberString(frame.mData2, Hexadecimal, 7, str_crc, sizeof(str_crc));
			str_flags = "R4";
			str_flags += ", CRC= ";
			str_flags += str_crc;
			AddResultString(str_flags.c_str());
			break;
		}

		case MMC_RSP_R5:
		{
			std::string str_flags("");
			AnalyzerHelpers::GetNumberString(frame.mData2, Hexadecimal, 7, str_crc, sizeof(str_crc));
			str_flags = "R5";
			str_flags += ", CRC= ";
			str_flags += str_crc;
			AddResultString(str_flags.c_str());
			break;
		}
		break;
		}
	}

	case FRAMETYPE_CRC:
	{
		char str_crc[8];

		AnalyzerHelpers::GetNumberString(frame.mData1, Hexadecimal, 7, str_crc, sizeof(str_crc));

		AddResultString("CRC");
		AddResultString("CRC=", str_crc);
		break;
	}

	default:
		AddResultString("error");
	}
}

void SDMMCAnalyzerResults::GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void SDMMCAnalyzerResults::GenerateFrameTabularText(U64 frame_index, DisplayBase display_base)
{
	Frame frame = GetFrame(frame_index);

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	char time_str[128];

	std::stringstream ss;

	AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 10);
	ClearTabularText();

	switch (frame.mType) {
	case FRAMETYPE_COMMAND:
		char str_cmd[4];
		char str_arg[33];
		AnalyzerHelpers::GetNumberString(frame.mData1, Decimal, 6, str_cmd, sizeof(str_cmd));
		AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 32, str_arg, sizeof(str_arg));
		ss << "CMD" << str_cmd << " " << mmc_cmd_name(frame.mData1) << ", arg=" << str_arg;
		break;

	case FRAMETYPE_RESPONSE:
		char str_rsp[33];

		switch (frame.mFlags) {
		case MMC_RSP_R1:
			ss << "RSP1 ";

			ss << convert_R1_state((frame.mData1 >> 9) & 0xf);

			if (frame.mData1 & (1 << 31))
					ss << ", ADDRESS_OUT_OF_RANGE";
			if (frame.mData1 & (1 << 30))
					ss << ", ADDRESS_MISALIGN";
			if (frame.mData1 & (1 << 29))
					ss << ", BLOCK_LEN_ERROR";
			if (frame.mData1 & (1 << 28))
					ss << ", ERASE_SEQ_ERROR";
			if (frame.mData1 & (1 << 27))
					ss << ", ERASE_PARAM";
			if (frame.mData1 & (1 << 26))
					ss << ", WP_VIOLATION";
			if (frame.mData1 & (1 << 25))
					ss << ", CARD_IS_LOCKED";
			if (frame.mData1 & (1 << 24))
					ss << ", LOCK_UNLOCK_FAILED";
			if (frame.mData1 & (1 << 23))
					ss << ", COM_CRC_ERROR";
			if (frame.mData1 & (1 << 22))
					ss << ", ILLEGAL_COMMAND";
			if (frame.mData1 & (1 << 21))
					ss << ", CARD_ECC_FAILED";
			if (frame.mData1 & (1 << 20))
					ss << ", CC_ERROR";
			if (frame.mData1 & (1 << 19))
					ss << ", ERROR";
			if (frame.mData1 & (1 << 18))
					ss << ", UNDERRUN";
			if (frame.mData1 & (1 << 17))
					ss << ", OVERRUN";
			if (frame.mData1 & (1 << 16))
					ss << ", CID/CSD_OVERWRITE";
			if (frame.mData1 & (1 << 15))
					ss << ", WP_ERASE_SKIP";
			if (frame.mData1 & (1 << 13))
					ss << ", ERASE_RESET";
			if (frame.mData1 & (1 << 8))
					ss << ", READY_FOR_DATA";
			if (frame.mData1 & (1 << 7))
					ss << ", SWITCH_ERROR";
			if (frame.mData1 & (1 << 6))
					ss << " URGENT_BKOPS";
			if (frame.mData1 & (1 << 5))
					ss << ", APP_CMD";
			break;

		case MMC_RSP_R2_CID:
			ss << "RSP2 CID";
			break;
		case MMC_RSP_R2_CSD:
			ss << "RSP2 CSD";
			break;
		case MMC_RSP_R3:
			ss << "RSP3";
			break;
		case MMC_RSP_R4:
			ss << "RSP4";
			break;
		case MMC_RSP_R5:
			ss << "RSP5";
			break;
		}
		break;

	case FRAMETYPE_HEADER:
		ss << "Header";
		break;

	case FRAMETYPE_CRC:
		ss << "CRC";
		break;

	default:
		break;
	}

	AddTabularText(ss.str().c_str());
}

void SDMMCAnalyzerResults::GeneratePacketTabularText(U64 packet_id, DisplayBase display_base)
{
	ClearResultStrings();
	AddResultString("not supported");
}

void SDMMCAnalyzerResults::GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base)
{
	ClearResultStrings();
	AddResultString("not supported");
}
