#pragma once
#include "OrthographicCamera.hpp"
#include "Syris/EngineTime.h"
namespace Syris {

class OrthographicCameraController {
    public:
        struct CreateInfo{
            float aspec_ratio;
            float zoom_level;
        };

        OrthographicCameraController(CreateInfo info);
        ~OrthographicCameraController();

        void on_update(engine_time::Time time);
        void on_event();

        CameraOrthographic& getCamera() {return m_camera;}
        const CameraOrthographic& getCamera() const {return m_camera;}

    private:
        float m_aspect_ratio;
        float m_zoom_level;
        CameraOrthographic m_camera;

        glm::vec3 m_position = {0.0f, 0.f, 0.f};
        float m_rotation = 0.f;
        float m_speed = 1; // per s
};
} // namespace Syris
