#include "Audio.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Audio/Audio.h>
#include <flan/PV/PV.h>
#include <flan/Wavetable.h>
#include <flan/Graph.h>

#include "LTMP.h"
#include "Usertypes.h"
#include "Utility.h"
#include "Function.h"
#include "Interpolators.h"
#include "VecMethods.h"

using namespace flan;

//============================================================================================================================================================
// Helpers
//============================================================================================================================================================

template<typename T>
static std::vector<std::shared_ptr<T>> vecToSharedPvec( std::vector<T> && vec )
    {
    std::vector<std::shared_ptr<T>> pvec;
    for( T & a : vec )
        pvec.emplace_back( std::make_shared<T>( std::move( a ) ) );
    return pvec;
    }

template<typename T>
static std::vector<const T *> sharedPvecToPvec( const std::vector<std::shared_ptr<T>> & sharedPvec )
    {
    std::vector<const T *> pvec;
    for( std::shared_ptr<T> a : sharedPvec )
        pvec.emplace_back( a.get() );
    return pvec;
    }



//============================================================================================================================================================
// Buffer Methods
//============================================================================================================================================================

struct F_Audio_get_num_channels { float operator()( pAudio a )
    { std::cout << "flan::Audio::get_num_channels";
    return a->get_num_channels(); } };

struct F_Audio_get_num_frames { float operator()( pAudio a )
    { std::cout << "flan::Audio::get_num_frames";
    return a->get_num_frames(); } };

struct F_Audio_get_sample_rate { float operator()( pAudio a )
    { std::cout << "flan::Audio::get_sample_rate";
    return a->get_sample_rate(); } };

struct F_Audio_get_length { float operator()( pAudio a )
    { std::cout << "flan::Audio::get_length";
    return a->get_length(); } };

struct F_Audio_get_max_amplitude { float operator()( pAudio a )
    { std::cout << "flan::Audio::get_max_amplitude";
    return a->get_max_sample_magnitude(); } };



//============================================================================================================================================================
// Construction
//============================================================================================================================================================

struct F_Audio_create_null { pAudio operator()()
    { std::cout << "flan::Audio::create_null";
    return std::make_shared<flan::Audio>( Audio::create_null() ); } };

struct F_Audio_load_from_file { pAudio operator()(
    const std::string & filename )
    { std::cout << "flan::Audio::load_from_file";
    return std::make_shared<flan::Audio>( Audio::load_from_file( filename ) ); } };

struct F_Audio_create_empty_with_length { pAudio operator()(
    Second length, 
    Channel num_channels = 1, 
    FrameRate sample_rate = 48000.0f  )
    { std::cout << "flan::Audio::create_empty_with_length";
    return std::make_shared<flan::Audio>( Audio::create_empty_with_length( length, num_channels, sample_rate ) ); } };

struct F_Audio_create_empty_with_frames { pAudio operator()(
    Frame num_frames,  
    Channel num_channels = 1, 
    FrameRate sample_rate = 48000.0f  )
    { std::cout << "flan::Audio::create_empty_with_frames";
    return std::make_shared<flan::Audio>( Audio::create_empty_with_frames( num_frames, num_channels, sample_rate ) ); } };



//============================================================================================================================================================
// Conversions
//============================================================================================================================================================

struct F_Audio_resample { pAudio operator()( pAudio a,
    flan::FrameRate b )
    { std::cout << "flan::Audio::resample";
    return std::make_shared<flan::Audio>( a->resample( b ) ); } };

struct F_Audio_convert_to_graph { pGraph operator()( pAudio a, 
    Interval b = Interval( 0, -1 ), 
    Pixel c = -1, 
    Pixel d = -1, 
    float e = 0 )
    { std::cout << "flan::Audio::convert_to_graph";
    return std::make_shared<flan::Graph>( a->convert_to_graph( b, c, d, flan::Graph::WaveformMode::Direct, e ) ); } };

struct F_Audio_save_to_bmp { void operator()( pAudio a, 
    const std::string & b, 
    Interval c = Interval( 0, -1 ), 
    Pixel d = -1, 
    Pixel e = -1 )
    { std::cout << "flan::Audio::save_to_bmp";
    a->save_to_bmp( b, c, d, e ); } };

struct F_Audio_convert_to_spectrum_graph { pGraph operator()( pAudio a,
    Pixel width = -1, 
    Pixel height = -1,
    Frame smoothing_frames = 128 )
    { std::cout << "flan::Audio::convert_to_spectrum_graph";
    return std::make_shared<flan::Graph>( a->convert_to_spectrum_graph( width, height, smoothing_frames ) ); } };

struct F_Audio_save_spectrum_to_bmp { void operator()( pAudio a,
    const std::string & filename,
    Pixel width = -1, 
    Pixel height = -1,
    Frame smoothing_frames = 128 )
    { std::cout << "flan::Audio::save_spectrum_to_bmp";
    a->save_spectrum_to_bmp( filename, width, height, smoothing_frames ); } };

struct F_Audio_convert_to_PV { pPV operator()( pAudio a, 
    Frame b = 2048, 
    Frame c = 128, 
    Frame d = 4096 )
    { std::cout << "flan::Audio::convert_to_PV";
    return std::make_shared<flan::PV>( a->convert_to_PV( b, c, d ) ); } };

struct F_Audio_convert_to_ms_PV { pPV operator()( pAudio a,
    Frame window_size = 2048, 
    Frame hop = 128, 
    Frame dft_size = 4096 )
    { std::cout << "flan::Audio::convert_to_ms_PV";
    return std::make_shared<flan::PV>( a->convert_to_ms_PV( window_size, hop, dft_size ) ); } };

struct F_Audio_convert_to_PV_selector { pPV operator()( pAudio a,
    Frame window_size = 2048, 
    Frame hop = 128, 
    Frame dft_size = 4096 )
    { 
    if( a->get_num_channels() == 2 )
        return F_Audio_convert_to_ms_PV()( a, window_size, hop, dft_size );
    else 
        return F_Audio_convert_to_PV()( a, window_size, hop, dft_size );
    } };

struct F_Audio_convert_to_mid_side { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::convert_to_mid_side";
    return std::make_shared<flan::Audio>( a->convert_to_mid_side() ); } };

