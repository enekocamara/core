#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Syris{
    class CameraOrthographic{
        public:
            struct CreateInfo{
                float left;
                float right;
                float top;
                float bottom;
            };


            CameraOrthographic(CreateInfo createInfo);


            void set_position(glm::vec3 position) {m_position = position;recalculate_view_matrix();}
            void set_rotation(float rotation) {m_rotation = rotation;recalculate_view_matrix();}

            glm::mat4 get_projection_matrix(){return m_projection_matrix;}
            glm::mat4 get_view_matrix(){return m_view_matrix;}
            glm::mat4 get_view_projection_matrix(){return m_view_projection_matrix;}
            glm::vec3 get_position(){return m_position;}
            float get_rotation(){return m_rotation;}

        private:
            void recalculate_view_matrix();

            glm::mat4 m_projection_matrix;
            glm::mat4 m_view_matrix;
            glm::mat4 m_view_projection_matrix;

            glm::vec3 m_position = {0.0f, 0.f, 0.f};
            float m_rotation = 0.f;
    };
}
