#include <format>

#include "AutoShader.hpp"
#include "Syris/types/Type.h"
#include "Syris/utils/file/readfile.h"
#include "Syris/utils/file/writeFile.h"
#include "Syris/types/OpenGLToSyrisTypes.h"
#include "Syris/utils/std/string_view.h"
#include "OpenGLUtils.h"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/utils/Breakpoint.h"

namespace Syris{
    AutoShader::AutoShader(CreateInfo info){
        m_program = glCreateProgram();
        std::cout << "PATH: " << info.path << "\n";
        std::string vertex_path = std::format("{}\\vertex.sy", info.path);
        std::string fragment_path = std::format("{}\\fragment.sy", info.path);
        std::string vertex_source = utils::read_file(vertex_path.c_str());
        std::string fragment_source = utils::read_file(fragment_path.c_str());
        
            auto parsed_vertex = parse_shader(vertex_source, true);
            if (!parsed_vertex)
                throw std::runtime_error(std::format("Failed to parse vertex shader: {}", parsed_vertex.error()));
            write_file(std::format("{}\\parsed_vertex.glsl", info.path), parsed_vertex.value());
            auto parsed_fragment = parse_shader(fragment_source, false);
            if (!parsed_fragment)
                throw std::runtime_error(std::format("Failed to parse fragment shader: {}", parsed_fragment.error()));
            write_file(std::format("{}\\parsed_fragment.glsl", info.path), parsed_fragment.value());
            auto compile_res = compile_shaders_source(m_program, parsed_fragment.value(), parsed_vertex.value());
            if (!compile_res)
                throw std::runtime_error(std::format("Failed to compile shaders: {}", compile_res.error()));
    };

    std::expected<std::string, std::string> AutoShader::parse_shader(std::string& source, bool is_vertex_shader){
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
                auto result = parse_layout_line(parsed_file, line);
                if (!result)
                    return std::unexpected(std::format("Failed to parse shader layout line: {}", result.error()));
            } else if (line.starts_with("uniform")){
                auto result = parse_uniform_line(parsed_file, line);
                if (!result)
                    return std::unexpected(std::format("Failed to parse shader uniform line: {}", result.error()));
            } else if (line.starts_with("void main()")){
                auto result = parse_main(parsed_file, line);
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

    std::expected<void, std::string> AutoShader::parse_layout_line(std::string& parsed_file, std::string_view line){
        std::vector<std::string_view> words = view_split(line.substr(0, line.size() -1), " ");//remove ; and split in words
        if (words.size() != 5)
            return std::unexpected(std::format("Incorrect number of words in '{}'. Must be 5", line));
        auto type = glsl_to_syris_type(words[3]);
        if (!type || !type::can_be_attribute(type.value())){
            return std::unexpected(std::format("'{}' is either not a type or not a type that can be used as attribute.", words[3]));
        }
        m_attributes.add_variable(words[4], type.value());
    }

    std::expected<void, std::string> AutoShader::parse_uniform_line(std::string& parsed_file, std::string_view line){
        std::vector<std::string_view> words = view_split(line.substr(0, line.size() -1), " ");
        if (words.size() != 3)
            return std::unexpected(std::format("Incorrect number of words in '{}'. Must be 3", line));
        auto type = glsl_to_syris_type(words[1]);
        if (!type || !type::can_be_uniform(type.value())){
            return std::unexpected(std::format("'{}' is either not a type or not a type that can be used as attribute.", words[1]));
        }
        m_uniforms.add_variable(words[2], type.value());
    }

    std::expected<void, std::string> AutoShader::parse_main(std::string& parsed_file, std::string_view main_line){
        uint32_t attribute_index = 0;
        std::vector<std::tuple<const std::string&, Type, std::vector<std::string>>> uncopled_types;
        auto& variables = m_attributes.get_vars();
        for (const std::string& name : m_attributes.get_order()){
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
        for (const auto& [name, type] : m_uniforms.get_vars()) {
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
    }

    void AutoShader::use(Uniform* uniforms){
        glUseProgram(m_program);
        uint32_t i = 0;
        while (uniforms != nullptr){
            auto it = m_uniforms.get_vars().find(uniforms->name);
            if (it == m_uniforms.get_vars().end()){
                BREAK_POINT("Uniform name doens't exist in shader uniforms");
            }
            renderAPI::set_uniform_value(m_program, uniforms->name.c_str(), it->second, uniforms->data);
            uniforms = uniforms->pnext;
            i++;
        }
        if (i != m_uniforms.get_vars().size()){
            BREAK_POINT("Wrong number of uniforms")
        }
        //auto hold = std::make_tuple(m_program, uniforms);
        //m_layout->set(&hold); set uniforms
        CHECK_GL_ERROR();
    }
}