struct F_Audio_convert_to_left_right { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::convert_to_left_right";
    return std::make_shared<flan::Audio>( a->convert_to_left_right() ); } };

struct F_Audio_convert_to_stereo { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::convert_to_stereo";
    return std::make_shared<flan::Audio>( a->convert_to_stereo() ); } };

struct F_Audio_convert_to_mono { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::convert_to_mono";
    return std::make_shared<flan::Audio>( a->convert_to_mono() ); } };

struct F_Audio_convert_to_function { pFunc1x1 operator()( pAudio a )
    { std::cout << "flan::Audio::convert_to_function";
    return std::make_shared<Func1x1>( a->convert_to_function() ); } };

struct F_Audio_convert_to_wavetable { pWavetable operator()( pAudio a )
    { 
    std::cout << "flan::Audio::convert_to_wavetable";
    return std::make_shared<Wavetable>( *a ); 
    } };



//============================================================================================================================================================
// Channels
//============================================================================================================================================================

struct F_Audio_split_channels { AudioVec operator()( pAudio a )
    { std::cout << "flan::Audio::split_channels";
    return vecToSharedPvec( a->split_channels() ); } };

struct F_Audio_combine_channels { pAudio operator()( AudioVec a )
    { std::cout << "flan::Audio::combine_channels";
    return std::make_shared<flan::Audio>( Audio::combine_channels( sharedPvecToPvec( a ) ) ); } };



//============================================================================================================================================================
// Information
//============================================================================================================================================================

struct F_Audio_get_total_energy { std::vector<float> operator()( pAudio a )
    { std::cout << "flan::Audio::get_total_energy";
    return a->get_total_energy(); } };

struct F_Audio_get_energy_difference { std::vector<float> operator()( pAudio a,
    pAudio b )
    { std::cout << "flan::Audio::get_energy_difference";
    return a->get_energy_difference( *b ); } };

struct F_Audio_get_local_wavelength { float operator()( pAudio a, 
    Channel b, 
    Frame c, 
    Frame d = 2048 )
    { std::cout << "flan::Audio::get_local_wavelength";
    return a->get_local_wavelength( b, c, d ); } };

struct F_Audio_get_local_wavelengths { std::vector<float> operator()( pAudio a, 
    Channel b = 0, 
    Frame c = 0, 
    Frame d = 2048, 
    Frame e = 128 )
    { std::cout << "flan::Audio::get_local_wavelengths";
    return a->get_local_wavelengths( b, c, d, e ); } };

struct F_Audio_get_average_wavelength { float operator()( pAudio a, 
    Channel b = 0, 
    float c = 0, 
    float d = -1, 
    Frame e = 0, 
    Frame f = -1, 
    Frame g = 2048, 
    Frame h = 128 )
    { std::cout << "flan::Audio::get_average_wavelength";
    return a->get_average_wavelength( b, c, d, e, f, g, h ); } };

struct F_Audio_get_local_frequency { float operator()( pAudio a, 
    Channel b, 
    Frame c = 0, 
    Frame d = 2048 )
    { std::cout << "flan::Audio::get_local_frequency";
    return a->get_local_frequency( b, c, d ); } };

struct F_Audio_get_local_frequencies { std::vector<Frequency> operator()( pAudio a, 
    Channel b = 0, 
    Frame c = 0, 
    Frame d = -1, 
    Frame e = 2048, 
    Frame f = 128 )
    { std::cout << "flan::Audio::get_local_frequencies";
    return a->get_local_frequencies( b, c, d, e, f ); } };

struct F_Audio_get_amplitude_envelope { pFunc1x1 operator()( pAudio a,
    Second window_width = 0.1f )
    { std::cout << "flan::Audio::get_amplitude_envelope";
    return std::make_shared<Func1x1>( a->get_amplitude_envelope( window_width ) ); } };

struct F_Audio_get_frequency_envelope { pFunc1x1 operator()( pAudio a )
    { std::cout << "flan::Audio::get_frequency_envelope";
    return std::make_shared<Func1x1>( a->get_frequency_envelope() ); } };



//============================================================================================================================================================
// Temporal
//============================================================================================================================================================

struct F_Audio_modify_boundaries { pAudio operator()( pAudio a,
    Second start, 
    Second end  )
    { std::cout << "flan::Audio::modify_boundaries";
    return std::make_shared<flan::Audio>( a->modify_boundaries( start, end ) ); } };

struct F_Audio_remove_edge_silence { pAudio operator()( pAudio a,
    Amplitude non_silent_level,
    Second fade_time = 0  )
    { std::cout << "flan::Audio::remove_edge_silence";
    return std::make_shared<flan::Audio>( a->remove_edge_silence( non_silent_level, fade_time ) ); } };

struct F_Audio_get_loud_chunks { AudioVec operator()( pAudio a,
    Amplitude non_silent_level,
    Second minimum_gap, 
    Second fade_time = 0 )
    { std::cout << "flan::Audio::get_loud_chunks";
    return vecToSharedPvec( a->get_loud_chunks( non_silent_level, minimum_gap, fade_time ) ); } };

struct F_Audio_remove_silence { pAudio operator()( pAudio a,
    Amplitude non_silent_level,
    Second minimum_gap, 
    Second fade_time = 0 )
    { std::cout << "flan::Audio::remove_silence";
    return std::make_shared<flan::Audio>( a->remove_silence( non_silent_level, minimum_gap, fade_time ) ); } };

struct F_Audio_reverse { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::reverse";
    return std::make_shared<flan::Audio>( a->reverse() ); } };

struct F_Audio_cut { pAudio operator()( pAudio a, 
    flan::Second b, 
    flan::Second c, 
    flan::Second d = 0, 
    flan::Second e = 0 )
    { std::cout << "flan::Audio::cut";
    return std::make_shared<flan::Audio>( a->cut( b, c, d, e ) ); } };

struct F_Audio_cut_frames { pAudio operator()( pAudio a, 
    Frame b, 
    Frame c, 
    Frame d = 0, 
    Frame e = 0 )
    { std::cout << "flan::Audio::cut_frames";
    return std::make_shared<flan::Audio>( a->cut_frames( b, c, d, e ) ); } };

struct F_Audio_repitch { pAudio operator()( pAudio a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 1 ), 
    Second c = .001 )
    { std::cout << "flan::Audio::repitch";
    return std::make_shared<flan::Audio>( a->repitch( *b, c, flan::Audio::WDLResampleType::Sinc ) ); } };

