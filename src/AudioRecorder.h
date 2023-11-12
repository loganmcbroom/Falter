#pragma once

#include <JuceHeader.h>

#include "Types.h"

class AudioRecorder : public AudioIODeviceCallback
{
public:
	AudioRecorder();
	~AudioRecorder() override;

	bool isRecording() const;
	void startRecording();
	std::unique_ptr<flan::Audio> stopRecording();
	int getNumRecords() const;

private:
	void audioDeviceAboutToStart( AudioIODevice * device ) override;
	void audioDeviceStopped() override;
	void audioDeviceIOCallbackWithContext( const float *const *, int, float *const *, int, int, const AudioIODeviceCallbackContext & ) override;

	int numRecords;
	std::unique_ptr<std::vector<std::vector<float>>> outputBuffer;
    int sample_rate;
	int num_channels;
};