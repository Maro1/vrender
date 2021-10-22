#include "app/vrender.hpp"
#include "core/graphics_context.hpp"
#include "utils/log.hpp"

int main()
{
    vrender::GraphicsContext::get().init();
    vrender::VRender vrender;
    if (!vrender.run())
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
