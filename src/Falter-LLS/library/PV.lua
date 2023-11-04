---@meta

---@class PV
PV = {}

-- //============================================================================================================================================================
-- // Constructors
-- //============================================================================================================================================================

---Create an PV with zero length
---@return PV
function PV.create_null( 
	) end

---Load pv .flan data from a file.
---@param filename string
---@return PV
function PV.load_from_file( 
	filename
	) end



-- //============================================================================================================================================================
-- // Conversions
-- //============================================================================================================================================================

---Transforms this PV to an Audio using the overlaps and samplerate given in format. 
---@return Audio
function PV:convert_to_audio(  
    ) end

---The inverse transform of Audio::convert_to_ms_PV.
---@return Audio
function PV:convert_to_lr_audio(  
    ) end

---Creates and saves a bmp spectrograph of the PV.
---@param filename string
---@param domain? number[] E.g. { time_start, freq_start, time_end, freq_end }. Negative 1 for time or frequency end will use the maximum.
---@param width? integer
---@param height? integer
---@return nil
function PV:save_to_bmp( 
	filename, 
	domain,
	width,
	height
    ) end



-- //============================================================================================================================================================
-- // Contours
-- //============================================================================================================================================================

---Prism, in theory, allows complete control over the frequency and magnitude of every harmonic of every note in the input.
---@param prism_function PrismFunc This takes the global or local time (see use_local_contour_time), the harmonic index (starting at 0), the base frequency and the 
---magnitudes of all harmonics. It should return the MF that the input harmonic should be modified to. This function type is defined in Function.h.
---@param use_local_contour_time? boolean This decides if the time passed to harmonic_function should be the time elapsed since the start of the 
---PV (false), or the start of the contour (true).
---@return PV
function PV:prism( 
	prism_function, 
	use_local_contour_time
    ) end



-- //============================================================================================================================================================
-- // Selection
-- //============================================================================================================================================================

---Every time/frequency point in the output PV will read an arbitrary point in the input determined by selector.
---@param length number
---@param selector function Second, Frequency -> Second, Frequency
---@return PV
function PV:select( 
	length, 
	selector
    ) end

---This is a classic "time-freeze" effect. At supplied times playback is "frozen", repeating the current frame 
---(or a linear interpolation of surrounding frames), for a specified amount of time. After this time has elapsed,
---playback resumes until another freeze time is encountered.
---@param pause_times number[]
---@param pause_lengths number[]
---@return PV
function PV:freeze( 
	pause_times,
	pause_lengths 
    ) end



-- //============================================================================================================================================================
-- // Resampling
-- //============================================================================================================================================================

---Each input bin is mapped to an arbitrary output point by mod. 
---Every quad in the input is thus mapped to a quad in the output. 
---The output points within that quad are given an average of any data already within the bin 
---and the four input bins that made up the quad being mapped.
---Quad averages are decided using the interpolator argument and the algorithm described here: 
---https://www.particleincell.com/2012/quad-interpolation/
---@param mod function Second, Frequency -> Second, Frequency
---@param interpolator? Interpolator
---@return PV
function PV:modify( 
	mod, 
	interpolator
    ) end

---This is functionally equivalent to using PV::modify and only outputting the input time
---@param mod function Second, Frequency -> Frequency
---@param interpolator? Interpolator
---@return PV
function PV:modify_frequency( 
	mod, 
	interpolator 
    ) end

---This is functionally equivalent to using PV::modify and only outputting the input frequency
---@param mod function Second, Frequency -> Second
---@param interpolator? Interpolator
---@return PV
function PV:modify_time( 
	mod, 
	interpolator
    ) end

---This is functionally equivalent to using PV::modify_frequency with the mod output multiplied by input frequency
---@param factor function Second, Frequency -> number
---@param interpolator? Interpolator
---@return PV
function PV:repitch( 
	factor,
	interpolator
    ) end

---This is functionally equivalent to using PV::modifySecond with the mod output multiplied by input time
---@param factor function Second, Frequency -> number
---@param interpolator? Interpolator
---@return PV
function PV:stretch( 
	factor, 
	interpolator
    ) end

---This is close to PV::stretch, but can only expand the input by integer quantities at any given time.
---It is also restricted to choosing the local expansion amount as a function of time, not frequency.
---Spline interpolation is used to fill the expanded space.
---@param expansion function Second -> number
---@return PV
function PV:stretch_spline( 
	expansion 
    ) end

---This is functionally equivalent to stretching the input down by factor, and then up.
---Resolution is lost, but the lost resolution is filled via interpolation.
---@param events_per_second function Second, Frequency -> number
---@param interpolator? Interpolator
---@return PV
function PV:desample( 
	events_per_second, 
	interpolator
    ) end

