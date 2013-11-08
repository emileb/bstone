#include "bstone_ogl_shader_objects.h"

#include "id_heads.h"


namespace bstone {


// (static)
GLuint OglShaderObjects::create_shader(
    GLenum shader_type,
    const GLchar* shader_text)
{
    GLuint shader = GL_NONE;
    shader = ::glCreateShader(shader_type);

    if (shader == GL_NONE)
        ::Quit("OGL: Failed to create a shader object.");

    GLint compile_status = GL_FALSE;
    const GLchar* lines[1] = { shader_text };
    GLint lengths[1] = {
        static_cast<GLint>(std::string::traits_type::length(shader_text))
    };

    ::glShaderSource(shader, 1, lines, lengths);
    ::glCompileShader(shader);
    ::glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    std::string shader_log = get_info_log(shader);

    if (compile_status != GL_FALSE) {
        if (!shader_log.empty()) {
            ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "OGL: %s", shader_log.c_str());
        }
    } else {
        if (shader_log.empty())
            shader_log = "Generic compile error.";

        ::Quit("OGL: %s", shader_log.c_str());
    }

    return shader;
}

// (static)
GLuint OglShaderObjects::create_program(
    GLuint fragment_shader,
    GLuint vertex_shader)
{
    if (::glIsShader(fragment_shader) == GL_FALSE)
        ::Quit("OGL: No fragment shader.");

    if (::glIsShader(vertex_shader) == GL_FALSE)
        ::Quit("OGL: No vertex shader.");

    GLuint program = ::glCreateProgram();

    if (program == GL_NONE)
        ::Quit("OGL: Failed to create a program object.");

    GLint link_status = GL_FALSE;

    ::glAttachShader(program, fragment_shader);
    ::glAttachShader(program, vertex_shader);
    ::glLinkProgram(program);
    ::glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    std::string program_log = get_info_log(program);

    if (link_status != GL_FALSE) {
        if (!program_log.empty()) {
            ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "OGL: %s", program_log.c_str());
        }
    } else {
        if (program_log.empty())
            program_log = "Generic link error.";

        Quit("OGL: %s", program_log.c_str());
    }

    return program;
}

// (static)
std::string OglShaderObjects::get_info_log(
    GLuint object)
{
    if (object == GL_NONE)
        return std::string();

    typedef void (APIENTRY* LogFunction)(GLuint,GLsizei,GLsizei*,GLchar*);
    typedef void (APIENTRY* LogLengthFunction)(GLuint,GLenum,GLint*);

    LogFunction info_func = NULL;
    LogLengthFunction length_func = NULL;

    if (::glIsShader(object)) {
        info_func = ::glGetShaderInfoLog;
        length_func = ::glGetShaderiv;
    } else if (::glIsProgram(object)) {
        info_func = ::glGetProgramInfoLog;
        length_func = ::glGetProgramiv;
    } else
        return std::string();


    GLint info_log_size = 0; // with a null terminator

    length_func(object, GL_INFO_LOG_LENGTH, &info_log_size);

    if (info_log_size <= 1)
        return std::string();

    GLsizei info_log_length; // without a null terminator
    std::vector<GLchar> info_log(info_log_size);

    info_func(object, info_log_size, &info_log_length, &info_log[0]);

    if (info_log_length > 0)
        return std::string(&info_log[0], info_log_length);

    return std::string();
}


} // namespace bstone
