﻿#include "Core/Engine.hpp"

int main(int argc, char* argv[])
{
    gEngine.Initialize();
    gEngine.Run();
    gEngine.Shutdown();

    return 0;
}
