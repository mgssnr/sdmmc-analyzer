#include <AnalyzerHelpers.h>
#include "SDMMCAnalyzerSettings.h"

SDMMCAnalyzerSettings::SDMMCAnalyzerSettings()
:	mClockChannel(UNDEFINED_CHANNEL),
	mCommandChannel(UNDEFINED_CHANNEL),
	mData0Channel(UNDEFINED_CHANNEL),
	mProtocol(PROTOCOL_MMC),
	mSampleEdge(SAMPLE_EDGE_RISING)
{
	mClockChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mClockChannelInterface->SetTitleAndTooltip("Clock", "Clock (CLK)");
	mClockChannelInterface->SetChannel(mClockChannel);

	mCommandChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mCommandChannelInterface->SetTitleAndTooltip("Command", "Command (CMD)");
	mCommandChannelInterface->SetChannel(mCommandChannel);

	mData0ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mData0ChannelInterface->SetTitleAndTooltip("Data0", "Data0 (DAT0)");
	mData0ChannelInterface->SetChannel(mData0Channel);

	mProtocolInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mProtocolInterface->SetTitleAndTooltip("Protocol", "Protocol");
	mProtocolInterface->AddNumber(PROTOCOL_MMC, "MMC", "MMC protocol");
	mProtocolInterface->AddNumber(PROTOCOL_SD,	"SD",  "SD protocol");
	mProtocolInterface->SetNumber(mProtocol);

	mSampleEdgeInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mSampleEdgeInterface->SetTitleAndTooltip("Sample edge", "Clock sampling edge");
	mSampleEdgeInterface->AddNumber(SAMPLE_EDGE_RISING,  "Rising",  "Sample on rising edge");
	mSampleEdgeInterface->AddNumber(SAMPLE_EDGE_FALLING, "Falling", "Sample on falling edge");
	mSampleEdgeInterface->SetNumber(mSampleEdge);

	AddInterface(mClockChannelInterface.get());
	AddInterface(mCommandChannelInterface.get());
	AddInterface(mData0ChannelInterface.get());
	AddInterface(mProtocolInterface.get());
	AddInterface(mSampleEdgeInterface.get());

	ClearChannels();
	AddChannel(mClockChannel, "Clock", false);
	AddChannel(mCommandChannel, "Command", false);
	AddChannel(mData0Channel, "Data0", false);
}

SDMMCAnalyzerSettings::~SDMMCAnalyzerSettings()
{
}

bool SDMMCAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel clk = mClockChannelInterface->GetChannel();
	Channel cmd = mCommandChannelInterface->GetChannel();
	Channel data0 = mData0ChannelInterface->GetChannel();

	if ((clk == cmd) || (clk == data0) || (cmd == data0)) {
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	mClockChannel = clk;
	mCommandChannel = cmd;
	mData0Channel = data0;
	mProtocol = SDMMCProtocol((U32)mProtocolInterface->GetNumber());
	mSampleEdge = SDMMCSampleEdge((U32)mSampleEdgeInterface->GetNumber());

	ClearChannels();
	AddChannel(mClockChannel, "Clock", true);
	AddChannel(mCommandChannel, "Command", true);
	AddChannel(mData0Channel, "Data0", true);

	return true;
}

void SDMMCAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mClockChannelInterface->SetChannel(mClockChannel);
	mCommandChannelInterface->SetChannel(mCommandChannel);
	mData0ChannelInterface->SetChannel(mData0Channel);
	mProtocolInterface->SetNumber(mProtocol);
	mSampleEdgeInterface->SetNumber(mSampleEdge);
}

void SDMMCAnalyzerSettings::LoadSettings(const char *settings)
{
	SimpleArchive archive;
	U32 tmp;

	archive.SetString(settings);

	archive >> mClockChannel;
	archive >> mCommandChannel;
	archive >> tmp; mProtocol = SDMMCProtocol(tmp);
	archive >> tmp; mSampleEdge = SDMMCSampleEdge(tmp);
	archive >> mData0Channel;

	ClearChannels();
	AddChannel(mClockChannel, "Clock", true);
	AddChannel(mCommandChannel, "Command", true);
	AddChannel(mData0Channel, "Data0", true);

	UpdateInterfacesFromSettings();
}

const char *SDMMCAnalyzerSettings::SaveSettings()
{
	SimpleArchive archive;

	archive << mClockChannel;
	archive << mCommandChannel;
	archive << mProtocol;
	archive << mSampleEdge;
	archive << mData0Channel;

	return SetReturnString(archive.GetString());
}