struct F_Audio_iterate { pAudio operator()( pAudio a, 
    uint32_t b, 
    Second fade = 0,
    pAudioMod c = std::make_shared<flan::AudioMod>(), 
    Fool d = false )
    { std::cout << "flan::Audio::iterate";
    return std::make_shared<flan::Audio>( a->iterate( b, fade, *c, d.b ) ); } };

struct F_Audio_delay { pAudio operator()( pAudio a, 
    flan::Second b, 
    pFunc1x1 c, 
    pFunc1x1 d, 
    pAudioMod e = std::make_shared<flan::AudioMod>() )
    { std::cout << "flan::Audio::delay";
    return std::make_shared<flan::Audio>( a->delay( b, *c, *d, *e ) ); } };

struct F_Audio_split_at_times { AudioVec operator()( pAudio a, 
    std::vector<flan::Second> b, 
    flan::Second c = 0 )
    { std::cout << "flan::Audio::split_at_times";
    return vecToSharedPvec( a->split_at_times( b, c ) ); } };

struct F_Audio_split_with_lengths { AudioVec operator()( pAudio a, 
    std::vector<flan::Second> b, 
    flan::Second c = 0 )
    { std::cout << "flan::Audio::split_with_lengths";
    return vecToSharedPvec( a->split_with_lengths( b, c ) ); } };

struct F_Audio_split_with_equal_lengths { AudioVec operator()( pAudio a, 
    flan::Second b, 
    flan::Second c = 0 )
    { std::cout << "flan::Audio::split_with_equal_lengths";
    return vecToSharedPvec( a->split_with_equal_lengths( b, c ) ); } };

struct F_Audio_random_chunks { pAudio operator()( pAudio a, 
    flan::Second b,
    pFunc1x1 c, 
    pFunc1x1 d = std::make_shared<Func1x1>( 0 ),
    pAudioMod e = std::make_shared<flan::AudioMod>() )
    { std::cout << "flan::Audio::random_chunks";
    return std::make_shared<flan::Audio>( a->random_chunks( b, *c, *d, *e ) ); } };

struct F_Audio_rearrange { pAudio operator()( pAudio a, 
    Second slice_length,
    Second fade_time = 0 )
    { std::cout << "flan::Audio::rearrange";
    return std::make_shared<flan::Audio>( a->rearrange( slice_length, fade_time ) ); } };



//============================================================================================================================================================
// Volume
//============================================================================================================================================================

struct F_Audio_modify_volume { pAudio operator()( pAudio a, 
    pFunc1x1 b )
    { std::cout << "flan::Audio::modify_volume";
    return std::make_shared<flan::Audio>( a->modify_volume( *b ) ); } };

struct F_Audio_modify_volume_in_place { void operator()( pAudio a, 
    pFunc1x1 b )
    { std::cout << "flan::Audio::modify_volume_in_place";
    a->modify_volume_in_place( *b ); } };

struct F_Audio_set_volume { pAudio operator()( pAudio a, 
    pFunc1x1 b )
    { std::cout << "flan::Audio::set_volume";
    return std::make_shared<flan::Audio>( a->set_volume( *b ) ); } };

struct F_Audio_ring_modulate { pAudio operator()( pAudio a, 
    pAudio b )
    { std::cout << "flan::Audio::ring_modulate";
    return std::make_shared<flan::Audio>( a->ring_modulate( *b ) ); } };

struct F_Audio_fade { pAudio operator()( pAudio a, 
    flan::Second b = 16.0f/48000.0f, 
    flan::Second c = 16.0f/48000.0f, 
    InterpolatorIndex d = InterpolatorIndex::sqrt )
    { std::cout << "flan::Audio::fade";
    return std::make_shared<flan::Audio>( a->fade( b, c, index2interpolator( d ) ) ); } };

struct F_Audio_apply_adsr { pAudio operator()( pAudio a, 
    Second b,
    Second c,
    Second d = 0,
    Second e = 0,
    Amplitude f = 0,
    float g = 1,
    float h = 1,
    float i = 1 )
    { 
    // This isn't a real flan function but nobody needs to know that.
    std::cout << "flan::Audio::apply_adsr_envelope";
    return std::make_shared<flan::Audio>( a->apply_adsr_envelope( b, c, d, e, f, g, h, i ) ); 
    } };

struct F_Audio_apply_ar { pAudio operator()( pAudio a, 
    Second b,
    Second e,
    float h = 1,
    float i = 1 )
    { 
    // This isn't a real flan function but nobody needs to know that.
    std::cout << "flan::Audio::apply_ar_envelope";
    return std::make_shared<flan::Audio>( a->apply_ar_envelope( b, e, h, i ) ); 
    } };

struct F_Audio_fade_in_place { void operator()( pAudio a, 
    flan::Second b = 16.0f/48000.0f, 
    flan::Second c = 16.0f/48000.0f, 
    InterpolatorIndex d = InterpolatorIndex::sqrt )
    { std::cout << "flan::Audio::fade_in_place";
    a->fade_in_place( b, c, index2interpolator( d ) ); } };

struct F_Audio_fade_frames { pAudio operator()( pAudio a, 
    Frame b = 16, 
    Frame c = 16, 
    InterpolatorIndex d = InterpolatorIndex::sqrt )
    { std::cout << "flan::Audio::fade_frames";
    return std::make_shared<flan::Audio>( a->fade_frames( b, c, index2interpolator( d ) ) ); } };

struct F_Audio_fade_frames_in_place { void operator()( pAudio a, 
    Frame b = 16, 
    Frame c = 16, 
    InterpolatorIndex d = InterpolatorIndex::sqrt )
    { std::cout << "flan::Audio::fade_frames_in_place";
    a->fade_frames_in_place( b, c, index2interpolator( d ) ); } };

struct F_Audio_invert_phase { pAudio operator()( pAudio a )
    { std::cout << "flan::Audio::invert_phase";
    return std::make_shared<flan::Audio>( a->invert_phase() ); } };

struct F_Audio_waveshape { pAudio operator()( pAudio a, 
    pFunc2x1 b,
    uint16_t c = 4 )
    { std::cout << "flan::Audio::waveshape";
    return std::make_shared<flan::Audio>( a->waveshape( wrapFuncAxB<std::pair<Second, Sample>, Sample>( b ), c ) ); } };

