#pragma once
#include <memory>
#include "modules/utils/PlatformDetection.h"

#define BIT(x) (1 << x)
#define NANO_EVENT_BIND(fn) std::bind(&fn, this, std::placeholders::_1)

using byte = uint8_t;