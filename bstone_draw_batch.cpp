#include "bstone_draw_batch.h"

#include "id_heads.h"

#include "bstone_draw_batch_command.h"
#include "bstone_font.h"
#include "bstone_sprite.h"
#include "bstone_picture.h"
#include "bstone_background.h"
#include "bstone_ogl_shader_objects.h"
#include "bstone_ogl_transformations.h"


namespace bstone {


namespace {


template<class G>
class IfEqPredicate {
public:
    IfEqPredicate(
        int id) :
            id_(id)
    {
    }

    bool operator()(
        const G& value)
    {
        return value.id == id_;
    }

private:
    int id_;
}; // class IfEqPredicate


const GLchar* batch_fs_text =
#ifdef BSTONE_USE_GLES
    "#version 100\n"
    "precision mediump float;\n"
#else
    "#version 120\n"
#endif

    "uniform sampler2D diffuse_tu;\n"
    "uniform vec4 solid_color_vec4;\n"
    "uniform float fade;\n"
    "uniform vec4 fade_color_vec4;\n"

    "varying vec2 tc;\n"

    "void main()\n"
    "{\n"
        "vec4 color = solid_color_vec4 * texture2D(diffuse_tu, tc);"
        "gl_FragColor = mix(color, fade_color_vec4, fade);\n"
        "gl_FragColor.a = color.a;\n"
    "}\n"
;

const GLchar* batch_vs_text =
#ifdef BSTONE_USE_GLES
    "#version 100\n"
    "precision mediump float;\n"
#else
    "#version 120\n"
#endif

    "attribute vec4 pos_vec4;\n"
    "attribute vec2 tc0_vec2;\n"

    "uniform mat4 proj_mat4;\n"

    "varying vec2 tc;\n"