struct F_Audio_add_moisture { pAudio operator()( pAudio a,
    pFunc1x1 amount = std::make_shared<Func1x1>( .5f ),
    pFunc1x1 frequency = std::make_shared<Func1x1>( 96 ),
    pFunc1x1 skew = std::make_shared<Func1x1>( 4 ),
    pFunc1x1 waveform = std::make_shared<Func1x1>( flan::waveforms::sine ) )
    { std::cout << "flan::Audio::add_moisture";
    return std::make_shared<flan::Audio>( a->add_moisture( *amount, *frequency, *skew, *waveform ) ); } };

struct F_Audio_compress { pAudio operator()( pAudio a,
    pFunc1x1 threshold, 
    pFunc1x1 compression_ratio = std::make_shared<Func1x1>( 3.0f ), 
    pFunc1x1 attack = std::make_shared<Func1x1>( 5.0f / 1000.0f ), 
    pFunc1x1 release = std::make_shared<Func1x1>( 100.0f / 1000.0f ), 
    pFunc1x1 knee_width = std::make_shared<Func1x1>( Decibel( 0 ) ), 
    pAudio sidechain_source = nullptr  )
    { std::cout << "flan::Audio::compress";
    return std::make_shared<flan::Audio>( a->compress( *threshold, *compression_ratio, *attack, *release, *knee_width, sidechain_source.get() ) ); } };



//============================================================================================================================================================
// Spatial
//============================================================================================================================================================

struct F_Audio_stereo_spatialize { pAudio operator()( pAudio a,
    pFunc1x2 position,
    Meter head_width = 0.18f,
    pFunc1x1 speed_limit = std::make_shared<Func1x1>( std::numeric_limits<float>::max() ) )
    { std::cout << "flan::Audio::stereo_spatialize";
    return std::make_shared<flan::Audio>( a->stereo_spatialize( *position, head_width, *speed_limit ) ); } };

struct F_Audio_pan { pAudio operator()( pAudio a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 0 ) )
    { std::cout << "flan::Audio::pan";
    return std::make_shared<flan::Audio>( a->pan( *b ) ); } };

struct F_Audio_pan_in_place { void operator()( pAudio a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 0 ) )
    { std::cout << "flan::Audio::pan_in_place";
    a->pan_in_place( *b ); } };

struct F_Audio_widen { pAudio operator()( pAudio a, 
    pFunc1x1 b = std::make_shared<Func1x1>( 1 ) )
    { std::cout << "flan::Audio::widen";
    return std::make_shared<flan::Audio>( a->widen( *b ) ); } };



//============================================================================================================================================================
// Filters
//============================================================================================================================================================

/* 1 Pole ============================ */

struct F_Audio_filter_1pole_lowpass { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_1pole_lowpass";
    return std::make_shared<flan::Audio>( a->filter_1pole_lowpass( *cutoff, order ) ); } };

struct F_Audio_filter_1pole_highpass { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_1pole_highpass";
    return std::make_shared<flan::Audio>( a->filter_1pole_highpass( *cutoff, order ) ); } };

struct F_Audio_filter_1pole_split { AudioVec operator()( pAudio a,
    pFunc1x1 cutoff,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_1pole_split";
    return vecToSharedPvec( a->filter_1pole_split( *cutoff, order ) ); } };

struct F_Audio_filter_1pole_lowshelf { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 gain,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_1pole_lowshelf";
    return std::make_shared<flan::Audio>( a->filter_1pole_lowshelf( *cutoff, *gain, order ) ); } };

struct F_Audio_filter_1pole_highshelf { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 gain,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_1pole_highshelf";
    return std::make_shared<flan::Audio>( a->filter_1pole_highshelf( *cutoff, *gain, order ) ); } };

// struct F_Audio_filter_1pole_repeat { pAudio operator()( pAudio a,
//     pFunc1x1 cutoff,
//     const uint16_t repeats )
//     { std::cout << "flan::Audio::filter_1pole_repeat";
//     return std::make_shared<flan::Audio>( a->filter_1pole_repeat( *cutoff, repeats ) ); } };

/* 2 Pole ============================ */

struct F_Audio_filter_2pole_lowpass { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_lowpass";
    return std::make_shared<flan::Audio>( a->filter_2pole_lowpass( *cutoff, *damping, order ) ); } };

struct F_Audio_filter_2pole_bandpass { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_bandpass";
    return std::make_shared<flan::Audio>( a->filter_2pole_bandpass( *cutoff, *damping, order ) ); } };

struct F_Audio_filter_2pole_highpass { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_highpass";
    return std::make_shared<flan::Audio>( a->filter_2pole_highpass( *cutoff, *damping, order ) ); } };

struct F_Audio_filter_2pole_notch { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_notch";
    return std::make_shared<flan::Audio>( a->filter_2pole_notch( *cutoff, *damping, order ) ); } };

struct F_Audio_filter_2pole_lowshelf { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    pFunc1x1 gain,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_lowshelf";
    return std::make_shared<flan::Audio>( a->filter_2pole_lowshelf( *cutoff, *damping, *gain, order ) ); } };

struct F_Audio_filter_2pole_bandshelf { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    pFunc1x1 gain,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_bandshelf";
    return std::make_shared<flan::Audio>( a->filter_2pole_bandshelf( *cutoff, *damping, *gain, order ) ); } };

struct F_Audio_filter_2pole_highshelf { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    pFunc1x1 gain,
    uint16_t order = 1 )
    { std::cout << "flan::Audio::filter_2pole_highshelf";
    return std::make_shared<flan::Audio>( a->filter_2pole_highshelf( *cutoff, *damping, *gain, order ) ); } };

/* Other filters ==================== */

struct F_Audio_filter_1pole_multinotch { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 wet_dry = std::make_shared<Func1x1>( .5 ),
    uint16_t order = 1,
    Fool invert = false )
    { std::cout << "flan::Audio::filter_1pole_multinotch";
    return std::make_shared<flan::Audio>( a->filter_1pole_multinotch( *cutoff, *wet_dry, order, invert.b ) ); } };

