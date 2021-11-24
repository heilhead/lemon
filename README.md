# Lemon Game Engine

A very early in development game engine being created mostly for educational purposes.

# Project Highlights

- Multi-threading and concurrency in every aspect of the engine, using C++20 coroutines (based on [folly's implementation](https://github.com/facebook/folly/blob/main/folly/experimental/coro/README.md)).
- WebGPU graphics API for portability.
- Robust [resource management API](https://github.com/heilhead/lemon/blob/main/engine/include/lemon/resource/ResourceManager.h) with resource dependencies and bundles support.
- Advanced [materials](https://github.com/heilhead/lemon/blob/main/resources/misc/M_Mannequin.meta) with inheritance support and easy runtime (uniform) configuration, thanks to shader code reflection.
- Actor-based [game object framework](https://github.com/heilhead/lemon/blob/main/engine/include/lemon/game/actor/Actor.h) with advanced features like actor/component tick dependencies.
- [Pushdown automaton](https://en.wikipedia.org/wiki/Pushdown_automaton) and FSM-based [game state management system](https://github.com/heilhead/lemon/blob/main/engine/include/lemon/game/state/GameState.h).

# Code Structure

- `demo/`: The code here is _very_ temporary and subject to change pretty soon. This is the place where engine features are developed before becoming stable-ish and moving over to `engine/`.
- `engine/`: The code here is _kinda_ finished. Obviously, the engine is very early in development and most of this code will be refactored at some point, but for now it's considered stable as it's working and has been tested.
- `lib/`: The libraries that are either shared between other packages, or needed to be separately compiled (i.e. using a different C++ standard).
- `resoures/`: Some internally used (e.g. shaders and materials) engine resources, as well as temporary testing resources.
- `test/`: Some unit tests for various parts of the engine and support libraries.
- `tools/`: Engine CLI tools, such as `fbx2lem` mesh converter.

# License

MIT
