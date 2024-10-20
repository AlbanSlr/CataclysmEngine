// std lib headers
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "CataclysmApplication.hpp"

int main()
{
    Cataclysm::CataclysmApplication app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}