struct F_Audio_filter_2pole_multinotch { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 damping,
    pFunc1x1 wet_dry = std::make_shared<Func1x1>( .5 ),
    uint16_t order = 1,
    Fool invert = false )
    { std::cout << "flan::Audio::filter_2pole_multinotch";
    return std::make_shared<flan::Audio>( a->filter_2pole_multinotch( *cutoff, *damping, *wet_dry, order, invert.b ) ); } };

struct F_Audio_filter_comb { pAudio operator()( pAudio a,
    pFunc1x1 cutoff,
    pFunc1x1 feedback = std::make_shared<Func1x1>( 0 ),
    pFunc1x1 wet_dry = std::make_shared<Func1x1>( .5 ),
    Fool invert = false )
    { std::cout << "flan::Audio::filter_comb";
    return std::make_shared<flan::Audio>( a->filter_comb( *cutoff, *feedback, *wet_dry, invert.b ) ); } };



//============================================================================================================================================================
// Combination
//============================================================================================================================================================
 
// struct F_Audio_mix_variable_gain { pAudio operator()( AudioVec a, 
//     std::vector<flan::Second> start_times = std::vector<flan::Second>(),
//     Func1x1Vec gains = Func1x1Vec() )
//     { 
//     Audio out = Audio::mix_variable_gain( 
//         sharedPvecToPvec( a ), 
//         start_times, 
//         sharedPvecToPvec<Func1x1>( gains ) );
//     std::cout << "flan::Audio::mix_variable_gain";
//     return std::make_shared<flan::Audio>( std::move( out ) ); 
//     } };

struct F_Audio_mix { pAudio operator()( AudioVec a, 
    std::vector<float> start_times = std::vector<float>(), 
    std::vector<float> gains = std::vector<float>() )
    { std::cout << "flan::Audio::mix";
    return std::make_shared<flan::Audio>( Audio::mix( sharedPvecToPvec( a ), start_times, gains ) ); } };

struct F_Audio_join { pAudio operator()( AudioVec a, 
    flan::Second b = 0 )
    { std::cout << "flan::Audio::join";
    return std::make_shared<flan::Audio>( Audio::join( sharedPvecToPvec( a ), b ) ); } };

struct F_Audio_select { pAudio operator()( AudioVec a, 
    pFunc1x1 b = 0, 
    std::vector<float> c = std::vector<float>() )
    { std::cout << "flan::Audio::select";
    return std::make_shared<flan::Audio>( Audio::select( sharedPvecToPvec( a ), *b, c ) ); } };

struct F_Audio_convolve { pAudio operator()( pAudio a, 
    pAudio b = std::make_shared<flan::Audio>() )
    { std::cout << "flan::Audio::convolve";
    return std::make_shared<flan::Audio>( a->convolve( *b ) ); } };



//============================================================================================================================================================
// Synthesis
//============================================================================================================================================================

struct F_Audio_synthesize_waveform { pAudio operator()(
    pFunc1x1 waveform, 
    Second length, 
    pFunc1x1 freq, 
    FrameRate sample_rate = 48000, 
    int oversample = 16 )
    { std::cout << "flan::Audio::synthesize_waveform";
    return std::make_shared<flan::Audio>( Audio::synthesize_waveform( *waveform, length, *freq, sample_rate, oversample ) ); } };

struct F_Audio_synthesize_white_noise { pAudio operator()(
    Second length,
    FrameRate sample_rate = 48000,
    int oversample = 16
    )
    { std::cout << "flan::Audio::synthesize_white_noise";
    return std::make_shared<flan::Audio>( Audio::synthesize_white_noise( length, sample_rate, oversample ) ); } };

struct F_Audio_synthesize_pink_noise { pAudio operator()(
    Second length,
    FrameRate sample_rate = 48000,
    int num_rows = 128
    )
    { std::cout << "flan::Audio::synthesize_pink_noise";
    return std::make_shared<flan::Audio>( Audio::synthesize_pink_noise( length, sample_rate, num_rows ) ); } };

struct F_Audio_synthesize_spectrum { pAudio operator()(
    flan::Second length, 
    pFunc1x1 freq,
    flan::Channel channels = 2,
    pFunc1x1 spread = std::make_shared<Func1x1>( []( float h ){ return h; } ),
    pFunc1x1 harmonic_scale = std::make_shared<Func1x1>( []( float h ){ return 1.0f/std::sqrt(h); } ),
    pFunc1x1 peak_distribution = std::make_shared<Func1x1>( []( float x )
        { 
        return 1.0f / std::sqrt(pi2) * std::exp( -0.5f * std::pow( x, 2.0f ) );
        } ),
    int fundamental_power = 8,
    int spectrum_size_power = 20
    )
    { 
    std::cout << "flan::Audio::synthesize_spectrum";
    return std::make_shared<flan::Audio>( Audio::synthesize_spectrum( 
        length, 
        *freq, 
        wrapFuncAxB<Harmonic, Frequency>( spread ), 
        wrapFuncAxB<Harmonic, Amplitude>( harmonic_scale ), 
        *peak_distribution, 
        fundamental_power,
        spectrum_size_power,
        channels ) ); 
    } };

struct F_Audio_synthesize_impulse { pAudio operator()(
    Frequency base_freq,
    Harmonic num_harmonics = std::numeric_limits<Harmonic>::max(),
    float chroma = 1.0f,
    FrameRate sample_rate = 48000.0f )
    { std::cout << "flan::Audio::synthesize_impulse";
    return std::make_shared<flan::Audio>( Audio::synthesize_impulse( base_freq, num_harmonics, chroma, sample_rate ) ); } };
    
// Grain Controllers ===================================================================================================================

struct F_Audio_synthesize_grains { pAudio operator()(
    Second length,
    pFunc1x1 grains_per_second,
    pFunc1x1 time_scatter,
    pGrainSource grain_source,
    FrameRate sample_rate = 48000 )
    { std::cout << "flan::Audio::synthesize_grains";
    return std::make_shared<flan::Audio>( Audio::synthesize_grains( length, *grains_per_second, *time_scatter, *grain_source, sample_rate ) ); } };

// struct F_Audio_synthesize_grains_repeat { pAudio operator()( pAudio a,
//     Second length,
//     pFunc1x1 grains_per_second,
//     pFunc1x1 time_scatter,
//     pFunc1x1 gain = std::make_shared<Func1x1>( 1 ) )
//     { std::cout << "flan::Audio::synthesize_grains_repeat";
//     return std::make_shared<flan::Audio>( a->synthesize_grains_repeat( length, *grains_per_second, *time_scatter, *gain ) ); } };

