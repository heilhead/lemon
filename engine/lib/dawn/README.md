# Build Dawn

```sh
# Set up paths
set DAWN_DIR=C:\git\dawn
set LEMON_DIR=C:\git\lemon

# Build generated sources
python3 %DAWN_DIR%/generator/dawn_json_generator.py --dawn-json %DAWN_DIR%/dawn.json  --wire-json %DAWN_DIR%/dawn_wire.json --targets dawn_headers,dawncpp_headers,dawncpp,dawn_proc,mock_webgpu,dawn_wire,dawn_native_utils --template-dir %DAWN_DIR%/generator/templates --output-dir %DAWN_DIR%

# Copy includes and missing CPP code
robocopy %DAWN_DIR%/src/include %LEMON_DIR%/engine/lib/dawn/include /E
robocopy %DAWN_DIR%/src/dawn/ %LEMON_DIR%/engine/lib/dawn/src/dawn/ webgpu_cpp.cpp

# Build dawn
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
ninja -C out\Release dawn_native_shared dawn_platform_shared dawn_proc_shared
ninja -C out\Debug dawn_native_shared dawn_platform_shared dawn_proc_shared

# Copy DLL/LIB files
robocopy %DAWN_DIR%/out/Debug/ %LEMON_DIR%/engine/lib/dawn/lib/x64/Debug/ dawn_*.dll dawn_*.exp dawn_*.lib dawn_*.pdb
robocopy %DAWN_DIR%/out/Release/ %LEMON_DIR%/engine/lib/dawn/lib/x64/Release/ dawn_*.dll dawn_*.exp dawn_*.lib dawn_*.pdb
```
