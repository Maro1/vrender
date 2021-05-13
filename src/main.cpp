#include "app/vrender.h"
#include "utils/vlog.h"

int main() 
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    VRender vrender;
    if (!vrender.run()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}       
