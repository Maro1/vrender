#include "app/vrender.hpp"
#include "core/engine.hpp"
#include "utils/log.hpp"
#include <memory>

int main()
{
    vrender::Engine engine;
    vrender::VRender* vrender = new vrender::VRender();
    engine.setApp(std::unique_ptr<vrender::VRender>(vrender));

    engine.run();

    return EXIT_SUCCESS;
}
