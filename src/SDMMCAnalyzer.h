#ifndef __SDMMC_ANALYZER_H
#define __SDMMC_ANALYZER_H

#include <Analyzer.h>
#include "SDMMCAnalyzerSettings.h"
#include "SDMMCAnalyzerResults.h"
#include "SDMMCSimulationDataGenerator.h"
#include "SDMMCAnalyzer.h"
#include "SDMMCHelpers.h"

class SDMMCAnalyzer : public Analyzer2
{
public:
	static const char Name[];

public:
	SDMMCAnalyzer();
	virtual ~SDMMCAnalyzer();
	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char *GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected:
	void AdvanceToNextClock();
	int TryReadCommand();
	int WaitForAndReadMMCResponse(struct MMCResponse response);
	void FindDataLimits();

protected:
	std::auto_ptr<SDMMCAnalyzerSettings> mSettings;
	std::auto_ptr<SDMMCAnalyzerResults> mResults;

	SDMMCSimulationDataGenerator mDataGenerator;
	bool mSimulationInitialized;

	AnalyzerChannelData *mClock;
	AnalyzerChannelData *mCommand;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif
