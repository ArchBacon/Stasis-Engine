#include "Engine.hpp"

int main(int /*argc*/, char* /*argv*/[])
{
    Engine.Initialize();
    Engine.Run();
    Engine.Shutdown();

    return 0;
}
