# Lemon Shader

A wrapper around `tint` shader compiler. Currently `tint` has some issues compiling with C++20 std, and exposes its internals in a bad way. This library is intended to address these issues and provide shader reflection API for use in engine materials code.

## Building

Requires Google's `depot_tools` to be in `PATH`:

```bash
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
gclient sync
```
