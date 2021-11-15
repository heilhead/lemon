#pragma once

#define INJA_THROW(exception) LEMON_ASSERT(false, "Inja Exception: ", exception.what())

#include <inja/inja.hpp>
#include <inja/environment.hpp>
