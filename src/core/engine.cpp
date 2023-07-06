#include "engine.hpp"

#include "app/app.hpp"
#include "core/graphics_context.hpp"

#include "GLFW/glfw3.h"
#include "utils/log.hpp"
#include <utility>

namespace vrender
{

Engine::Engine() {}

Engine::~Engine() {}

int Engine::run()
{
    V_LOG_LEVEL(V_LOG_LEVEL_DEBUG);
    if (!m_App)
    {
        V_LOG_WARNING("No app set to run.");
        return -1;
    }

    if (!glfwInit())
    {
        V_LOG_ERROR("Could not init GLFW library.");
        return -1;
    }

    GraphicsContext::get().init(m_App->info());

    m_App->init();
    V_LOG_INFO("Started {} application.", m_App->info().title);

    while (!GraphicsContext::get().window()->shouldClose())
    {
        glfwPollEvents();
        window()->update();
        world()->update();

        double deltaTime = glfwGetTime() - m_LastTime;
        m_App->update(deltaTime);
    }
    glfwTerminate();
    vkDeviceWaitIdle(device()->device());
    return 0;
}

void Engine::setApp(std::unique_ptr<App> app)
{
    if (m_App)
    {
        // GraphicsContext::get().terminate();
        m_App->terminate();
    }
    m_App = std::move(app);
}
} // namespace vrender