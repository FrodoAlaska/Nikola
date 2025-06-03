#pragma once

#include "nikola_pch.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Typedefs ***

/// @TODO (Containers): Create your own data structures, you lazy bastard.

/// An ASCII string
using String       = std::string;

/// A dynamically-sized array
template<typename T>
using DynamicArray = std::vector<T>;

/// A key-value pair hash map
template<typename K, typename V> 
using HashMap      = std::unordered_map<K, V>;

/// *** Typedefs ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
