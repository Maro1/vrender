#include "app/app.hpp"

#include "core/engine.hpp"

namespace vrender
{
Scene* App::world() const
{
    return Engine::get().world();
}
} // namespace vrender
