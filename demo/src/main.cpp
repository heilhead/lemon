#include "lemon/engine.h"

int main(int argc, char *argv[])
{
    auto* engine = lemon::Engine::get();
    engine->init();
    engine->shutdown();
    return 0;
}
