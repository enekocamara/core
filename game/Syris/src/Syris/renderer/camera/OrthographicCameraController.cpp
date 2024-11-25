#include "OrthographicCameraController.hpp"
#include "Syris/input/Input.h"
#include "Syris/input/KeyCodes.h"
namespace Syris {
    OrthographicCameraController::OrthographicCameraController(CreateInfo info)
        : m_aspect_ratio(info.aspec_ratio),
        m_zoom_level(info.zoom_level),
        m_camera({-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level}){ 
    }

    OrthographicCameraController::~OrthographicCameraController(){}
    
    void OrthographicCameraController::on_update(const engine_time::Time& time){
        if (Input::is_key_pressed(KeyCode::KEY_A))
            m_position.x -= time.get_delta_ms() / 1000 * m_speed / m_zoom_level;
            
        if (Input::is_key_pressed(KeyCode::KEY_D))
            m_position.x += time.get_delta_ms() / 1000 * m_speed / m_zoom_level;
        if (Input::is_key_pressed(KeyCode::KEY_W))
            m_position.y += time.get_delta_ms() / 1000 * m_speed / m_zoom_level;

        if (Input::is_key_pressed(KeyCode::KEY_S))
            m_position.y -= time.get_delta_ms() / 1000 * m_speed / m_zoom_level;

        m_camera.set_position(m_position);
    }
}
