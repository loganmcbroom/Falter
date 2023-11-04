---@meta

---@class Audio
Audio = {}

-- //============================================================================================================================================================
-- // Constructors
-- //============================================================================================================================================================

---Create an Audio with zero length
---@return Audio
function Audio.create_null( 
	) end

---Load audio data from a file. Uses libsndfile, so see its documentation for a full list of file types available. Notably, cannot load mp3.
---@param filename string
---@return Audio
function Audio.load_from_file( 
	filename
	) end

---Create an Audio given a length in seconds.
---@param length number
---@param num_channels? integer
---@param sample_rate? number
---@return Audio
function Audio.create_empty_with_length( 
	length,
	num_channels,
	sample_rate 
	) end

---Create an Audio given a length in frames.
---@param num_frames integer
---@param num_channels? integer
---@param sample_rate? number
---@return Audio
function Audio.create_empty_with_frames( 
	num_frames,
	num_channels,
	sample_rate 
	) end



-- //============================================================================================================================================================
-- // Conversions
-- //============================================================================================================================================================

---Create a copy with a different sample rate.
---@param new_sample_rate number
---@return Audio
function Audio:resample( 
	new_sample_rate 
	) end

---Save an image of the audio waveform.
---@param filename string
---@param interval? number[] The time interval to graph. Defaults to the entire audio. E.g. {0,1} for the first second.
---@param width? integer The output image width in pixels.
---@param height? integer The output image height in pixels.
---@return nil
function Audio:save_to_bmp(
	filename, 
	interval,
	width, 
	height
	) end

---Save an image of the audio spectrum.
---@param filename string
---@param width? integer The output image width in pixels.
---@param height? integer The output image height in pixels.
---@param smoothing_frames? integer
---@return nil
function Audio:save_spectrum_to_bmp(
	filename,
	width, 
	height,
	smoothing_frames
	) end

---Apply a short-time Forier transform to the Audio, and phase vocode the output.
---@param window_size? integer
---@param hop? integer
---@param dft_size? integer
---@return PV
function Audio:convert_to_PV(
	window_size,
	hop, 
	dft_size
	) end

---For stereo inputs this is identical to Audio::convert_to_PV, but converts the audio to mid-side first.
---@param window_size? integer
---@param hop? integer
---@param dft_size? integer
---@return PV
function Audio:convert_to_ms_PV(
	window_size,
	hop, 
	dft_size
	) end

---Convert a stereo left/right Audio into a mid/side Audio
---@return Audio
function Audio:convert_to_mid_side(
	) end

---Convert a stereo mid/side Audio into a left/right Audio
---@return Audio
function Audio:convert_to_left_right(
	) end

---This converts the input to a two channel Audio. The conversion is channel-count dependant.
---The currently accepted channel counts are: 1 and 2.
---@return Audio
function Audio:convert_to_stereo(
	) end

---This converts an Audio with any number of channels to a single channel Audio.
---Each output sample is the sum of the channels at that time over the square root of the number of channels.
---@return Audio
function Audio:convert_to_mono(
 	) end

---Convert to a function exactly. This function maps time to amplitude, rounding the input to the nearest frame.
---Inputs outside the Audio domain return 0.
---@return function
function Audio:convert_to_function(
	) end



-- //============================================================================================================================================================
-- // Channels
-- //============================================================================================================================================================

---Split the input over channels into an array of mono Audios
---@return Audio[]
function Audio:split_channels(
	) end

---Combine the input mono channels into a sigle multichannel Audio.
---@param channels Audio[]
---@return Audio
function Audio.combine_channels( 
	channels 
	) end



-- //============================================================================================================================================================
-- // Information
-- //============================================================================================================================================================

---Find the total energy in each channel of the input. Energy is the sum of the square of each sample. 
---@return number[]
function Audio:get_total_energy(
	) end

---Find the difference in energy between two Audios. This is useful for unit testing algorithms.
---@param other Audio
---@return number[]
function Audio:get_energy_difference( 
	other
	) end

---Try to find the wavelength of the input over time. Selects to minimize differences per repitition.
---@param channel integer
---@param start_frame integer
---@param window_size? integer
---@return number
function Audio:get_local_wavelength( 
 	channel,
 	start_frame, 
 	window_size
	) end