    "void main()\n"
    "{\n"
        "tc = tc0_vec2;\n"
        "gl_Position = proj_mat4 * pos_vec4;\n"
        "gl_Position.z = 0.0;\n"
    "}\n"
;

const Rgba32F SOLID_WHITE(1.0F);
const Rgba32F SOLID_BLACK(0.0F, 0.0F, 0.0F, 1.0F);


} // namespace


// ========================================================================
// DrawBatch::Command

DrawBatch::Command::Command() :
    id(),
    is_enabled(true)
{
}

DrawBatch::Command::Command(
    int id,
    const DrawBatchCommand& command) :
        id(id),
        is_enabled(true),
        command(command)
{
}

DrawBatch::Command::Command(
    const Command& that) :
        id(that.id),
        is_enabled(that.is_enabled),
        command(that.command)
{
}

DrawBatch::Command::~Command()
{
}

DrawBatch::Command& DrawBatch::Command::operator=(
    const Command& that)
{
    if (&that != this) {
        id = that.id;
        is_enabled = that.is_enabled;
        command = that.command;
    }

    return *this;
}

// DrawBatch::Command
// ========================================================================

// ========================================================================
// DrawBatch::Group

DrawBatch::Group::Group() :
    id(),
    is_enabled(true)
{
}

DrawBatch::Group::Group(
    int id) :
        id(id),
        is_enabled(true)
{
}

DrawBatch::Group::Group(
    const Group& that) :
        id(that.id),
        is_enabled(that.is_enabled),
        commands(that.commands)
{
}

DrawBatch::Group::~Group()
{
}

DrawBatch::Group& DrawBatch::Group::operator=(
    const Group& that)
{
    if (&that != this) {
        id = that.id;
        is_enabled = that.is_enabled;
        commands = that.commands;
    }

    return *this;
}

// DrawBatch::Group
// ========================================================================

// ========================================================================
// DrawBatch

DrawBatch::DrawBatch() :
    current_group_(),
    current_commands_(),
    command_id_(),
    group_id_(),
    vertices_count_(),
    white_texture_(),
    vertices_vbo_(),
    fragment_shader_(),
    vertex_shader_(),
    program_(),
    a_pos_vec4_(-1),
    a_tc0_vec2_(-1),
    u_proj_mat4_(-1),
    u_diffuse_tu_(-1),
    u_solid_color_vec4_(-1),
    u_fade_(-1),
    u_fade_color_vec4_(-1)
{
}

DrawBatch::~DrawBatch()
{
}

void DrawBatch::initialize()
{
    uninitialize();

    setup_textures();
    setup_vertices();
    setup_shader_objects();
}

void DrawBatch::uninitialize()
{
    groups_.clear();
    current_group_ = NULL;
    current_commands_ = NULL;
    vertices_.clear();
    vertices_count_ = 0;

    delete white_texture_;
    white_texture_ = NULL;

    if (vertices_vbo_ != GL_NONE) {
        ::glDeleteBuffers(1, &vertices_vbo_);
        vertices_vbo_ = GL_NONE;
    }

    if (fragment_shader_ != GL_NONE) {
        ::glDeleteShader(fragment_shader_);
        fragment_shader_ = GL_NONE;
    }

    if (vertex_shader_ != GL_NONE) {
        ::glDeleteShader(vertex_shader_);
        vertex_shader_ = GL_NONE;
    }

    if (program_ != GL_NONE) {
        ::glDeleteProgram(program_);
        program_ = GL_NONE;
    }

    a_pos_vec4_ = -1;
    a_tc0_vec2_ = -1;
    u_proj_mat4_ = -1;
    u_diffuse_tu_ = -1;
    u_solid_color_vec4_ = -1;
}

int DrawBatch::add_command(
    const DrawBatchCommand& command)
{
    if (command.type == DBCT_NONE)
        throw std::invalid_argument("command.type");

    int command_id = command_id_;

    current_commands_->push_back(Command(command_id, command));

    ++command_id_;

    if (command_id_ < 0)
        command_id_ = 0;

    return command_id;
}

void DrawBatch::remove_command(
    int id)
{
    if (current_commands_ == NULL)
        throw std::runtime_error("No current commands.");

    current_commands_->remove_if(IfEqPredicate<Command>(id));
}

int DrawBatch::get_current_group() const
{
    if (current_group_ == NULL)
        return -1;

    return current_group_->id;
}

DrawBatchCommand& DrawBatch::get_command(
    int id)
{
    if (current_commands_ == NULL)
        throw std::runtime_error("No current commands.");

    CommandsIt it = std::find_if(
        current_commands_->begin(),
        current_commands_->end(),
        IfEqPredicate<Command>(id));

    if (it != current_commands_->end())
        return it->command;

    throw std::invalid_argument("id");
}

DrawBatchCommand& DrawBatch::get_command(
    int id,
    int group_id)
{
    Group* group = get_group(group_id);

    if (group == NULL)
        throw std::invalid_argument("group_id");

    Commands& commands = group->commands;

    CommandsIt it = std::find_if(
        commands.begin(),
        commands.end(),
        IfEqPredicate<Command>(id));

    if (it != commands.end())
        return it->command;

    throw std::invalid_argument("id");
}

void DrawBatch::enable_command(
    int id,
    bool value)
{
    if (current_commands_ == NULL)
        throw std::runtime_error("No current commands.");

    CommandsIt it = std::find_if(
        current_commands_->begin(),
        current_commands_->end(),
        IfEqPredicate<Command>(id));

    if (it != current_commands_->end())
        throw std::invalid_argument("id");

    it->is_enabled = value;
}

void DrawBatch::enable_command(
    int id,
    int group_id,
    bool value)
{
    Group* group = get_group(group_id);

    if (group == NULL)
        throw std::invalid_argument("group_id");

    Commands& commands = group->commands;

    CommandsIt cit = std::find_if(
        commands.begin(),
        commands.end(),
        IfEqPredicate<Command>(id));

    if (cit != commands.end())
        throw std::invalid_argument("id");

    cit->is_enabled = value;
}

int DrawBatch::add_group()
{
    int group_id = group_id_;

    groups_.push_back(Group(group_id));

    current_group_ = &groups_.back();
    current_commands_ = &current_group_->commands;

    ++group_id_;

    if (group_id_ < 0)
        group_id_ = 0;

    return group_id;
}

void DrawBatch::remove_group(
    int id)
{
    groups_.remove_if(IfEqPredicate<Group>(id));

    if (groups_.empty()) {
        current_group_ = NULL;
        current_commands_ = NULL;
    } else {
        current_group_ = &groups_.back();
        current_commands_ = &current_group_->commands;
    }
}

void DrawBatch::clear_group(
    int id)
{
    Group* group = get_group(id);

    if (group == NULL)
        return;

    group->commands.clear();
}

void DrawBatch::clear_current_group()
{
    if (current_group_ == NULL)
        return;

    current_group_->commands.clear();
}

void DrawBatch::enable_group(
    bool value)
{
    if (current_group_ == NULL)
        throw std::runtime_error("No current group.");

    current_group_->is_enabled = value;
}

void DrawBatch::enable_group(
    int id,
    bool value)
{
    GroupsIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(id));

