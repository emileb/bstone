#ifndef BSTONE_DRAW_BATCH_H
#define BSTONE_DRAW_BATCH_H


#include <list>
#include <vector>

#include "bstone_draw_batch_command.h"
#include "bstone_ogl_api.h"
#include "bstone_ogl_texture.h"


namespace bstone {


class DrawBatch {
public:
    DrawBatch();

    ~DrawBatch();

    void initialize();

    void uninitialize();

    int add_command(
        const DrawBatchCommand& command);

    void remove_command(
        int id);

    void enable_command(
        int id,
        bool value);

    void enable_command(
        int id,
        int group_id,
        bool value);

    int add_group();

    void remove_group(
        int id);

    void clear_group(
        int id);

    void clear_current_group();

    int get_current_group() const;

    DrawBatchCommand& get_command(
        int id);

    DrawBatchCommand& get_command(
        int id,
        int group_id);

    void enable_group(
        bool value);

    void enable_group(
        int id,
        bool value);

    void set_current_group(
        int id);

    void set_prev_group();

    void set_next_group();

    void clear();

    void draw();

    bool is_command_exists(
        int id) const;

    bool is_group_exists(
        int id) const;

private:
    class Command {
    public:
        int id;
        bool is_enabled;
        DrawBatchCommand command;

        Command();

        Command(
            int id,
            const DrawBatchCommand& command);

        Command(
            const Command& that);

        ~Command();

        Command& operator=(
            const Command& that);
    }; // class Command

    typedef std::list<Command> Commands;
    typedef Commands::iterator CommandsIt;
    typedef Commands::const_iterator CommandsCIt;

    class Group {
    public:
        int id;
        bool is_enabled;
        Commands commands;

        Group();

        explicit Group(
            int id);

        Group(
            const Group& that);

        ~Group();

        Group& operator=(
            const Group& that);
    }; // class Group

    typedef std::list<Group> Groups;
    typedef Groups::iterator GroupsIt;
    typedef Groups::const_iterator GroupsCIt;

    typedef Uint16 Index;
    typedef std::vector<Index> Indices;
    typedef Indices::iterator IndicesIt;
    typedef Indices::const_iterator IndicesCIt;

    class Vertex {
    public:
        float x;
        float y;

        float s;
        float t;
    }; // class Vertex

    typedef std::vector<Vertex> Vertices;

    Groups groups_;
    Group* current_group_;
    Commands* current_commands_;
    int command_id_;
    int group_id_;

    Vertices vertices_;
    int vertices_count_;

    OglTexture* white_texture_;

    GLuint vertices_vbo_;
    GLuint fragment_shader_;
    GLuint vertex_shader_;
    GLuint program_;

    GLint a_pos_vec4_;
    GLint a_tc0_vec2_;
    GLint u_proj_mat4_;
    GLint u_diffuse_tu_;
    GLint u_solid_color_vec4_;

    DrawBatch(
        const DrawBatch& that);

    DrawBatch& operator=(
        const DrawBatch& that);

    Group* get_group(
        int id);

    void setup_textures();

    void setup_vertices();

    void setup_shader_objects();

    void draw_background(
        const DrawBatchCommand& command);

    void draw_string(
        const DrawBatchCommand& command);

    void draw_rectangle(
        const DrawBatchCommand& command);

    void draw_picture(
        const DrawBatchCommand& command);

    void draw_sprite(
        const DrawBatchCommand& command);

    void draw_wall(
        const DrawBatchCommand& command);

    void draw_latch8(
        const DrawBatchCommand& command);

    void draw_latch(
        const DrawBatchCommand& command);

    static int get_max_vertices();

    static int get_max_triangles();
}; // class DrawBatch


} // namespace bstone


#endif // BSTONE_DRAW_BATCH_H