struct F_Audio_texture { pAudio operator()( pAudio a,
    Second length, 
    pFunc1x1 grains_per_second, 
    pFunc1x1 time_scatter = std::make_shared<Func1x1>( 0 ),
    pAudioMod mod = std::make_shared<flan::AudioMod>(),
    Fool feedback = false )
    { 
    std::cout << "flan::Audio::texture";
        return std::make_shared<flan::Audio>( a->texture( length, *grains_per_second, *time_scatter, *mod, feedback.b ) ); 
    } };

// Grain Compositions ===================================================================================================================

struct F_Audio_synthesize_trainlets { pAudio operator()(
    Second a,
    pFunc1x1 b,
    pFunc1x1 c,
    pFunc1x2 d,
    pFunc1x1 e,
    pFunc1x1 f,
    pFunc1x1 g,
    pFunc1x1 h = std::make_shared<Func1x1>( 48000*10 ), // I can't remember why I didn't just set this to max, but I don't care to find out.
    pFunc1x1 i = std::make_shared<Func1x1>( 1 ),
    pFunc1x1 j = std::make_shared<Func1x1>( 32 ),
    FrameRate k = 48000 )
    { std::cout << "flan::Audio::synthesize_trainlets";
    return std::make_shared<flan::Audio>( Audio::synthesize_trainlets( a, *b, *c, *d, *e, *f, *g, wrapFuncAxB<Second, Harmonic>( h ), *i, *j, k ) ); } };

struct F_Audio_granulate { pAudio operator()( pAudio a,
    Second length, 
    pFunc1x1 grains_per_second, 
    pFunc1x1 time_scatter, 
    pFunc1x1 time_selection, 
    pFunc1x1 grain_length,
    Second fade = 0,
    pAudioMod mod = std::make_shared<flan::AudioMod>() )
    { std::cout << "flan::Audio::texture_granulate";
    return std::make_shared<flan::Audio>( a->granulate( length, *grains_per_second, *time_scatter, *time_selection, *grain_length, fade, *mod ) ); } };

struct F_Audio_psola { pAudio operator()( pAudio a,
    Second length, 
    pFunc1x1 time_selection,
    pAudioMod mod = std::make_shared<flan::AudioMod>() )
    { 
    std::cout << "flan::Audio::texture_psola";
    auto out = std::make_shared<flan::Audio>( a->psola( length, *time_selection, *mod ) ); 
    if( length > 0 && !a->is_null() && out->is_null() )
        std::cout << "    Psola returned no data, which indicates the input had no detectable frequency envelope.";
    return out;
    } };



//============================================================================================================================================================
// Helpers - These are not part of flan, they are helpers for ltmp processing
//============================================================================================================================================================

// struct F_Audio_vec_repeat_for_each { AudioVec operator()( AudioVec a,
//     int n,
//     pAudioMod mod,
//     Fool b = false )
//     { 
//     std::cout << "falter::Audio::vec_repeat_for_each";

//     AudioVec out;

//     if( b.b )
//         for( int input_index = 0; input_index < a.size(); ++input_index )
//             for( int repeat_index = 0; repeat_index < n; ++repeat_index )
//                 {
//                 Audio audio = a[input_index]->copy();
//                 (*mod)( audio, repeat_index+1 ); 
//                 out.push_back( std::make_shared<flan::Audio>( std::move( audio ) ) );
//                 }
//     else
//         for( int repeat_index = 0; repeat_index < n; ++repeat_index )
//             for( int input_index = 0; input_index < a.size(); ++input_index )
//                 {
//                 Audio audio = a[input_index]->copy();
//                 (*mod)( audio, repeat_index+1 ); 
//                 out.push_back( std::make_shared<flan::Audio>( std::move( audio ) ) );
//                 }

//     return out;
//     } };

struct F_Audio_vec_texture { pAudio operator()( AudioVec a,
    Second length, 
    pFunc1x1 grains_per_second, 
    pFunc1x1 time_scatter = std::make_shared<Func1x1>( 0 ), 
    pAudioMod mod = std::make_shared<flan::AudioMod>() )
    { 
    std::cout << "flan::Audio::vec_texture";

    std::random_device rng;
    std::mt19937 gen( rng() );
    std::uniform_int_distribution<int> dist(0, a.size()-1);

    return std::make_shared<flan::Audio>( Audio::synthesize_grains( length, *grains_per_second, *time_scatter, 
        flan::Function<Second, Audio>( [&]( Second t )
            { 
            const size_t selection = dist( gen );
            flan::Audio out = a[selection]->copy();
            (*mod)( out, t );
            return std::move( out );
            }, ExecutionPolicy::Linear_Sequenced ), 
        48000 ) );
    } };
    
struct F_Audio_apply_to_section { pAudio operator()( pAudio a,
    Second start_time,
    Second length,
    pAudioMod mod,
    Second fade = 0.0f,
    Fool keep_timing = true )
    {
    /*
    I'm sure the branches in this function could be simplified, but I don't think it's worth doing.
    The branches handle cases where the start_time/end_time lay outside the bounds of the input.
    */

    length = std::max( 0.0f, length );
    const Second end_time = start_time + length;
    if( start_time >= a->get_length() || end_time <= 0.0f ) return a;

    std::vector<flan::Audio> a_split;
    std::vector<Second> mix_times;

    if( start_time <= 0 )
        {  
        if( end_time >= a->get_length() )
            {
            a_split.push_back( a->copy() );
            mix_times = {0};
            }
        else
            {
            a_split = a->split_at_times( { start_time+length }, fade );
            mix_times = {0, end_time - fade};
            }

        (*mod)( a_split[0], 0 );
        }
    else
        {
        if( end_time >= a->get_length() )
            {
            a_split = a->split_at_times( { start_time }, fade );
            mix_times = {0, start_time - fade};
            }
        else
            {
            a_split = a->split_at_times( { start_time, end_time }, fade );
            mix_times = {0, start_time - fade, end_time - 2*fade};
            }

        (*mod)( a_split[1], start_time );
        }

    if( keep_timing.b ) return std::make_shared<flan::Audio>( Audio::mix( a_split, mix_times ) );
    else return std::make_shared<flan::Audio>( Audio::join( a_split, -fade ) );
    } };

