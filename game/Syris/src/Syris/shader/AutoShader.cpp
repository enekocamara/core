#include <format>

#include "AutoShader.hpp"
#include "Syris/types/Type.h"
#include "Syris/utils/filesystem/readfile.h"
#include "Syris/utils/filesystem/writeFile.h"
#include "Syris/types/OpenGLToSyrisTypes.h"
#include "Syris/utils/std/string_view.h"
#include "OpenGLUtils.h"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/utils/Breakpoint.h"
#include "Syris/utils/Assert.h"

namespace Syris{
    AutoShader::AutoShader(CreateInfo info):m_info(info){
        m_vertex_path = std::format("{}\\vertex.sy", info.path);
        m_fragment_path = std::format("{}\\fragment.sy", info.path);
        m_last_change = std::max(std::filesystem::last_write_time(m_vertex_path), std::filesystem::last_write_time(m_fragment_path));
        auto res = this->compile_shaders();
        if (!res){
            BREAK_POINT(std::format("Failed to create autoshader: {}", res.error()));
        }
            //throw std::runtime_error(std::format("Failed to create autoshader, {}", res.error()));
        m_attributes = res.value().m_attributes;
        m_uniforms = res.value().m_uniforms;
        m_program = res.value().m_program;
    };

    std::expected<AutoShader::CompiledShaderBundle, std::string> AutoShader::compile_shaders(){
        CompiledShaderBundle bundle;
        bundle.m_program = glCreateProgram();
        
        std::string vertex_source = utils::read_file(m_vertex_path.c_str()).value();
        std::string fragment_source = utils::read_file(m_fragment_path.c_str()).value();

        auto parsed_vertex = parse_shader(vertex_source, true, bundle);
        if (!parsed_vertex)
            return std::unexpected(std::format("Failed to parse vertex shader: {}", parsed_vertex.error()));
        auto parsed_fragment = parse_shader(fragment_source, false, bundle);
        if (!parsed_fragment)
            return std::unexpected(std::format("Failed to parse fragment shader: {}", parsed_fragment.error()));
        auto compile_res = compile_shaders_source(bundle.m_program, parsed_fragment.value(), parsed_vertex.value());
        CHECK_GL_ERROR();
        if (!compile_res)
            return std::unexpected(std::format("Failed to compile shaders: {}", compile_res.error()));
        write_file(std::format("{}\\parsed_vertex.glsl", m_info.path), parsed_vertex.value());
        write_file(std::format("{}\\parsed_fragment.glsl", m_info.path), parsed_fragment.value());
        return bundle;
    }

    std::expected<std::string, std::string> AutoShader::parse_shader(std::string& source, bool is_vertex_shader, CompiledShaderBundle& bundle){
        if (!is_vertex_shader)
            return source;//todo
        std::vector<std::string_view> lines = view_split(source, '\n');
        std::string parsed_file;
        bool copy_rest_of_file = false;
        for (auto& line : lines){
            if (copy_rest_of_file){
                parsed_file += line;
                parsed_file += '\n';
                continue;
            }
            if (line.starts_with("#") || line.starts_with("out")) {
                parsed_file += line;
                parsed_file += '\n';
            }
            else if (line.starts_with("layout")){
                auto result = parse_layout_line(parsed_file, line, bundle);
                if (!result){
                    CORE_ERROR(std::format("Failed to parse shader layout line", result.error()));
                    return std::unexpected(std::format("Failed to parse shader layout line: {}", result.error()));
                }
            } else if (line.starts_with("uniform")){
                auto result = parse_uniform_line(parsed_file, line, bundle);
                if (!result)
                    return std::unexpected(std::format("Failed to parse shader uniform line: {}", result.error()));
            } else if (line.starts_with("void main()")){
                auto result = parse_main(parsed_file, line, bundle);
                if (!result)
                    return std::unexpected(std::format("Failed to parse shader main: {}", result.error()));
                copy_rest_of_file = true;
            } else {
                parsed_file += line;
                parsed_file += '\n';
            }
        }
        return parsed_file;
    }

    std::expected<void, std::string> AutoShader::parse_layout_line(std::string& parsed_file, std::string_view line, CompiledShaderBundle& bundle){
        std::vector<std::string_view> words = view_split(line.substr(0, line.size() -1), " ");//remove ; and split in words
        if (words.size() != 5)
            return std::unexpected(std::format("Incorrect number of words in '{}'. Must be 5", line));
        auto type = glsl_to_syris_type(words[3]);
        if (!type || !type::can_be_attribute(type.value())){
            return std::unexpected(std::format("'{}' is either not a type or not a type that can be used as attribute.", words[3]));
        }
        bundle.m_attributes.add_variable(words[4], type.value());
        return std::expected<void, std::string>{};
    }

