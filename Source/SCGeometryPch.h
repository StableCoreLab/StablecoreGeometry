#pragma once

// Keep this header small and stable so it helps compile time without becoming a
// maintenance burden.
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Core/GeometryTypes.h"
#include "Support/Epsilon.h"
#include "Support/Geometry2d/Predicate2.h"