    if (it == groups_.end())
        return;

    it->is_enabled = value;
}

void DrawBatch::set_current_group(
    int id)
{
    GroupsIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(id));

    if (it == groups_.end())
        throw std::invalid_argument("id");

    current_group_ = &(*it);
    current_commands_ = &current_group_->commands;
}

void DrawBatch::set_prev_group()
{
    GroupsIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(current_group_->id));

    if (it == groups_.begin())
        return;

    --it;

    current_group_ = &(*it);
    current_commands_ = &current_group_->commands;
}

void DrawBatch::set_next_group()
{
    GroupsIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(current_group_->id));

    if (it == groups_.end())
        return;

    ++it;

    current_group_ = &(*it);
    current_commands_ = &current_group_->commands;
}

void DrawBatch::clear()
{
    groups_.clear();
    current_group_ = NULL;
    current_commands_ = NULL;

    vertices_count_ = 0;
}

void DrawBatch::draw()
{
    if (groups_.empty())
        return;

    vertices_count_ = 0;

    ::glUseProgram(program_);

    ::glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);

    if (a_pos_vec4_ >= 0) {
        ::glVertexAttribPointer(
            a_pos_vec4_,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<const GLvoid*>(offsetof(Vertex,x)));

        ::glEnableVertexAttribArray(a_pos_vec4_);
    }

    if (a_tc0_vec2_ >= 0) {
        ::glVertexAttribPointer(
            a_tc0_vec2_,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<const GLvoid*>(offsetof(Vertex,s)));

        ::glEnableVertexAttribArray(a_tc0_vec2_);
    }

    for (GroupsCIt group = groups_.begin();
        group != groups_.end(); ++group)
    {
        if (!group->is_enabled)
            continue;

        const Commands& commands = group->commands;

        for (CommandsCIt command = commands.begin();
            command != commands.end(); ++command)
        {
            if (!command->is_enabled)
                continue;

            const DrawBatchCommand& draw_command = command->command;

            switch (draw_command.type) {
            case DBCT_BACKGROUND:
                draw_background(draw_command);
                break;

            case DBCT_STRING:
                draw_string(draw_command);
                break;

            case DBCT_RECTANGLE:
                draw_rectangle(draw_command);
                break;

            case DBCT_PICTURE:
                draw_picture(draw_command);
                break;

            case DBCT_SPRITE:
                draw_sprite(draw_command);
                break;

            case DBCT_WALL:
                draw_wall(draw_command);
                break;

            case DBCT_LATCH8:
                draw_latch8(draw_command);
                break;

            case DBCT_LATCH:
                draw_latch(draw_command);
                break;

            default:
                break;
            }
        }
    }

    ::glDisable(GL_BLEND);

    if (a_pos_vec4_ >= 0)
        ::glDisableVertexAttribArray(a_pos_vec4_);

    if (a_tc0_vec2_ >= 0)
        ::glDisableVertexAttribArray(a_tc0_vec2_);

    ::glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void DrawBatch::set_fade(
    float value)
{
    ::glUseProgram(program_);
    ::glUniform1f(u_fade_, value);
}

