#ifndef BSTONE_OGL_SHADER_OBJECTS_H
#define BSTONE_OGL_SHADER_OBJECTS_H


#include "bstone_ogl_api.h"


namespace bstone {


class OglShaderObjects {
public:
    static GLuint create_shader(
        GLenum shader_type,
        const GLchar* shader_text);

    static GLuint create_program(
        GLuint fragment_shader,
        GLuint vertex_shader);

    static std::string get_info_log(
        GLuint object);

private:
    OglShaderObjects();

    OglShaderObjects(
        const OglShaderObjects& that);

    ~OglShaderObjects();

    OglShaderObjects& operator=(
        const OglShaderObjects& that);
}; // class OglShaderObjects


} // namespace bstone


#endif // BSTONE_OGL_SHADER_OBJECTS_H
