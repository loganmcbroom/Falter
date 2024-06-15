#include "VecMethods.h"

#include <flan/Audio/Audio.h>
#include <flan/PV/PV.h>

template<typename T>
VecP<T> F_vec_group<T>::operator()( VecP<T> a,
	int group_size,
	pVecGroupFunc<T> func ) // f( VecP<T>, int ) -> VecP<T>
	{ 
	std::cout << "falter::vec_group";

	// Split the in vec into a vector of in vecs, each of group_size
	const int num_groups = std::ceil( float( a.size() ) / group_size );
	std::vector<VecP<T>> vec_split( num_groups );
	for( int i = 0; i < a.size(); ++i )
		{
		const int group = std::floor( float( i ) / group_size );
		vec_split[group].push_back( a[i] );
		}

	// Process each in vec group using func, flatten outputs to single vec
	VecP<T> out;
	for( int i = 0; i < vec_split.size(); ++i )
		{
		VecP<T> current_out = (*func)( std::make_pair( vec_split[i], i ) );
		out.insert( out.end(), current_out.begin(), current_out.end() );
		}

	return out;
	}
template struct F_vec_group<flan::Audio>;
template struct F_vec_group<flan::PV>;

template<typename T>
VecP<T> F_vec_attach<T>::operator()( VecP<T> as,
	VecP<T> b )
	{ 
	std::cout << "falter::vec_attach";

	VecP<T> out;
	for( auto & a : as )
		out.push_back( std::make_shared<T>( a->copy() ) );
	out.insert( out.end(), b.begin(), b.end() );

	return out;
	}
template struct F_vec_attach<flan::Audio>;
template struct F_vec_attach<flan::PV>;

template<typename T>
VecP<T> F_vec_repeat<T>::operator()( VecP<T> a,
	int n,
	Fool b )
	{ 
	std::cout << "falter::vec_repeat";

	VecP<T> out;

	if( b.b )
		for( int j = 0; j < a.size(); ++j )
			for( int i = 0; i < n; ++i )
				out.push_back( std::make_shared<T>( a[j]->copy() ) );
	else
		for( int i = 0; i < n; ++i )
			for( int j = 0; j < a.size(); ++j )
				out.push_back( std::make_shared<T>( a[j]->copy() ) );

	return out;
	}
template struct F_vec_repeat<flan::Audio>;
template struct F_vec_repeat<flan::PV>;

template<typename T>
VecP<T> F_vec_for_each<T>::operator()( 
	VecP<T> as,
	pSoundMod<T> mod )
	{ 
	std::cout << "falter::vec_for_each";

	VecP<T> out;
	for( std::shared_ptr<T> a : as )
		out.push_back( std::make_shared<T>( a->copy() ) );
	for( int n = 0; n < as.size(); ++n )
		(*mod)( *out[n], n + 1 );
	
	return out;
	}
template struct F_vec_for_each<flan::Audio>;
template struct F_vec_for_each<flan::PV>;

template<typename T>
VecP<T> F_vec_filter<T>::operator()( 
	VecP<T> a,
    pVecPredicate<T> predicate )
    { 
    std::cout << "falter::vec_filter";

    VecP<T> out;

    for( int n = 0; n < a.size(); ++n )
        if( (*predicate)( std::make_pair( a[n], n+1 ) ) )
            out.push_back( a[n] );

    return out;
    }
template struct F_vec_filter<flan::Audio>;
template struct F_vec_filter<flan::PV>;