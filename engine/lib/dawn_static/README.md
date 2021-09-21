# DAWN

Source: https://dawn.googlesource.com/dawn

## Updating code

Run in `dawn` source directory, e.g. `C:\git\dawn`:

```sh
# Clean any untracked files from previous builds
git clean -fd

# Pull latest changes
git pull

# Update dependencies
gclient sync
```

## Building

```sh
# Set up env
set DAWN_DIR=C:\git\dawn
set LEMON_DIR=C:\git\lemon
set DEPOT_TOOLS_WIN_TOOLCHAIN=0

# Build generated sources
python3 %DAWN_DIR%/generator/dawn_json_generator.py --dawn-json %DAWN_DIR%/dawn.json  --wire-json %DAWN_DIR%/dawn_wire.json --targets dawn_headers,dawncpp_headers,dawncpp,dawn_proc,mock_webgpu,dawn_wire,dawn_native_utils --template-dir %DAWN_DIR%/generator/templates --output-dir %DAWN_DIR%

# Configure debug
gn --root=%DAWN_DIR% gen //out/debug_x64_static --args="is_debug=true strip_debug_info=true enable_iterator_debugging=true symbol_level=0 is_component_build=false use_goma=false dawn_use_swiftshader=false is_clang=false visual_studio_version=\"2019\" target_cpu=\"x64\" dawn_use_angle=false dawn_complete_static_libs=true"

# Configure release
gn --root=%DAWN_DIR% gen //out/release_x64_static --args="is_debug=false strip_debug_info=true enable_iterator_debugging=false symbol_level=0 is_component_build=false use_goma=false dawn_use_swiftshader=false is_clang=false visual_studio_version=\"2019\" target_cpu=\"x64\" dawn_use_angle=false dawn_complete_static_libs=true"

# Build debug
ninja -C %DAWN_DIR%/out/debug_x64_static -t clean
ninja -C %DAWN_DIR%/out/debug_x64_static dawn_native_static dawn_platform_static dawn_proc_static

# Build release
ninja -C %DAWN_DIR%/out/release_x64_static -t clean
ninja -C %DAWN_DIR%/out/release_x64_static dawn_native_static dawn_platform_static dawn_proc_static

# Copy sources
robocopy %DAWN_DIR%/src/include %LEMON_DIR%/engine/lib/dawn_static/include /S
robocopy %DAWN_DIR%/src/dawn %LEMON_DIR%/engine/lib/dawn_static/src/dawn webgpu_cpp.cpp

# Copy debug libs
robocopy %DAWN_DIR%/out/debug_x64_static/obj/src/dawn %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Debug/ dawn_proc_static.lib
robocopy %DAWN_DIR%/out/debug_x64_static/obj/src/dawn_native %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Debug/ dawn_native_static.lib
robocopy %DAWN_DIR%/out/debug_x64_static/obj/src/dawn_platform %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Debug/ dawn_platform_static.lib

# Copy release libs
robocopy %DAWN_DIR%/out/release_x64_static/obj/src/dawn %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Release/ dawn_proc_static.lib
robocopy %DAWN_DIR%/out/release_x64_static/obj/src/dawn_native %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Release/ dawn_native_static.lib
robocopy %DAWN_DIR%/out/release_x64_static/obj/src/dawn_platform %LEMON_DIR%/engine/lib/dawn_static/lib/x64/Release/ dawn_platform_static.lib
```