struct F_Audio_dry_wet { pAudio operator()( pAudio a,
    pFunc1x1 ratio,
    pAudioMod mod )
    {
    Audio b = a->copy();
    (*mod)( b, 0 );
    Func1x1 f1 = [&]( float t ){ const float ratio_c = std::clamp( (*ratio)(t), 0.0f, 1.0f ); return std::sqrt( 1.0f - ratio_c ); };
    Func1x1 f2 = [&]( float t ){ const float ratio_c = std::clamp( (*ratio)(t), 0.0f, 1.0f ); return std::sqrt( ratio_c ); };
    return std::make_shared<flan::Audio>( Audio::mix( { a.get(), &b }, {0, 0}, std::vector<const Func1x1*>{&f1, &f2} ) );
    } };


//============================================================================================================================================================
// Registration
//============================================================================================================================================================

void luaF_register_Audio( lua_State * L )
    {
    // Create Lua Audio type
    lua_newtable( L ); // Create global Audio table
        lua_newtable( L ); // Create Audio metatable
        lua_setmetatable( L, -2 );

        // Register all static flan::Audio methods as functions in global Lua Audio table. Vector methods are registered later as AudioVec methods
        lua_pushcclosure( L, luaF_LTMP<F_Audio_create_null,              0>, 0 ); lua_setfield( L, -2, "create_null"                ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_load_from_file,           1>, 0 ); lua_setfield( L, -2, "load_from_file"             ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_create_empty_with_length, 1>, 0 ); lua_setfield( L, -2, "create_empty_with_length"   ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_create_empty_with_frames, 1>, 0 ); lua_setfield( L, -2, "create_empty_with_frames"   );   
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_waveform,      3>, 0 ); lua_setfield( L, -2, "synthesize_waveform"        ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_white_noise,   1>, 0 ); lua_setfield( L, -2, "synthesize_white_noise"     ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_pink_noise,    1>, 0 ); lua_setfield( L, -2, "synthesize_pink_noise"      ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_spectrum,      2>, 0 ); lua_setfield( L, -2, "synthesize_spectrum"        ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_impulse,       1>, 0 ); lua_setfield( L, -2, "synthesize_impulse"         ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_grains,        4>, 0 ); lua_setfield( L, -2, "synthesize_grains"          ); 
        lua_pushcclosure( L, luaF_LTMP<F_Audio_synthesize_trainlets,     7>, 0 ); lua_setfield( L, -2, "synthesize_trainlets"       ); 

    lua_setglobal( L, luaF_getUsertypeName<pAudio>().c_str() );

	luaL_newmetatable( L, luaF_getUsertypeName<pAudio>().c_str() );
        lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );
        // Create Lua AudioVec type
        lua_register( L, luaF_getUsertypeName<AudioVec>().c_str(), luaF_Usertype_vec_new<pAudio> );
	    luaL_newmetatable( L, luaF_getUsertypeName<AudioVec>().c_str() );
            lua_pushvalue( L, -1 ); lua_setfield( L, -2, "__index" );

            // Buffer methods
            luaF_register_helper<F_Audio_get_num_channels,                      1>( L, "get_num_channels"                       );                  
            luaF_register_helper<F_Audio_get_num_frames,                        1>( L, "get_num_frames"                         );              
            luaF_register_helper<F_Audio_get_sample_rate,                       1>( L, "get_sample_rate"                        );              
            luaF_register_helper<F_Audio_get_length,                            1>( L, "get_length"                             );      
            luaF_register_helper<F_Audio_get_max_amplitude,                     1>( L, "get_max_amplitude"                      );      

            // Conversions
            luaF_register_helper<F_Audio_resample,                              2>( L, "resample"                               );
            luaF_register_helper<F_Audio_save_to_bmp,                           2>( L, "save_to_bmp"                            );
            luaF_register_helper<F_Audio_save_spectrum_to_bmp,                  2>( L, "save_spectrum_to_bmp"                   );                           
            luaF_register_helper<F_Audio_convert_to_PV,                         1>( L, "convert_to_PV"                          );
            luaF_register_helper<F_Audio_convert_to_ms_PV,                      1>( L, "convert_to_ms_PV"                       );          
            luaF_register_helper<F_Audio_convert_to_mid_side,                   1>( L, "convert_to_mid_side"                    );
            luaF_register_helper<F_Audio_convert_to_left_right,                 1>( L, "convert_to_left_right"                  );
            luaF_register_helper<F_Audio_convert_to_stereo,                     1>( L, "convert_to_stereo"                      );
            luaF_register_helper<F_Audio_convert_to_mono,                       1>( L, "convert_to_mono"                        );
            luaF_register_helper_select_return_type<F_Audio_convert_to_function, 1>( L, "convert_to_function"                   );
            luaF_register_helper<F_Audio_convert_to_wavetable,                   1>( L, "convert_to_wavetable"                  );

            // Channels
            luaF_register_helper<F_Audio_split_channels,                        1>( L, "split_channels"                         );        

            // Information
            luaF_register_helper<F_Audio_get_total_energy,                      1>( L, "get_total_energy"                       );
            luaF_register_helper<F_Audio_get_energy_difference,                 2>( L, "get_energy_difference"                  );                   
            luaF_register_helper<F_Audio_get_local_wavelength,                  3>( L, "get_local_wavelength"                   );
            luaF_register_helper<F_Audio_get_local_frequency,                   2>( L, "get_local_frequency"                    );
            luaF_register_helper_select_return_type<F_Audio_get_amplitude_envelope, 1>( L, "get_amplitude_envelope"             );
            luaF_register_helper_select_return_type<F_Audio_get_frequency_envelope, 1>( L, "get_frequency_envelope"             );

            // Temporal
            luaF_register_helper<F_Audio_modify_boundaries,                     3>( L, "modify_boundaries"                      );           
            luaF_register_helper<F_Audio_remove_edge_silence,                   2>( L, "remove_edge_silence"                    );                 
            luaF_register_helper<F_Audio_get_loud_chunks,                       3>( L, "get_loud_chunks"                        );                 
            luaF_register_helper<F_Audio_remove_silence,                        3>( L, "remove_silence"                         );        
            luaF_register_helper<F_Audio_reverse,                               1>( L, "reverse"                                );
            luaF_register_helper<F_Audio_cut,                                   3>( L, "cut"                                    );
            luaF_register_helper<F_Audio_cut_frames,                            3>( L, "cut_frames"                             );
            luaF_register_helper<F_Audio_repitch,                               2>( L, "repitch"                                );
            luaF_register_helper<F_Audio_iterate,                               2>( L, "iterate"                                );
            luaF_register_helper<F_Audio_delay,                                 4>( L, "delay"                                  );
            luaF_register_helper<F_Audio_split_at_times,                        2>( L, "split_at_times"                         );
            luaF_register_helper<F_Audio_split_with_lengths,                    2>( L, "split_with_lengths"                     );
            luaF_register_helper<F_Audio_split_with_equal_lengths,              2>( L, "split_with_equal_lengths"               );
            luaF_register_helper<F_Audio_rearrange,                             2>( L, "rearrange"                              );
            luaF_register_helper<F_Audio_random_chunks,                         3>( L, "random_chunks"                          );

            // Volume
            luaF_register_helper<F_Audio_modify_volume,                         2>( L, "modify_volume"                          );
            luaF_register_helper<F_Audio_set_volume,                            2>( L, "set_volume"                             );
            luaF_register_helper<F_Audio_ring_modulate,                         2>( L, "ring_modulate"                          );
            luaF_register_helper<F_Audio_apply_adsr,                            3>( L, "apply_adsr_envelope"                    );
            luaF_register_helper<F_Audio_apply_ar,                              3>( L, "apply_ar_envelope"                      );
            luaF_register_helper<F_Audio_fade,                                  1>( L, "fade"                                   );
            luaF_register_helper<F_Audio_fade_frames,                           1>( L, "fade_frames"                            );
            luaF_register_helper<F_Audio_invert_phase,                          1>( L, "invert_phase"                           );
            luaF_register_helper<F_Audio_waveshape,                             2>( L, "waveshape"                              );
            luaF_register_helper<F_Audio_add_moisture,                          1>( L, "add_moisture"                           );  
            luaF_register_helper<F_Audio_compress,                              2>( L, "compress"                               );

            // Spatial
            luaF_register_helper<F_Audio_stereo_spatialize,                     2>( L, "spatialize"                             );                               
            luaF_register_helper<F_Audio_pan,                                   2>( L, "pan"                                    );
            luaF_register_helper<F_Audio_widen,                                 2>( L, "widen"                                  );

            // Filters
            luaF_register_helper<F_Audio_filter_1pole_lowpass,                  2>( L, "filter_1pole_lowpass"                   );                       
            luaF_register_helper<F_Audio_filter_1pole_highpass,                 2>( L, "filter_1pole_highpass"                  );                        
            luaF_register_helper<F_Audio_filter_1pole_split,                    2>( L, "filter_1pole_split"                     );                     
            luaF_register_helper<F_Audio_filter_1pole_lowshelf,                 3>( L, "filter_1pole_lowshelf"                  );                        
            luaF_register_helper<F_Audio_filter_1pole_highshelf,                3>( L, "filter_1pole_highshelf"                 );                        
            luaF_register_helper<F_Audio_filter_2pole_lowpass,                  3>( L, "filter_2pole_lowpass"                   );                       
            luaF_register_helper<F_Audio_filter_2pole_bandpass,                 3>( L, "filter_2pole_bandpass"                  );                        
            luaF_register_helper<F_Audio_filter_2pole_highpass,                 3>( L, "filter_2pole_highpass"                  );                        
            luaF_register_helper<F_Audio_filter_2pole_notch,                    3>( L, "filter_2pole_notch"                     );                     
            luaF_register_helper<F_Audio_filter_2pole_lowshelf,                 4>( L, "filter_2pole_lowshelf"                  );                        
            luaF_register_helper<F_Audio_filter_2pole_bandshelf,                4>( L, "filter_2pole_bandshelf"                 );                             
            luaF_register_helper<F_Audio_filter_2pole_highshelf,                4>( L, "filter_2pole_highshelf"                 );                             
            luaF_register_helper<F_Audio_filter_1pole_multinotch,               2>( L, "filter_1pole_multinotch"                );                              
            luaF_register_helper<F_Audio_filter_2pole_multinotch,               3>( L, "filter_2pole_multinotch"                );                              
            luaF_register_helper<F_Audio_filter_comb,                           2>( L, "filter_comb"                            );      

            // Combination
            luaF_register_helper<F_Audio_convolve,                              2>( L, "convolve"                               );

            // Synthesis
            luaF_register_helper<F_Audio_texture,                               3>( L, "texture"                                );                                                 
            luaF_register_helper<F_Audio_granulate,                             6>( L, "texture_granulate"                      );                        
            luaF_register_helper<F_Audio_psola,                                 3>( L, "texture_psola"                          );          

            // Vector input methods. These can still act on single Audios, but usually are no-ops when doing so.
            luaF_register_helper<F_Audio_combine_channels,                      1>( L, "combine_channels"                       );  
            luaF_register_helper<F_Audio_mix,                                   1>( L, "mix"                                    ); 
            luaF_register_helper<F_Audio_join,                                  1>( L, "join"                                   ); 
            luaF_register_helper<F_Audio_select,                                2>( L, "select"                                 );  
            luaF_register_helper<F_Audio_vec_texture,                           3>( L, "vec_texture"                            );  

            luaF_register_helper<F_vec_group<Audio>,                            3>( L, "vec_group"                              ); 
            luaF_register_helper<F_vec_attach<Audio>,                           2>( L, "vec_attach"                             ); 
            luaF_register_helper<F_vec_repeat<Audio>,                           2>( L, "vec_repeat"                             );
            luaF_register_helper<F_vec_for_each<Audio>,                         2>( L, "vec_for_each"                           );
            luaF_register_helper<F_vec_filter<Audio>,                           2>( L, "vec_filter"                             ); 

            luaF_register_helper<F_Audio_apply_to_section,                      4>( L, "apply_to_section"                       ); 
            luaF_register_helper<F_Audio_dry_wet,                               3>( L, "dry_wet"                                ); 

	lua_pop( L, 2 );
    } 
       