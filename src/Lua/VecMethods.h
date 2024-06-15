/* 
This file contains all the templates for lua vec methods that are used by both Audio and PV
*/

#pragma once

#include "Types.h"

template<typename T>
struct F_vec_group { VecP<T> operator()( VecP<T> a, int group_size, pVecGroupFunc<T> func ); };

template<typename T>
struct F_vec_attach { VecP<T> operator()( VecP<T> as, VecP<T> b ); };

template<typename T>
struct F_vec_repeat { VecP<T> operator()( VecP<T> a, int n,	Fool b = false ); };

template<typename T>
struct F_vec_for_each { VecP<T> operator()( VecP<T> as,	pSoundMod<T> mod ); };

template<typename T>
struct F_vec_filter { VecP<T> operator()( VecP<T> a, pVecPredicate<T> predicate ); };