# Build Dawn

```sh
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
ninja -C out\Release dawn_native_shared dawn_platform_shared dawn_proc_shared
ninja -C out\Debug dawn_native_shared dawn_platform_shared dawn_proc_shared
```