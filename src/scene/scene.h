#pragma once

#include "camera/camera.h"
#include "model/mesh.h"
#include "ecs/world.h"

#include <memory>
#include <unordered_set>

namespace vrender
{
    class Scene
    {
        public:
            
            Scene();
            ~Scene();

        private:

            std::unique_ptr<Camera> m_Camera;
            std::unordered_set<Mesh*> m_Meshes;

            World m_World;

    };
}