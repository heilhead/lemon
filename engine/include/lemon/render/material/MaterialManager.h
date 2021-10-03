#pragma once

#include <lemon/render/material/SurfaceMaterialInstance.h>

namespace lemon::render {
    class MaterialManager {
    public:
        MaterialManager();
        ~MaterialManager();

        static MaterialManager*
        get();
    };
} // namespace lemon::render