-- ---Try to find the wavelength of the input over time. This is estimated using parabolic interpolation, so it may not be an integer.
-- ---@param channel number The channel to process.
-- ---@param start number Start frame.
-- ---@param end number End frame.
-- ---@param window_size number The minimum that the local wave difference minimum must achieve to be considered a potential wavelength.
-- ---@param hop number The amount of input data used in each analysis.
-- ---@return number[] The hop size in frames per anlysis.
-- function Audio:get_local_wavelengths( 
-- 	channel, 
-- 	start, 
-- 	end,
-- 	window_size,
-- 	hop
--     ) end

-- /** Find the average of local wavelengths over time.
-- 	*	\param local_wavelengths Wavelengths to average. A -1 wavelength indicates no detectable wavelength. 
-- 	*	\param min_active_ratio This proportion of the inputs must have a detectable wavelength to continue computation. Otherwise, -1 is returned.
-- 	*	\param max_length_sigma If the standard deviation of the wavelengths is above this threshold, -1 is returned. Inputting -1 will disable sigma thresholding.
-- 	*/
-- ---@return number
-- function Audio:get_average_wavelength( 
-- -- 	const std::vector<float> & local_wavelengths, 
-- -- 	float min_active_ratio = 0, 
-- -- 	float max_length_sigma = -1 
--     ) end // Same as get_average_wavelengths

-- /** Find the average of local wavelengths over time. This is a utility for calling get_local_wavelength and passing it to the other get_average_wavelength.
-- 	*	\param channel The channel to process.
-- 	*	\param min_active_ratio This proportion of the inputs must have a detectable wavelength to continue computation. Otherwise, -1 is returned.
-- 	*	\param max_length_sigma If the standard deviation of the wavelengths is above this threshold, -1 is returned. Inputting -1 will disable sigma thresholding.
-- 	*	\param start Start frame.
-- 	*	\param end End frame.
-- 	*	\param window_size The amount of input data used in each analysis.
-- 	*	\param hop The hop size in frames per anlysis.
-- 	*/

-- ---comment
-- ---@param channel any
-- ---@param min_active_ratio any
-- ---@param max_length_sigma any
-- ---@param start any
-- ---@param end any
-- ---@param window_size any
-- ---@param hop any
-- function Audio:get_average_wavelength( 
-- 	channel,
-- 	min_active_ratio,
-- 	max_length_sigma,
-- 	start,
-- 	end,
-- 	window_size,
-- 	hop
--     ) end // Averages get_local_wavelengths

---Try to find the frequency of the input at a given time. Selects highest to minimize octave error.
---@param channel integer The channel to process.
---@param start_frame? integer Frame to analyze.
---@param window_size? integer Input window size.
---@return number
function Audio:get_local_frequency( 
	channel, 
	start_frame, 
	window_size 
    ) end

-- /** Utility for calling get_local_frequency at a set frame hop interval from start to end.
-- 	*/
-- std::vector<Frequency> get_local_frequencies( 
-- 	Channel channel, 
-- 	Frame start = 0, 
-- 	Frame end = -1, 
-- 	Frame window_size = 2048, 
-- 	Frame hop = 128, 
-- 	flan_CANCEL_ARG 
--    ) end

---Returns a function that approximates the amplitude envelope over time.
---@param window_width? number
---@return function
function Audio:get_amplitude_envelope(
	window_width
    ) end

---Returns a function that approximates the frequency envelope over time.
---@return function
function Audio:get_frequency_envelope( 
    ) end



-- //============================================================================================================================================================
-- // Temporal
-- //============================================================================================================================================================

---Modify the end points in time. 
---@param start_time_change number
---@param end_time_change number
---@return Audio
function Audio:modify_boundaries( 
 	start_time_change, 
	end_time_change 
    ) end

---Remove silence at the start and end of the Audio.
---@param non_silent_level number
---@return Audio
function Audio:remove_edge_silence( 
 	non_silent_level 
    ) end

---Remove any silence that sits below the non_silent_level for at least minimum_gap seconds.
---@param non_silent_level number
---@param minimum_gap number
---@return Audio
function Audio:remove_silence(
	non_silent_level,
	minimum_gap
    ) end

---Reverse the Audio.
---@return Audio
function Audio:reverse(
    ) end

---This returns a peice of the original Audio that lied between start_time and end_time.
---@param start_time number
---@param end_time number
---@param start_fade? number
---@param end_fade? number
---@return Audio
function Audio:cut( 
	start_time, 
	end_time, 
	start_fade, 
	end_fade 
    ) end

---This returns a peice of the original Audio that lied between start and end.
---@param start_frame number
---@param end_frame number
---@param start_fade? number
---@param end_fade? number
---@return Audio
function Audio:cut_frames( 
	start_frame, 
	end_frame, 
	start_fade, 
	end_fade 
    ) end