void DrawBatch::set_fade_color(
    int r,
    int g,
    int b)
{
    Rgba32F color(r / 255.0F, g / 255.0F, b / 255.0F);

    ::glUseProgram(program_);
    ::glUniform4fv(u_fade_color_vec4_, 1, &color[0]);
}

void DrawBatch::set_fade(
    float value,
    int r,
    int g,
    int b)
{
    Rgba32F color(r / 255.0F, g / 255.0F, b / 255.0F);
    ::glUseProgram(program_);
    ::glUniform1f(u_fade_, value);
    ::glUniform4fv(u_fade_color_vec4_, 1, &color[0]);
}

bool DrawBatch::is_command_exists(
    int id) const
{
    if (current_commands_ == NULL)
        throw std::runtime_error("No current commands.");

    CommandsCIt it = std::find_if(
        current_commands_->begin(),
        current_commands_->end(),
        IfEqPredicate<Command>(id));

    return it != current_commands_->end();
}

bool DrawBatch::is_group_exists(
    int id) const
{
    GroupsCIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(id));

    return it != groups_.end();
}

DrawBatch::Group* DrawBatch::get_group(
    int id)
{
    GroupsIt it = std::find_if(
        groups_.begin(),
        groups_.end(),
        IfEqPredicate<Group>(id));

    if (it != groups_.end())
        return &(*it);

    return NULL;
}

void DrawBatch::setup_textures()
{
    Rgba8U white_color(255);

    white_texture_ = new OglTexture(1, 1, OTF_RGB, &white_color);
}

void DrawBatch::setup_vertices()
{
    // An order of drawing striped triangles:
    //
    // 0   2   4
    // *---*---* ...
    // | / | / | /
    // *---*---* ...
    // 1   3   5
    //
    // 1st: 0-1-2
    // 2nd: 3-2-1
    // 3rd: 2-3-4
    // 4th: 5-4-3
    // ...
    //

    vertices_vbo_ = GL_NONE;
    ::glGenBuffers(1, &vertices_vbo_);

    if (vertices_vbo_ == GL_NONE)
        ::Quit("Failed to generate a vertex buffer.");

    ::glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);

    ::glBufferData(
        GL_ARRAY_BUFFER,
        get_max_vertices() * sizeof(Vertex),
        NULL,
        GL_DYNAMIC_DRAW);

    ::glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    vertices_.resize(get_max_vertices());
}

void DrawBatch::setup_shader_objects()
{
    fragment_shader_ = OglShaderObjects::create_shader(
        GL_FRAGMENT_SHADER, batch_fs_text);

    vertex_shader_ = OglShaderObjects::create_shader(
        GL_VERTEX_SHADER, batch_vs_text);

    program_ = OglShaderObjects::create_program(
        fragment_shader_, vertex_shader_);

    ::glUseProgram(program_);

    a_pos_vec4_ = ::glGetAttribLocation(program_, "pos_vec4");
    a_tc0_vec2_ = ::glGetAttribLocation(program_, "tc0_vec2");

    float proj_mat4[16];

    proj_mat4[0] = 1.0F;
    proj_mat4[4] = 0.0F;
    proj_mat4[8] = 0.0F;
    proj_mat4[12] = 0.0F;

    proj_mat4[1] = 0.0F;
    proj_mat4[5] = 1.0F;
    proj_mat4[9] = 0.0F;
    proj_mat4[13] = 0.0F;

    proj_mat4[2] = 0.0F;
    proj_mat4[6] = 0.0F;
    proj_mat4[10] = 1.0F;
    proj_mat4[14] = 0.0F;

    proj_mat4[3] = 0.0F;
    proj_mat4[7] = 0.0F;
    proj_mat4[11] = 0.0F;
    proj_mat4[15] = 1.0F;

    u_proj_mat4_ = ::glGetUniformLocation(program_, "proj_mat4");

    OglTransformations::ortho(
        vanilla_screen_width, vanilla_screen_height, proj_mat4);

    ::glUniformMatrix4fv(u_proj_mat4_, 1, GL_FALSE, proj_mat4);

    u_diffuse_tu_ = ::glGetUniformLocation(program_, "diffuse_tu");
    ::glUniform1i(u_diffuse_tu_, 0);

    u_solid_color_vec4_ =
        ::glGetUniformLocation(program_, "solid_color_vec4");
    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    u_fade_ = ::glGetUniformLocation(program_, "fade");
    ::glUniform1f(u_fade_, 0.0F);

    u_fade_color_vec4_ =
        ::glGetUniformLocation(program_, "fade_color_vec4");
    ::glUniform4fv(u_fade_color_vec4_, 1, &SOLID_BLACK[0]);

    ::glUseProgram(GL_NONE);
}

