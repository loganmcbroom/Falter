---@meta

---@class Wavetable
Wavetable = {}

-- //============================================================================================================================================================
-- // Constructors
-- //============================================================================================================================================================

---Create a Wavetable from an Audio.
---@param audio Audio
---@param snap_mode? integer 0 -> Don't snap, 1 -> Snap to 0 crossings, 2 -> Snap to matching start/end levels.
---@param pitch_mode? integer 0 -> Don't estimate pitch, 1 -> Estimate pitch over time, 2 -> Estimate single global pitch of source.
---@param snap_ratio? number The proportion of the expected frame jump to search for frame snapping.
---@param fixed_frame_size? integer When pitch_mode is None, this is used in place of pitch deduction.
---@return Wavetable
function Wavetable.create_from_audio(
	audio,
	snap_mode,
	pitch_mode,
	snap_ratio,
	fixed_frame_size
	) end

---Create a Wavetable from a function.
---@param func function Second -> Amplitude. The function to sample. Each wave will be sampled on the interval [k,k+1), starting at 0.
---@param num_waves integer The number of waveforms to sample.
---@return Wavetable
function Wavetable.create_from_function(
	func,
	num_waves
	) end



-- //============================================================================================================================================================
-- // Synthesis
-- //============================================================================================================================================================

---Synthesize Audio from the Wavetable.
---@param length number
---@param freq number|function Second -> Frequency
---@param index? number|function Second -> number
---@param granularity? number
---@return Audio
function Wavetable:synthesize (
	length,
	freq,
	index,
	granularity
	) end

---Warning: impure function. Fade the edges of target waveforms towards zero.
---@param fade_frames? integer
---@param target? integer Defaults to targeting all.
---@return nil
function Wavetable:add_fades_in_place( 
	fade_frames,
	target
	) end

---Warning: impure function. Fade the edges of target waveforms towards the halfway point between the edge heights.
---@param fade_frames integer
---@param target integer Defaults to targeting all.
---@return nil
function Wavetable:remove_jumps_in_place( 
	fade_frames,
	target
	) end

---Warning: impure function. Remove the DC offset from target waveforms.
---@param target integer Defaults to targeting all.
---@return nil
function Wavetable:remove_dc_in_place( 
	target
	) end