---Repitches the Audio.
---@param factor number|function
---@param granularity_time? number
---@return Audio
function Audio:repitch( 
	factor, 
	granularity_time
    ) end

---This repeats the input n times.
---@param num_iterations integer
---@param mod? function AudioMod
---@param feedback? boolean
---@return Audio
function Audio:iterate( 
	num_iterations, 
	mod,
	feedback
    ) end

---Generates a volume-decaying iteration of the input. If a mod is supplied, it will 
---@param output_length number
---@param delay_time number|function Second -> Second
---@param decay? number|function Second -> number
---@param mod? function AudioMod
---@return Audio
function Audio:delay( 
	output_length, 
	delay_time, 
	decay, 
	mod
    ) end

-- function Audio.stereo_delay(
-- 	outputlength,
-- 	const Function<Second, Second> & l_time,
-- 	const Function<Second, Second> & r_time,
-- 	const Function<Second, Amplitude> & decay
-- 	) end

---This slices the input into a vector of chunks, each slice_length seconds long.
---@param slice_length number
---@param fade? number
---@return Audio
function Audio:chop( 
	slice_length, 
	fade
    ) end

---This performs chop, randomizes the order of the chopped pieces, and joins, crossfading.
---@param slice_length number
---@param fade_time? number
---@return Audio
function Audio:rearrange( 
	slice_length, 
	fade_time
    ) end



-- //============================================================================================================================================================
-- // Volume
-- //============================================================================================================================================================

---Scales output by gain.
---@param gain number|function Second -> number
---@return Audio
function Audio:modify_volume( 
 	gain
    ) end

---Normalizes, then scales output by level.
---@param level number|function Second -> number
---@return Audio
function Audio:set_volume( 
	level
    ) end

---This adds a fade to the ends of the input Audio.
---@param start_time? number
---@param end_time? number
---@param interp? Interpolator
---@return Audio
function Audio:fade( 
	start_time,
	end_time,
	interp
    ) end

---This adds a fade to the ends of the input Audio.
---@param start_frames? number
---@param end_frames? number
---@param interp? Interpolator
---@return Audio
function Audio:fade_frames( 
	start_frames,
	end_frames,
	interp
    ) end

---@return Audio
function Audio:invert_phase(
    ) end

---This applies the shaper as a function to each sample in the input.
---@param shaper function Second, Sample -> Sample
---@param oversample_factor? integer
---@return Audio
function Audio:waveshape( 
	shaper,
	oversample_factor
    ) end

---This is meant to be a black box process for adding a moisture effect to bass signals. It is based on waveshape.
---@param amount? number|function Second -> Amplitude
---@param frequency? number|function Second -> Frequency
---@param skew? number|function Second -> number
---@param waveform? number|function Second -> Amplitude
---@return Audio
function Audio:add_moisture(
	amount,
	frequency,
	skew,
	waveform
    ) end

---This is a dynamic range compressor.
---@param threshold number|function Second -> Decibel
---@param compression_ratio? number|function Second -> number
---@param attack? number|function Second -> Second
---@param release? number|function Second -> Second
---@param knee_width? number|function Second -> Decibel
---@param sidechain_source? Audio
---@return Audio
function Audio:compress( 
	threshold, 
	compression_ratio, 
	attack,
	release,
	knee_width,
	sidechain_source 
    ) end



-- //============================================================================================================================================================
-- // Spatial
-- //============================================================================================================================================================

---Position the input in a horizontal 2d plane, with a movable position.
---@param position function Second -> number[]
---@return Audio
function Audio:stereo_spatialize_variable( 
	position 
    ) end

---Position the input in a horizontal 2d plane, with a fixed position.
---@param position number[]
---@return Audio
function Audio:stereo_spatialize( 
	position 
    ) end 

---This spatially repositions the input for mono and stereo inputs. Stereo speakers are assumed form an equilateral trangle with side
---length 1 meter with the listener. The input position is assumed to sit halfway between the speakers.
---If the number of channels is neither 1 nor 2 a null Audio is returned.
---@param pan_position number|function Second -> number. The input spatial position from -1 (left) to 1 (right) over time.
---@return Audio
function Audio:pan( 
	pan_position 
    ) end

---This redistributes energy between the mid and side signals
---@param widen_amount number|function Second -> number. This should return a value on [-1,1], representing movement from mid to side.
---@return Audio
function Audio:widen( 
	widen_amount 
    ) end



-- //============================================================================================================================================================
-- // Filters
-- //============================================================================================================================================================

