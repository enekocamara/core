#include <glad/glad.h>

namespace Syris::renderer{
    class VertexArray{
        public:
            VertexArray() {glGenVertexArrays(1, &m_vertex_array);};
            ~VertexArray();
            void bind() {glBindVertexArray(m_vertex_array);}
            void unbind(){glBindVertexArray(0);}
        GLuint m_vertex_array;
    };
}
