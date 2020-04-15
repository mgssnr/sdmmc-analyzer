#include <AnalyzerHelpers.h>
#include "SDMMCAnalyzerSettings.h"

SDMMCAnalyzerSettings::SDMMCAnalyzerSettings()
:	mClockChannel(UNDEFINED_CHANNEL),
	mCommandChannel(UNDEFINED_CHANNEL),
	mDat0Channel(UNDEFINED_CHANNEL),
	mProtocol(PROTOCOL_MMC),
	mSampleEdge(SAMPLE_EDGE_RISING)
{
	mClockChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mClockChannelInterface->SetTitleAndTooltip("Clock", "Clock (CLK)");
	mClockChannelInterface->SetChannel(mClockChannel);

	mCommandChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mCommandChannelInterface->SetTitleAndTooltip("Command", "Command (CMD)");
	mCommandChannelInterface->SetChannel(mCommandChannel);

	mDat0ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mDat0ChannelInterface->SetTitleAndTooltip("DAT0", "Data 0 (DAT0)");
	mDat0ChannelInterface->SetChannel(mDat0Channel);

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
	AddInterface(mDat0ChannelInterface.get());
	AddInterface(mProtocolInterface.get());
	AddInterface(mSampleEdgeInterface.get());

	ClearChannels();
	AddChannel(mClockChannel, "Clock", false);
	AddChannel(mCommandChannel, "Command", false);
	AddChannel(mDat0Channel, "Data 0", false);
}

SDMMCAnalyzerSettings::~SDMMCAnalyzerSettings()
{
}

bool SDMMCAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel clk = mClockChannelInterface->GetChannel();
	Channel cmd = mCommandChannelInterface->GetChannel();
	Channel dat0 = mDat0ChannelInterface->GetChannel();

	if ((clk == cmd) || (clk == dat0) || (cmd == dat0)) {
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	mClockChannel = clk;
	mCommandChannel = cmd;
	mDat0Channel = dat0;
	mProtocol = SDMMCProtocol((U32)mProtocolInterface->GetNumber());
	mSampleEdge = SDMMCSampleEdge((U32)mSampleEdgeInterface->GetNumber());

	ClearChannels();
	AddChannel(mClockChannel, "Clock", true);
	AddChannel(mCommandChannel, "Command", true);
	AddChannel(mDat0Channel, "Data 0", true);

	return true;
}

void SDMMCAnalyzerSettings::LoadSettings(const char *settings)
{
	SimpleArchive archive;
	archive.SetString(settings);

	const char *name_string;
	archive >> &name_string;
	if (strcmp(name_string, "SDMMCAnalyzer") != 0)
		AnalyzerHelpers::Assert("SDMMCAnalyzer: Provided with a settings string that doesn't belong to us;" );

	archive >> mClockChannel;
	archive >> mCommandChannel;
	archive >> mDat0Channel;
	archive >> *(U32*) &mProtocol;
	archive >> *(U32*) &mSampleEdge;

	ClearChannels();
	AddChannel(mClockChannel, "Clock", true);
	AddChannel(mCommandChannel, "Command", true);
	AddChannel(mDat0Channel, "Dat 0", true);

	UpdateInterfacesFromSettings();
}

const char *SDMMCAnalyzerSettings::SaveSettings()
{
	SimpleArchive archive;

	archive << "SDMMCAnalyzer";
	archive << mClockChannel;
	archive << mCommandChannel;
	archive << mDat0Channel;
	archive << mProtocol;
	archive << mSampleEdge;

	return SetReturnString(archive.GetString());
}

void SDMMCAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mClockChannelInterface->SetChannel(mClockChannel);
	mCommandChannelInterface->SetChannel(mCommandChannel);
	mDat0ChannelInterface->SetChannel(mDat0Channel);
	mProtocolInterface->SetNumber(mProtocol);
	mSampleEdgeInterface->SetNumber(mSampleEdge);
}