-- /*
-- There are an essentially unlimited number of filter types and use cases, so I have made no attempt to cover all of them.
-- If you are reading this and your experience with filters is purely musical, you will find 2-pole filters to be
-- the familiar resonant filter type found in most DAW programs. The "bell" shaped filter found in most EQs is here called
-- a "band shelving filter".

-- Everything to do with filters here is based on the book "VA Filter Design" (second edition).
-- https://ia601900.us.archive.org/5/items/the-art-of-va-filter-design-rev.-2.1.2/VAFilterDesign_2.1.2.pdf#chapter.10
-- This textbook can be very rough around the edges, but most questions on why I am building filters a certain way can
-- be answered by searching it.
-- */

-- /* 1 Pole ============================ */

---
---@param cutoff number|function Second -> Frequency
---@param order? integer
---@return Audio
function Audio:filter_1pole_lowpass(
	cutoff,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param order? integer
---@return Audio
function Audio:filter_1pole_highpass(
	cutoff,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param order? integer
---@return Audio[]
function Audio:filter_1pole_split(
	cutoff,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param gain number|function Second -> Decibel
---@param order? integer
---@return Audio
function Audio:filter_1pole_lowshelf(
	cutoff,
	gain,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param gain number|function Second -> Decibel
---@param order? integer
---@return Audio
function Audio:filter_1pole_highshelf(
	cutoff,
	gain,
	order
    ) end

-- -- /** This applies the same 1-pole low pass filter to the input n times. 
-- -- 	It is mainly a tool used for modeling atmospheric scattering in spacialization methods.
-- -- */
-- function Audio:filter_1pole_repeat(
-- -- 	const Function<Second, Frequency> & cutoff,
-- -- 	const uint16_t repeats
--     ) end

-- /* 2 Pole ============================ */

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param order? integer
---@return Audio
function Audio:filter_2pole_lowpass(
	cutoff,
	damping,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param order? integer
---@return Audio
function Audio:filter_2pole_bandpass(
	cutoff,
	damping,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param order? integer
---@return Audio
function Audio:filter_2pole_highpass(
	cutoff,
	damping,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param order? integer
---@return Audio
function Audio:filter_2pole_notch(
	cutoff,
	damping,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param gain number|function Second -> Decibel
---@param order? integer
---@return Audio
function Audio:filter_2pole_lowshelf(
	cutoff,
	damping,
	gain,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param gain number|function Second -> Decibel
---@param order? integer
---@return Audio
function Audio:filter_2pole_bandshelf(
	cutoff,
	damping,
	gain,
	order
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param gain number|function Second -> Decibel
---@param order? integer
---@return Audio
function Audio:filter_2pole_highshelf(
	cutoff,
	damping,
	gain,
	order
    ) end

-- /* Other filters ==================== */

---
---@param cutoff number|function Second -> Frequency
---@param wet_dry? number|function Second -> number
---@param order? integer
---@param invert? boolean
---@return Audio
function Audio:filter_1pole_multinotch(
	cutoff,
	wet_dry,
	order,
	invert
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param damping number|function Second -> number
---@param wet_dry? number|function Second -> number
---@param order? integer
---@param invert? boolean
---@return Audio
function Audio:filter_2pole_multinotch(
	cutoff,
	damping,
	wet_dry,
	order,
	invert
    ) end

---
---@param cutoff number|function Second -> Frequency
---@param wet_dry? number|function Second -> number
---@param feedback? number|function Second -> number
---@param invert? boolean
---@return Audio
function Audio:filter_comb(
	cutoff,
	wet_dry,
	feedback,
	invert
    ) end



-- //============================================================================================================================================================
-- // Combination
-- //============================================================================================================================================================

---
---@param inputs Audio[]|AudioVec
---@param start_times? number[]
---@param gains? number[]
---@return Audio
function Audio.mix( 
	inputs, 
	start_times, 
	gains
	) end

---
---@param inputs Audio[]|AudioVec
---@param start_times number[]
---@param gains function[] Second -> Amplitude
---@return Audio
function Audio.mix_variable_gain( 
	inputs, 
	start_times, 
	gains 
	) end

---
---@param inputs Audio[]|AudioVec
---@param offset_time? number The amount of time away from the natural join position each input should be. For example, using a negative offset 
---along with fading the inputs can be used to crossfade.
---@return Audio
function Audio.join( 
	inputs, 
	offset_time
	) end
	
---At each point in time, selection decides which of the input Audio streams is playing.
---Non-integer selections will mix appropriately scaled copies of the surrounding integer inputs.
---@param inputs Audio[]|AudioVec
---@param selection function Second -> number
---@param start_times? number[]
---@return Audio
function Audio.select( 
	inputs,
	selection,
	start_times
	) end

---This is normal convolution between Audio. If the number of channels are mismatched, the ir channels will be used cyclically.
---@param impulse_response Audio
---@return Audio
function Audio:convolve( 
	impulse_response 
    ) end



-- //============================================================================================================================================================
-- // Synthesis
-- //============================================================================================================================================================

---Generate an Audio from a waveform function. 
---@param waveform function Second -> Amplitude
---@param length number
---@param freq number|function Second -> Frequency
---@param sample_rate? number
---@param oversample? integer
---@return Audio
function Audio.synthesize_waveform(  
	waveform,
	length, 
	freq, 
	sample_rate,
	oversample
	) end

---Additive synthesis of an impulse.
---@param base_freq number
---@param num_harmonics? integer
---@param chroma? number
---@param sample_rate? number
---@return Audio
function Audio.synthesize_impulse(
	base_freq,
	num_harmonics,
	chroma,
	sample_rate
	) end

-- Grain Controllers ===================================================================================================================

-- ---This is the base granular algorithm in flan. Most other granular concepts can be built on it, but using a grain source function
-- ---isn't very effecient. If the grain being used is constant, synthesize_grains_repeat does the same thing but faster.
-- ---@param length any
-- ---@param grains_per_second any
-- ---@param time_scatter any
-- ---@param grain_source any
-- ---@param sample_rate any
-- function Audio.synthesize_grains( 
-- 	length,
-- 	grains_per_second,
-- 	time_scatter,
-- 	grain_source,
-- 	sample_rate
-- 	) end
	
---Same as synthesize_grains, but reuses the same sound source.
---@param length number
---@param grains_per_second number|function Second -> number
---@param time_scatter number|function Second -> number
---@param gain number|function Second -> Amplitude
---@param sample_rate number
---@return Audio
function Audio:synthesize_grains_repeat(
	length,
	grains_per_second,
	time_scatter,
	gain,
	sample_rate
    ) end

---Same as synthesize_grains_repeat, but applies an AudioMod to each output.
---@param length number
---@param grains_per_second  number|function Second -> number
---@param time_scatter number|function Second -> number
---@param mod AudioMod
---@param sample_rate? number
---@return Audio
function Audio:synthesize_grains_with_feedback_mod( 
	length, 
	grains_per_second, 
	time_scatter, 
	mod,
	mod_feedback,
	sample_rate
    ) end

-- Grain Compositions ===================================================================================================================

---Trainlet synthesis as seen in the book "Microsound".
---@param length number
---@param grains_per_second number|function Second -> number
---@param time_scatter number|function Second -> number
---@param position number|function Second -> number[]. Spatial plane position to spacialize trainlet at.
---@param trainlet_gain_envelope number|function Second -> Amplitude. The amplitude over time of individual trainlets 
---@param impulse_freq number|function Second -> Frequency. The rate at which impulses occur within an individual trainlet. 
---@param trainlet_length number|function Second -> Second.
---@param num_harmonics? number|function Second -> integer. How many harmonics should be used to synthesize an impulse.
---@param chroma? number|function Second -> number. A geometric scaling factor on the harmonics within an impulse.
---@param impulse_harmonic_frequency? number|function Second -> Frequency. The frequency of the harmonics used in impulse generation.
---@param sample_rate? number
---@return Audio
function Audio.synthesize_trainlets( 
	length,
	grains_per_second,
	time_scatter,
	position,
	trainlet_gain_envelope,
	impulse_freq,
	trainlet_length,
	num_harmonics,
	chroma,
	impulse_harmonic_frequency,
	sample_rate
	) end

---Granulation is similar to normal granular synthesis, but it uses segments of a single input Audio as the grain source.
---@param length number
---@param grains_per_second  number|function Second -> number
---@param time_scatter number|function Second -> number
---@param time_selection number|function Second -> Second
---@param grain_length number|function Second -> Second
---@param mod? AudioMod
---@return Audio
function Audio:synthesize_granulation( 
	length, 
	grains_per_second, 
	time_scatter, 
	time_selection, 
	grain_length,
	mod
    ) end

---Very basic psola implementation. This will likely be updated in the future for more advanced grain syncronizatian, but for now it
---can still produce many interesting sounds.
---@param length number
---@param time_selection number|function Second -> Second
---@param mod? AudioMod
---@return Audio
function Audio:synthesize_psola(
	length, 
	time_selection,
	mod
    ) end
