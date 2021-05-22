#include "app/vrender.h"
#include "utils/log.h"

int main()
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    vrender::VRender vrender;
    if (!vrender.run())
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
