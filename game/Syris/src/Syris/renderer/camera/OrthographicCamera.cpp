#include "OrthographicCamera.hpp"
#include <iostream>
#include <Syris/Libs.h>
namespace Syris{
    CameraOrthographic::CameraOrthographic(CreateInfo info)
        : m_projection_matrix(glm::ortho(info.left, info.right, info.top, info.bottom,-1.0f, 1.0f)),
          m_view_matrix(1.f)
    {
        m_view_projection_matrix = m_projection_matrix * m_view_matrix;
    }

    void CameraOrthographic::set_projection_matrix(CreateInfo info){
        m_projection_matrix = glm::ortho(info.left, info.right, info.top, info.bottom,-1.0f, 1.0f); 
		m_view_projection_matrix = m_projection_matrix * m_view_matrix;
    }

    void CameraOrthographic::recalculate_view_matrix(){ 
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0.f, 0.f, 1.f));

		m_view_matrix = glm::inverse(transform);
		m_view_projection_matrix = m_projection_matrix * m_view_matrix;
    }
}