    std::expected<void, std::string> AutoShader::parse_uniform_line(std::string& parsed_file, std::string_view line, CompiledShaderBundle& bundle){
        std::vector<std::string_view> words = view_split(line.substr(0, line.size() -1), " ");
        if (words.size() != 3)
            return std::unexpected(std::format("Incorrect number of words in '{}'. Must be 3", line));
        auto type = glsl_to_syris_type(words[1]);
        if (!type || !type::can_be_uniform(type.value())){
            return std::unexpected(std::format("'{}' is either not a type or not a type that can be used as attribute.", words[1]));
        }
        bundle.m_uniforms.add_variable(words[2], type.value());
        return std::expected<void, std::string>{};
    }

    std::expected<void, std::string> AutoShader::parse_main(std::string& parsed_file, std::string_view main_line, CompiledShaderBundle& bundle){
        uint32_t attribute_index = 0;
        std::vector<std::tuple<const std::string&, Type, std::vector<std::string>>> uncopled_types;
        auto& variables = bundle.m_attributes.get_vars();
        for (const std::string& name : bundle.m_attributes.get_order()){
            Type type = variables.at(name);
            auto [uncoupled_type, count] = to_glsl_type(type);
            if (count == 1){
                parsed_file += std::format("layout (location={}) in {} {};\n", attribute_index, type_to_shader_script_type(uncoupled_type), name);
                attribute_index++;
            }
            else{
                uncopled_types.emplace_back(name, type, std::vector<std::string>());
                for (uint32_t i = 0; i < count; i++)
                {
                    std::string unclopled_name = std::format("{}_{}", name, i);
                    parsed_file += std::format("layout (location={}) in {} {};\n", attribute_index, type_to_shader_script_type(uncoupled_type), unclopled_name);
                    std::get<2>(uncopled_types[uncopled_types.size() - 1]).push_back(unclopled_name);
                    attribute_index++;
                }

            }
        }
        for (const auto& [name, type] : bundle.m_uniforms.get_vars()) {
            parsed_file += std::format("uniform {} {};\n", type_to_shader_script_type(type), name);
            attribute_index++;
        }
        parsed_file += main_line;
        parsed_file += '\n';
        for (auto& [name, type, uncopled_names] : uncopled_types){
            if (type == Type::mat4){
                parsed_file += std::format("\tmat4 {} = mat4({},{},{},{});\n", name, uncopled_names[0], uncopled_names[1], uncopled_names[2], uncopled_names[3]);
            }
            else{
                CORE_ERROR("only mat4 supported in type uncopling");
                return std::unexpected("only mat4 supported in type uncopling");
            }
        }
        return std::expected<void, std::string>{};
    }

    void AutoShader::use(Uniform* uniforms){
        glUseProgram(m_program);
        uint32_t i = 0;
        auto& layout = m_uniforms.get_vars();
        while (uniforms != nullptr){
            ASSERT(uniforms->name.data() != nullptr, "Name must be set");
            auto it = layout.find(uniforms->name);
            ASSERT(it != layout.end(), "Uniform name doens't exist in shader uniforms");
            //todo ASSERT(layout.find(uniforms->name)->second == uniforms->type, "Uniform name doens't exist in shader uniforms");
            renderAPI::set_uniform_value(m_program, uniforms->name.c_str(), it->second, uniforms->data);
            uniforms = uniforms->pnext;
            i++;
        }
        ASSERT(i == layout.size(), "Wrong number of uniforms");
        //auto hold = std::make_tuple(m_program, uniforms);
        //m_layout->set(&hold); set uniforms
        CHECK_GL_ERROR();
    }


    void AutoShader::on_update(const engine_time::Time& time){
        if (m_last_change != std::max(std::filesystem::last_write_time(m_vertex_path), std::filesystem::last_write_time(m_fragment_path))){
            m_last_change = std::max(std::filesystem::last_write_time(m_vertex_path), std::filesystem::last_write_time(m_fragment_path));
            auto res = compile_shaders();
            if (!res){
                CORE_INFO("Failed to refresh shader");
                return;
            }
            m_attributes = res.value().m_attributes;
            m_uniforms = res.value().m_uniforms;
            m_program = res.value().m_program;
        }
    }
}