#include "Camera.hpp"
#include <iostream>
#include <Libs.h>
namespace Syris{
    CameraOrthographic::CameraOrthographic(CameraOrthographic::CreateInfo info)
        : m_projection_matrix(glm::ortho(info.left, info.right, info.top, info.bottom,-1.0f, 1.0f)),
          m_view_matrix(1.f)
    {
        recalculate_view_matrix();
//        m_view_projection_matrix = m_projection_matrix * m_view_matrix;

        /*test*/
        glm::vec4 result = m_view_projection_matrix * glm::vec4(1.f,1.f, 0.f,0.f);
        std::cout << "POS: " << result << '\n';
    }

    void CameraOrthographic::recalculate_view_matrix(){
        
        auto translate = glm::translate(glm::mat4{ 1.0f }, m_position);

		auto rotate = glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_rotation), glm::vec3{ 0.0f,0.0f,1.0f });

		auto scale = glm::scale(glm::mat4{ 1.0f }, glm::vec3{ 1.f, 1.f, 1.0f });  // zoom the same factor in x and y axis, neutral on z axis

		glm::mat4 transform = scale * rotate * translate;
		m_view_matrix = glm::inverse(transform);

		/*glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0.f, 0.f, 1.f));

		m_view_matrix = glm::inverse(transform);*/
		m_view_projection_matrix = m_projection_matrix * m_view_matrix;
    }
}