---Warning: this process can produce very loud outputs with unscrupulouss parameters.
---The input is left alone until start_time. Between start_time and end_time the output is an interpolation
---of the frames at start_time and end_time. After end_time, the interpolation continues into the realm of extrapolation
---for extrap_time seconds. High frequency partials in the input can cause unpleasant sine sweeps in the output,
---so you may want to use a mild low pass filter before processing.
---@param start_time number
---@param end_time number
---@param extrap_time number
---@param interpolator? Interpolator
---@return PV
function PV:time_extrapolate( 
	start_time, 
	end_time, 
	extrap_time, 
	interpolator
    ) end


-- //============================================================================================================================================================
-- // Generation
-- //============================================================================================================================================================

---Generates a PV from a spectrum.
---@param length number
---@param frequency number
---@param harmonic_weights? function Second, Harmonic -> Magnitude. This takes time and a harmonic index starting at 0, and returns a magnitude.
---@param harmonic_bandwidth? number|function Second -> Frequency. Each generated harmonic is spread over this amount of frequency.
---@param harmonic_frequency_std_dev? number|function Time, Frequency -> Frequency
---@return PV
function PV.synthesize( 
	length, 
	frequency, 
	harmonic_weights,
	harmonic_bandwidth,
	harmonic_frequency_std_dev
	) end



-- //============================================================================================================================================================
-- // Extras
-- //============================================================================================================================================================

---For all frequencies, every octave above it is set to a copy of the base, scaled by series_scale.
---@param series_scale function Second,Harmonic -> number. The scaling function. The inputs to this are time and harmonic index starting at 0.
---@return PV
function PV:add_octaves( 
	series_scale 
    ) end

---For all frequencies, every harmonic above it is set to a copy of the base, scaled by series_scale
---@param series_scale function Second,Harmonic -> number. The scaling function. The inputs to this are time and harmonic index starting at 0.
---@return PV
function PV:add_harmonics( 
	series_scale 
    ) end

---Replaces the Amplitudes (Magnitudes) of bins in the input with those in amp_source
---@param amp_source PV Source PV from which to draw amplitudes.
---@param amount? number|function Second, Frequency -> number. An amount of 1 fully replaces the amplitudes, 0 does nothing, and amounts between
---give a linear interpolation of the two.
---@return PV
function PV:replace_amplitudes( 
	amp_source, 
	amount
    ) end

---Subtracts the Amplitudes (Magnitudes) of bins in other with those in this
---@param other PV Source PV from which to draw amplitudes to subtract.
---@param amount? number|function Second, Frequency -> number. A scalar on the subtraction amount. This isn't clamped to [0,1].
---@return PV
function PV:subtract_amplitudes( 
	other, 
	amount
    ) end

-- /** 
-- 	*	\param shaper A function which takes each input MF and returns an MF to be written to the output.
-- 	*	\param use_shift_alignment 
-- 	*/
---Passes each MF through the shaper.
---@param shaper function Magnitude, Frequency -> Magnitude, Frequency.
---@param use_shift_alignment? boolean If enabled, for each input MF the difference between the bin the MF came from and the bin its
---frequency would naturally land in is found. That is the bin shift. After shaping, rather than placing data back into the bin it 
---came from, the natural bin for the shaped frequency is found, and the bin shift is added. This makes an effort to maintain the
---cohesion of partials when the shaper is shifting frequency information.
---@return PV
function PV:shape( 
	shaper, 
	use_shift_alignment 
    ) end

---At any given time, num_partials should return a number of bins, N, to retain. The N loudest bins are copied to the output. 
---All other output bins are 0 filled.
---@param num_partials number|function Second -> Bin
---@return PV
function PV:retain_n_loudest_partials( 
	num_partials 
    ) end

---At any given time, num_partials should return a number of bins, N, to remove. The N loudest bin positions are 0 filled in the output.
---All other bins are copied to the output.
---@param num_partials number|function Second -> Bin
---@return PV
function PV:remove_n_loudest_partials( 
	num_partials 
    ) end

---Each bin has its frequency copied to subsequent output bins with decaying magnitude until a bin with magnitude greater than
---the current decayed magnitude is read from the input. The frequency of the output at that point then changes to that of the louder bin.
---@param length number Because the decay is exponential, an output length is needed. 
---@param decay number|function Time, Frequency -> number. This returns the decay amount per second at every time/frequency input.
---For example, a constant decay of .5 applied to an impulse will lose half it's magnitude every second.
---@return PV
function PV:resonate( 
	length, 
	decay 
    ) end
