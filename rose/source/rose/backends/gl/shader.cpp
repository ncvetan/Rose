﻿#include <rose/backends/gl/shader.hpp>

#include <GL/glew.h>
#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <optional>

namespace gl {

rses Shader::init(const std::vector<ShaderCtx>& shader_ctxs) {
    
    i32 success = 0;
    char info_log[512];

    prg = glCreateProgram();
    std::vector<u32> shaders;

    for (const auto& shader_info : shader_ctxs) {

        std::ifstream shader_file(shader_info.path);

        if (!shader_file) {
            glDeleteProgram(prg);
            prg = 0;
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }
            return rses().io("Unable to open shader at path: {}", shader_info.path.generic_string());
        }

        std::stringstream shader_code_buf;
        shader_code_buf << shader_file.rdbuf();
        std::string shader_code_str = shader_code_buf.str();
        const char* shader_code = shader_code_str.c_str();
        u32 curr_shader = 0;

        curr_shader = glCreateShader(shader_info.type);

        if (!curr_shader) {
            glDeleteProgram(prg);
            prg = 0;
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }
            return rses().gl("Invalid shader type for shader at path {}", shader_info.path.generic_string());
        }

        glShaderSource(curr_shader, 1, &shader_code, NULL);
        glCompileShader(curr_shader);
        glGetShaderiv(curr_shader, GL_COMPILE_STATUS, &success);
        
        if (!success) {
            glGetShaderInfoLog(curr_shader, 512, NULL, info_log);
            glDeleteProgram(prg);
            prg = 0;
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }
            return rses().gl("Shader compilation failed at path {}: {}", shader_info.path.generic_string(), info_log);
        };

        glAttachShader(prg, curr_shader);
        shaders.push_back(curr_shader);
    }
    
    glLinkProgram(prg);
    glGetProgramiv(prg, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(prg, 512, NULL, info_log);
        glDeleteProgram(prg);
        prg = 0;
        for (auto shader : shaders) {
            glDeleteShader(shader);
        }
        return rses().gl("Shader linking failed: {}", info_log);
    }

    for (auto shader : shaders) {
        glDeleteShader(shader);
    }

    return {};
}

Shader::~Shader() {
    if (prg) {
        glDeleteProgram(prg);
    }
}

void Shader::use() { glUseProgram(prg); }

void Shader::set_bool(const std::string_view& name, bool value) const {
    glProgramUniform1i(prg, glGetUniformLocation(prg, name.data()), (int)value);
}

void Shader::set_u32(const std::string_view& name, u32 value) const {
    glProgramUniform1ui(prg, glGetUniformLocation(prg, name.data()), value);
}

void Shader::set_i32(const std::string_view& name, int value) const {
    glProgramUniform1i(prg, glGetUniformLocation(prg, name.data()), value);
}

void Shader::set_tex(const std::string_view& name, int value, u32 tex) const {
    glProgramUniform1i(prg, glGetUniformLocation(prg, name.data()), value);
    glBindTextureUnit(value, tex);
}
void Shader::set_f32(const std::string_view& name, f32 value) const {
    glProgramUniform1f(prg, glGetUniformLocation(prg, name.data()), value);
}

void Shader::set_mat4(const std::string_view& name, const glm::mat4& value) const {
    glProgramUniformMatrix4fv(prg, glGetUniformLocation(prg, name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec2(const std::string_view& name, const glm::vec2& value) const {
    glProgramUniform2f(prg, glGetUniformLocation(prg, name.data()), value.x, value.y);
}

void Shader::set_uvec2(const std::string_view& name, const glm::uvec2& value) const {
    glProgramUniform2ui(prg, glGetUniformLocation(prg, name.data()), value.x, value.y);
}

void Shader::set_vec3(const std::string_view& name, const glm::vec3& value) const {
    glProgramUniform3f(prg, glGetUniformLocation(prg, name.data()), value.x, value.y, value.z);
}

void Shader::set_uvec3(const std::string_view& name, const glm::uvec3& value) const {
    glProgramUniform3ui(prg, glGetUniformLocation(prg, name.data()), value.x, value.y, value.z);
}

void Shader::set_vec4(const std::string_view& name, const glm::vec4& value) const {
    glProgramUniform4f(prg, glGetUniformLocation(prg, name.data()), value.x, value.y, value.z, value.w);
}

rses Shaders::init() {

    rses err;

    if (err = downsample.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER },
                                { SOURCE_DIR "/rose/shaders/gl/bloom/downsample.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = upsample.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER },
                              { SOURCE_DIR "/rose/shaders/gl/bloom/upsample.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = ssao.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER },
                          { SOURCE_DIR "/rose/shaders/gl/ssao.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = blur.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER },
                          { SOURCE_DIR "/rose/shaders/gl/blur.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = clusters_build.init({ { SOURCE_DIR "/rose/shaders/gl/compute/clusters_build.comp", GL_COMPUTE_SHADER } })) {
        return err;
    }
    if (err = clusters_cull.init({ { SOURCE_DIR "/rose/shaders/gl/compute/clusters_cull.comp", GL_COMPUTE_SHADER } })) {
        return err;
    }
    if (err = gbuf.init({ { SOURCE_DIR "/rose/shaders/gl/gbuf.vert", GL_VERTEX_SHADER   },
                          { SOURCE_DIR "/rose/shaders/gl/gbuf.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = out.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER   },
                         { SOURCE_DIR "/rose/shaders/gl/out.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = light.init({ { SOURCE_DIR "/rose/shaders/gl/light.vert", GL_VERTEX_SHADER   },
                           { SOURCE_DIR "/rose/shaders/gl/light.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = lighting_deferred.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER   },
                                       { SOURCE_DIR "/rose/shaders/gl/lighting_deferred.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = lighting_forward.init({ { SOURCE_DIR "/rose/shaders/gl/lighting_forward.vert", GL_VERTEX_SHADER },
                                      { SOURCE_DIR "/rose/shaders/gl/lighting_forward.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = passthrough.init({ { SOURCE_DIR "/rose/shaders/gl/quad.vert", GL_VERTEX_SHADER   },
                                 { SOURCE_DIR "/rose/shaders/gl/passthrough.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }
    if (err = dir_shadow.init({{ SOURCE_DIR "/rose/shaders/gl/shadow/shadow.vert", GL_VERTEX_SHADER },
                               { SOURCE_DIR "/rose/shaders/gl/shadow/dir_shadow.frag", GL_FRAGMENT_SHADER },
                               { SOURCE_DIR "/rose/shaders/gl/shadow/dir_shadow.geom", GL_GEOMETRY_SHADER } })) {
        return err;
    }
    if (err = pt_shadow.init({ { SOURCE_DIR "/rose/shaders/gl/shadow/shadow.vert", GL_VERTEX_SHADER   },
                               { SOURCE_DIR "/rose/shaders/gl/shadow/pt_shadow.frag", GL_FRAGMENT_SHADER },
                               { SOURCE_DIR "/rose/shaders/gl/shadow/pt_shadow.geom", GL_GEOMETRY_SHADER } })) {
        return err;
    }
    if (err = skybox.init({ { SOURCE_DIR "/rose/shaders/gl/skybox.vert", GL_VERTEX_SHADER   },
                            { SOURCE_DIR "/rose/shaders/gl/skybox.frag", GL_FRAGMENT_SHADER } })) {
        return err;
    }

    return err;
}

} // namespace gl
