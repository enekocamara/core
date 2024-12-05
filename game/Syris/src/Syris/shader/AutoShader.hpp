#pragma once
#include <filesystem>
#include "Shader.hpp"
#include "Layout.hpp"
#include <expected>

namespace Syris{
    

    class AutoShader : public Shader{
    private:
        struct CompiledShaderBundle{
            Layout m_attributes;
            Layout m_uniforms;
            int m_program;
        };

    public:
        AutoShader(Shader::CreateInfo info);
        ~AutoShader(){};
        std::expected<CompiledShaderBundle, std::string> compile_shaders();
        void use(Uniform* uniforms)override;
        void on_update(const engine_time::Time& time)override;
        bool on_event(Event* event)override{return false;}; 
        const Layout& get_attribute_layout()override {return m_attributes;}
        const Layout& get_uniform_layout()override{return m_uniforms;}
        std::expected<std::string, std::string> parse_shader(std::string& source, bool is_vertex_shader, CompiledShaderBundle& bundle);
        std::expected<void, std::string> parse_layout_line(std::string& parsed_file, std::string_view line, CompiledShaderBundle& bundle);
        std::expected<void, std::string> parse_uniform_line(std::string& parsed_file, std::string_view line, CompiledShaderBundle& bundle);
        std::expected<void, std::string> parse_main(std::string& parsed_file, std::string_view line, CompiledShaderBundle& bundle);
    private:
        int m_program;
        CreateInfo m_info;
        std::filesystem::file_time_type m_last_change;
        std::string m_vertex_path;
        std::string m_fragment_path;
        Layout m_attributes;
        Layout m_uniforms;

        //IShaderLayout* m_layout;
    };
}