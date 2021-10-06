#include "app/vrender.h"
#include "core/graphics_context.h"
#include "utils/log.h"
#include "ecs/world.h"

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