void DrawBatch::draw_background(
    const DrawBatchCommand& command)
{
    float width = static_cast<float>(vanilla_screen_width);
    float height = static_cast<float>(vanilla_screen_height);

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.background->get_texture()->bind();

    Vertex* vertex = &vertices_[0];

    vertex->x = 0.0F;
    vertex->y = height;
    vertex->s = 0.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = 0.0F;
    vertex->y = 0.0;
    vertex->s = 0.0F;
    vertex->t = 0.0F;
    ++vertex;

    vertex->x = width;
    vertex->y = height;
    vertex->s = 1.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = width;
    vertex->y = 0.0F;
    vertex->s = 1.0F;
    vertex->t = 0.0F;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_string(
    const DrawBatchCommand& command)
{
    if (command.text.empty())
        return;

    ::glEnable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.font->get_texture()->bind();

    size_t length = command.text.size();

    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float height = static_cast<float>(command.font->get_height());
    Vertex* vertices = &vertices_[0];

    for (size_t i = 0; i < length; ++i) {
        Uint8 ch = static_cast<Uint8>(command.text[i]);

        const FontChar& font_char =
            command.font->get_char(ch);

        if (font_char.width == 0)
            continue;

        float width = static_cast<float>(font_char.width);

        // Left triangle
        //
        // 0-2
        // |/
        // 1
        //

        vertices[0].x = x;
        vertices[0].y = y;
        vertices[0].s = font_char.tc_x0;
        vertices[0].t = font_char.tc_y1;

        vertices[1].x = x;
        vertices[1].y = y - height;
        vertices[1].s = font_char.tc_x0;
        vertices[1].t = font_char.tc_y0;

        vertices[2].x = x + width;
        vertices[2].y = y;
        vertices[2].s = font_char.tc_x1;
        vertices[2].t = font_char.tc_y1;


        // Right triangle
        //
        //   4
        //  /|
        // 5-3
        //

        vertices[3].x = x + width;
        vertices[3].y = y - height;
        vertices[3].s = font_char.tc_x1;
        vertices[3].t = font_char.tc_y0;

        vertices[4] = vertices[2];
        vertices[5] = vertices[1];

        vertices += 6;

        x += static_cast<float>(font_char.width);
    }

    int vertex_count = static_cast<int>(6 * length);

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        vertex_count * sizeof(Vertex),
        &vertices_[0]);

    Rgba32F color_f32 = Rgba32F::from_u8_to_f32(
        ::g_palette[command.color_index]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &color_f32[0]);

    ::glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void DrawBatch::draw_rectangle(
    const DrawBatchCommand& command)
{
    if (command.width == 0 || command.height == 0)
        return;

    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = static_cast<float>(command.width);
    float height = static_cast<float>(command.height);

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    white_texture_->bind();

    Vertex* vertices = &vertices_[0];

    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].s = 0.5F;
    vertices[0].t = 0.5F;

    vertices[1].x = x;
    vertices[1].y = y - height;
    vertices[1].s = 0.5F;
    vertices[1].t = 0.5F;

    vertices[2].x = x + width;
    vertices[2].y = y;
    vertices[2].s = 0.5F;
    vertices[2].t = 0.5F;

    vertices[3].x = x + width;
    vertices[3].y = y - height;
    vertices[3].s = 0.5F;
    vertices[3].t = 0.5F;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    Rgba32F color_f32 = Rgba32F::from_u8_to_f32(
        ::g_palette[command.color_index]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &color_f32[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_picture(
    const DrawBatchCommand& command)
{
    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = static_cast<float>(command.picture->get_width());
    float height = static_cast<float>(command.picture->get_height());

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.picture->get_texture()->bind();

    Vertex* vertex = &vertices_[0];

    vertex->x = x;
    vertex->y = y;
    vertex->s = 0.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x;
    vertex->y = y - height;
    vertex->s = 0.0F;
    vertex->t = 0.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y;
    vertex->s = 1.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y - height;
    vertex->s = 1.0F;
    vertex->t = 0.0F;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_sprite(
    const DrawBatchCommand& command)
{
    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = 64.0F;
    float height = 64.0F;

    ::glEnable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.sprite->get_texture()->bind();

    Vertex* vertex = &vertices_[0];

    vertex->x = x;
    vertex->y = y;
    vertex->s = 0.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x;
    vertex->y = y - height;
    vertex->s = 0.0F;
    vertex->t = 0.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y;
    vertex->s = 1.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y - height;
    vertex->s = 1.0F;
    vertex->t = 0.0F;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_wall(
    const DrawBatchCommand& command)
{
    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = 64.0F;
    float height = 64.0F;

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.wall->get_texture()->bind();

    Vertex* vertex = &vertices_[0];

    vertex->x = x;
    vertex->y = y;
    vertex->s = 0.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x;
    vertex->y = y - height;
    vertex->s = 0.0F;
    vertex->t = 0.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y;
    vertex->s = 1.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y - height;
    vertex->s = 1.0F;
    vertex->t = 0.0F;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_latch8(
    const DrawBatchCommand& command)
{
    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = 8.0F;
    float height = 8.0F;

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.latches8->get_texture()->bind();

    const Latch8& latch8 =
        command.latches8->get_latch(command.latch8_index);

    Vertex* vertex = &vertices_[0];

    vertex->x = x;
    vertex->y = y;
    vertex->s = latch8.tc_x0;
    vertex->t = latch8.tc_y1;
    ++vertex;

    vertex->x = x;
    vertex->y = y - height;
    vertex->s = latch8.tc_x0;
    vertex->t = latch8.tc_y0;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y;
    vertex->s = latch8.tc_x1;
    vertex->t = latch8.tc_y1;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y - height;
    vertex->s = latch8.tc_x1;
    vertex->t = latch8.tc_y0;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DrawBatch::draw_latch(
    const DrawBatchCommand& command)
{
    float x = static_cast<float>(command.x);
    float y = static_cast<float>(vanilla_screen_height - command.y);
    float width = static_cast<float>(command.latch->get_width());
    float height = static_cast<float>(command.latch->get_height());

    ::glDisable(GL_BLEND);

    ::glActiveTexture(GL_TEXTURE0);
    command.latch->get_texture()->bind();

    Vertex* vertex = &vertices_[0];

    vertex->x = x;
    vertex->y = y;
    vertex->s = 0.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x;
    vertex->y = y - height;
    vertex->s = 0.0F;
    vertex->t = 0.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y;
    vertex->s = 1.0F;
    vertex->t = 1.0F;
    ++vertex;

    vertex->x = x + width;
    vertex->y = y - height;
    vertex->s = 1.0F;
    vertex->t = 0.0F;
    ++vertex;

    ::glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        4 * sizeof(Vertex),
        &vertices_[0]);

    ::glUniform4fv(u_solid_color_vec4_, 1, &SOLID_WHITE[0]);

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// (static)
int DrawBatch::get_max_vertices()
{
    return 3 * get_max_triangles();
}

// (static)
int DrawBatch::get_max_triangles()
{
    return 2048;
}

// DrawBatch
// ========================================================================


} // namespace bstone
