#include "gfx.hpp"
#include "fx.hpp"
#include <array>
#include <variant>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>


namespace gfx {

namespace {


constexpr uint32_t map_to_gl(ComponentType t) {
    const uint32_t lut[] = {
        GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT,
        GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_HALF_FLOAT,
    };
    return lut[static_cast<int>(t)];
}
constexpr uint32_t map_to_gl(BufferHint h) {
    const uint32_t lut[] = { GL_STREAM_DRAW, GL_DYNAMIC_DRAW, GL_STATIC_DRAW };
    return lut[static_cast<int>(h)];
}
constexpr uint32_t map_to_gl(DepthTestFunc dtf) {
    const uint32_t lut[] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, };
    return lut[static_cast<int>(dtf)];
}
constexpr uint32_t map_to_gl(PrimitiveType pt) {
    const uint32_t lut[] = {
        GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES,
        GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES
    };
    return lut[static_cast<int>(pt)];
}
constexpr uint32_t map_to_gl(BlendFunc bf) {
    const uint32_t lut[] = {
        GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA,
        GL_SRC_ALPHA_SATURATE,
    };
    return lut[static_cast<int>(bf)];
}
constexpr uint32_t map_to_gl(BlendEquation be) {
    const uint32_t lut[] = { GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT };
    return lut[static_cast<int>(be)];
}
constexpr uint32_t map_to_gl(CullFace cf) {
    const uint32_t lut[] = { GL_FRONT, GL_BACK, GL_FRONT_AND_BACK };
    return lut[static_cast<int>(cf)];
}
constexpr uint32_t map_to_gl(TextureFormat tf) {
    const uint32_t lut[] = { GL_RED, GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT, GL_STENCIL_INDEX, GL_DEPTH_STENCIL };
    return lut[static_cast<int>(tf)];
}


// opengl functions with caching
class {
public:
    void bind_vertex_array(uint32_t handle) {
        if (m_vertex_array != handle) {
            m_vertex_array = handle;
            glBindVertexArray(handle);
        }
    }
    void bind_framebuffer(uint32_t handle) {
        if (m_framebuffer != handle) {
            m_framebuffer = handle;
            glBindFramebuffer(GL_FRAMEBUFFER, handle);
        }
    }
    void bind_texture(int unit, uint32_t target, uint32_t handle) {
        if (m_active_texture != unit) {
            m_active_texture = unit;
            glActiveTexture(GL_TEXTURE0 + unit);
        }
        if (m_textures[unit] != handle) {
            m_textures[unit] = handle;
            glBindTexture(target, handle);
        }
    }
private:
    uint32_t                    m_vertex_array;
    uint32_t                    m_framebuffer;
    std::array<uint32_t, 80>    m_textures;
    int                         m_active_texture = 0;
} gl;


template<class T, uint32_t target>
struct GpuBuffer : T {

    GpuBuffer(BufferHint hint) : m_hint(hint) {
        glGenBuffers(1, &m_handle);
    }

    ~GpuBuffer() override {
        glDeleteBuffers(1, &m_handle);
    }

    void init_data(const void* data, int size) override {
        m_size = size;
        gl.bind_vertex_array(0);
        bind();
        glBufferData(target, m_size, data, map_to_gl(m_hint));
    }

    void bind() const {
        glBindBuffer(target, m_handle);
    }

    BufferHint m_hint;
    int        m_size = 0;
    uint32_t   m_handle;
};


using VertexBufferImpl = GpuBuffer<VertexBuffer, GL_ARRAY_BUFFER>;
using IndexBufferImpl = GpuBuffer<IndexBuffer, GL_ELEMENT_ARRAY_BUFFER>;


struct VertexArrayImpl : VertexArray {
    VertexArrayImpl() {
        glGenVertexArrays(1, &m_handle);
    }
    ~VertexArrayImpl() override {
        glDeleteVertexArrays(1, &m_handle);
    }

    void set_first(int i) override { m_first = i; }
    void set_count(int i) override { m_count = i; }
    void set_primitive_type(PrimitiveType t) override { m_primitive_type = t; }

    void set_attribute(int i, VertexBuffer* vb, ComponentType component_type,
                       int component_count, bool normalized, int offset, int stride) override
    {
        gl.bind_vertex_array(m_handle);
        static_cast<VertexBufferImpl*>(vb)->bind();
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, component_count, map_to_gl(component_type),
                              normalized, stride, reinterpret_cast<void*>(offset));
    }
    void set_attribute(int i, float f) override {
        gl.bind_vertex_array(m_handle);
        glDisableVertexAttribArray(i);
        glVertexAttrib1f(i, f);
    }
    void set_attribute(int i, const glm::vec2& v) override {
        gl.bind_vertex_array(m_handle);
        glDisableVertexAttribArray(i);
        glVertexAttrib2fv(i, &v.x);
    }
    void set_attribute(int i, const glm::vec3& v) override {
        gl.bind_vertex_array(m_handle);
        glDisableVertexAttribArray(i);
        glVertexAttrib3fv(i, &v.x);
    }
    void set_attribute(int i, const glm::vec4& v) override {
        gl.bind_vertex_array(m_handle);
        glDisableVertexAttribArray(i);
        glVertexAttrib4fv(i, &v.x);
    }

    void set_index_buffer(IndexBuffer* ib) override {
        if (ib) {
            m_indexed = true;
            gl.bind_vertex_array(m_handle);
            static_cast<IndexBufferImpl*>(ib)->bind();
        }
        else m_indexed = false;
    }

    PrimitiveType get_primitive_type() const override { return m_primitive_type; }

    int           m_first = 0;
    int           m_count = 0;
    bool          m_indexed = false;
    PrimitiveType m_primitive_type = PrimitiveType::Triangles;
    uint32_t      m_handle;
};


struct Texture2DImpl : Texture2D {
    Texture2DImpl() {
        glGenTextures(1, &m_handle);
    }

    ~Texture2DImpl() override {
        glDeleteTextures(1, &m_handle);
    }

    int get_width() const override { return m_width; }
    int get_height() const override { return m_height; }

    // TODO: sampler stuff
//    void set_wrap(WrapMode horiz, WrapMode vert);
//    void set_filter(FilterMode min, FilterMode mag);



    bool init(const char* filename, FilterMode filter) {
        SDL_Surface* s = IMG_Load(filename);
        if (!s) return false;
        init(s, filter);
        SDL_FreeSurface(s);
        return true;
    }
    bool init(SDL_Surface* s, FilterMode filter) {
        TextureFormat f = s->format->BytesPerPixel == 1 ? TextureFormat::Red
                        : s->format->BytesPerPixel == 3 ? TextureFormat::RGB
                                                        : TextureFormat::RGBA;
        return init(f, s->w, s->h, s->pixels, filter);
    }
    bool init(TextureFormat format, int w, int h, void* data, FilterMode filter) {
        //printf("error %d\n", glGetError());
        m_width  = w;
        m_height = h;
        m_format = format;

        gl.bind_texture(0, GL_TEXTURE_2D, m_handle);

        if (filter == FilterMode::Nearest) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    filter == FilterMode::Trilinear ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        }

        // XXX: the browser is very finicky. this is the result of trial and error.
        if (m_format == TextureFormat::Depth) {
            float c[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                         m_width, m_height, 0, map_to_gl(m_format),
                         GL_UNSIGNED_INT, data);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, map_to_gl(m_format),
                         m_width, m_height, 0, map_to_gl(m_format),
                         GL_UNSIGNED_BYTE, data);
        }

        if (filter == FilterMode::Trilinear) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        return true;
    }


    int           m_width;
    int           m_height;
    TextureFormat m_format;
    uint32_t      m_handle;
};


void gl_uniform(int l, int v) { glUniform1i(l, v); }
void gl_uniform(int l, float v) { glUniform1f(l, v); }
void gl_uniform(int l, glm::vec2 const& v) { glUniform2fv(l, 1, &v.x); }
void gl_uniform(int l, glm::vec3 const& v) { glUniform3fv(l, 1, &v.x); }
void gl_uniform(int l, glm::vec4 const& v) { glUniform4fv(l, 1, &v.x); }
void gl_uniform(int l, glm::mat3 const& v) { glUniformMatrix3fv(l, 1, false, &v[0].x); }
void gl_uniform(int l, glm::mat4 const& v) { glUniformMatrix4fv(l, 1, false, &v[0].x); }

GLuint compile_shader(uint32_t type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint e = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &e);
    if (e) return s;
    int len = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    char log[len];
    glGetShaderInfoLog(s, len, &len, log);
    fprintf(stderr, "Error: can't compile shader\n%s\n%s\n", src, log);
    glDeleteShader(s);
    return 0;
}

struct ShaderImpl : Shader {

    bool init(const char* vs, const char* fs) {
        GLint v = compile_shader(GL_VERTEX_SHADER, vs);
        if (v == 0) return false;
        GLint f = compile_shader(GL_FRAGMENT_SHADER, fs);
        if (f == 0) {
            glDeleteShader(v);
            return false;
        }
        m_program = glCreateProgram();

        glAttachShader(m_program, v);
        glAttachShader(m_program, f);
        glDeleteShader(v);
        glDeleteShader(f);
        glLinkProgram(m_program);


        // attributes
        int count;
        glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &count);
        for (int i = 0; i < count; ++i) {
            char name[128];
            int size;
            uint32_t type;
            glGetActiveAttrib(m_program, i, sizeof(name), nullptr, &size, &type, name);
            m_attributes.push_back({ name, type, glGetAttribLocation(m_program, name) });
        }

        // uniforms
        glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &count);
        for (int i = 0; i < count; ++i) {
            char name[128];
            int size; // > 1 for arrays
            uint32_t type;
            glGetActiveUniform(m_program, i, sizeof(name), nullptr, &size, &type, name);
            int location = glGetUniformLocation(m_program, name);
            m_uniforms.emplace_back(name, type, location);
            Uniform& u = m_uniforms.back();
            switch (type) {
            case GL_INT:        u.extent = Uniform::Extent<int>(); break;
            case GL_FLOAT:      u.extent = Uniform::Extent<float>(); break;
            case GL_FLOAT_VEC2: u.extent = Uniform::Extent<glm::vec2>(); break;
            case GL_FLOAT_VEC3: u.extent = Uniform::Extent<glm::vec3>(); break;
            case GL_FLOAT_VEC4: u.extent = Uniform::Extent<glm::vec4>(); break;
            case GL_FLOAT_MAT3: u.extent = Uniform::Extent<glm::mat3>(); break;
            case GL_FLOAT_MAT4: u.extent = Uniform::Extent<glm::mat4>(); break;
            case GL_SAMPLER_2D: u.extent = Uniform::ExtentTexture2D(); break;
            default:
                fprintf(stderr, "Error: uniform '%s' has unknown type (%d)\n", name, type);
                assert(false);
            }
        }

        return true;
    }

    ~ShaderImpl() override {
        glDeleteProgram(m_program);
    }


    struct Attribute {
        std::string name;
        uint32_t    type;
        int         location;
    };


    struct Uniform {
        Uniform(const std::string& name, uint32_t type, int location)
            : name(name), type(type), location(location) {}

        void update() const {
            std::visit([this](auto& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, ExtentTexture2D>) {
                    int unit = location;
                    gl.bind_texture(unit, GL_TEXTURE_2D, e.handle);
                    glUniform1i(location, unit);
                }
                else {
                    if (!e.dirty) return;
                    e.dirty = false;
                    gl_uniform(location, e.value);
                }
            }, extent);
        }

        template<class T>
        struct Extent {
            Extent() : dirty(true) {}
            mutable bool dirty;
            T            value;
        };
        struct ExtentTexture2D {
            uint32_t handle;
        };

        template<class T>
        void set(const T& value) {
            if constexpr (std::is_same<T, Texture2D*>::value) {
                ExtentTexture2D& e = std::get<ExtentTexture2D>(extent);
                e.handle = static_cast<Texture2DImpl*>(value)->m_handle;
            }
            else {
                Extent<T>& e = std::get<Extent<T>>(extent);
                if (e.value != value) {
                    e.value = value;
                    e.dirty = true;
                }
            }
        }

        const std::string name;
        const uint32_t    type;
        const int         location;
        std::variant<
            Extent<int>,
            Extent<float>,
            Extent<glm::vec2>,
            Extent<glm::vec3>,
            Extent<glm::vec4>,
            Extent<glm::mat3>,
            Extent<glm::mat4>,
            ExtentTexture2D
        > extent;
    };


    bool has_uniform(std::string const& name) override { return find_uniform(name) != nullptr; }
    void set_uniform(std::string const& name, Texture2D* v) override { set(name, v); }
    void set_uniform(std::string const& name, int v) override { set(name, v); }
    void set_uniform(std::string const& name, float v) override { set(name, v); }
    void set_uniform(std::string const& name, glm::vec2 const& v) override { set(name, v); }
    void set_uniform(std::string const& name, glm::vec3 const& v) override { set(name, v); }
    void set_uniform(std::string const& name, glm::vec4 const& v) override { set(name, v); }
    void set_uniform(std::string const& name, glm::mat3 const& v) override { set(name, v); }
    void set_uniform(std::string const& name, glm::mat4 const& v) override { set(name, v); }

    Uniform* find_uniform(std::string const& name) {
        for (auto& u : m_uniforms) {
            if (u.name == name) return &u;
        }
        return nullptr;
    }

    template<class T>
    void set(const std::string& name, const T& value) {
        Uniform* u = find_uniform(name);
        assert(u);
        u->set(value);
    }

    void update_uniforms() const {
        for (auto& u : m_uniforms) u.update();
    }

    uint32_t               m_program = 0;
    std::vector<Attribute> m_attributes;
    std::vector<Uniform>   m_uniforms;
};



struct FramebufferImpl : Framebuffer {
    FramebufferImpl() {
        glGenFramebuffers(1, &m_handle);
    }
    ~FramebufferImpl() override {
        glDeleteFramebuffers(1, &m_handle);
    }
    void attach_color(Texture2D* t) override {
        auto ti = static_cast<Texture2DImpl*>(t);
        if (ti) {
            m_width  = ti->m_width;
            m_height = ti->m_height;
        }
        gl.bind_framebuffer(m_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, ti ? ti->m_handle : 0, 0);
        // TODO: is this correct?
        //glDrawBuffer(t ? GL_COLOR_ATTACHMENT0 : GL_NONE);
    }
    void attach_depth(Texture2D* t) override {
        auto ti = static_cast<Texture2DImpl*>(t);
        if (ti) {
            m_width  = ti->m_width;
            m_height = ti->m_height;
        }
        gl.bind_framebuffer(m_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, ti ? ti->m_handle : 0, 0);
    }
    bool is_complete() const override {
        gl.bind_framebuffer(m_handle);
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    uint32_t m_handle;
    int      m_width;
    int      m_height;
};



} // namespace


VertexBuffer* VertexBuffer::create(BufferHint hint) {
    return new VertexBufferImpl(hint);
}

IndexBuffer* IndexBuffer::create(BufferHint hint) {
    return new IndexBufferImpl(hint);
}

VertexArray* VertexArray::create() {
    return new VertexArrayImpl();
}

Shader* Shader::create(const char* vs, const char* fs) {
    auto s = new ShaderImpl;
    if (!s->init(vs, fs)) return nullptr;
    return s;
}

Texture2D* Texture2D::create(SDL_Surface* s, FilterMode filter) {
    auto t = new Texture2DImpl;
    if (!t->init(s, filter)) return nullptr;
    return t;
}

Texture2D* Texture2D::create(const char* filename, FilterMode filter) {
    auto t = new Texture2DImpl;
    if (!t->init(filename, filter)) return nullptr;
    return t;
}

Texture2D* Texture2D::create(TextureFormat format, int w, int h, void* data, FilterMode filter) {
    auto t = new Texture2DImpl;
    if (!t->init(format, w, h, data, filter)) return nullptr;
    return t;
}

Framebuffer* Framebuffer::create() {
    return new FramebufferImpl();
}


namespace {

    RenderState s_render_state;
    glm::vec4   s_clear_color;
    ShaderImpl* s_shader;

    void sync_render_state(const RenderState& rs) {
        // depth
        if (s_render_state.depth_test_enabled != rs.depth_test_enabled) {
            s_render_state.depth_test_enabled = rs.depth_test_enabled;
            if (s_render_state.depth_test_enabled) glEnable(GL_DEPTH_TEST);
            else glDisable(GL_DEPTH_TEST);
        }
        if (s_render_state.depth_test_enabled) {
            if (s_render_state.depth_test_func != rs.depth_test_func) {
                s_render_state.depth_test_func = rs.depth_test_func;
                glDepthFunc(map_to_gl(s_render_state.depth_test_func));
            }
        }

        // cull face
        if (s_render_state.cull_face_enabled != rs.cull_face_enabled) {
            s_render_state.cull_face_enabled = rs.cull_face_enabled;
            if (s_render_state.cull_face_enabled) glEnable(GL_CULL_FACE);
            else glDisable(GL_CULL_FACE);
        }
        if (s_render_state.cull_face_enabled) {
            if (s_render_state.cull_face != rs.cull_face) {
                s_render_state.cull_face = rs.cull_face;
                glCullFace(map_to_gl(s_render_state.cull_face));
            }
        }

        // depth
        if (s_render_state.scissor_test_enabled != rs.scissor_test_enabled) {
            s_render_state.scissor_test_enabled = rs.scissor_test_enabled;
            if (s_render_state.scissor_test_enabled) glEnable(GL_SCISSOR_TEST);
            else glDisable(GL_SCISSOR_TEST);
        }
        if (s_render_state.scissor_test_enabled) {
            if (s_render_state.scissor_box.x != rs.scissor_box.x ||
                s_render_state.scissor_box.y != rs.scissor_box.y ||
                s_render_state.scissor_box.w != rs.scissor_box.w ||
                s_render_state.scissor_box.h != rs.scissor_box.h)
            {
                s_render_state.scissor_box = rs.scissor_box;
                glScissor(s_render_state.scissor_box.x,
                          s_render_state.scissor_box.y,
                          s_render_state.scissor_box.w,
                          s_render_state.scissor_box.h);
            }
        }


        // blend
        if (s_render_state.blend_enabled != rs.blend_enabled) {
            s_render_state.blend_enabled = rs.blend_enabled;
            if (s_render_state.blend_enabled) glEnable(GL_BLEND);
            else glDisable(GL_BLEND);
        }
        if (s_render_state.blend_enabled) {
            if (s_render_state.blend_func_src_rgb != rs.blend_func_src_rgb ||
                s_render_state.blend_func_src_alpha != rs.blend_func_src_alpha ||
                s_render_state.blend_func_dst_rgb != rs.blend_func_dst_rgb ||
                s_render_state.blend_func_dst_alpha != rs.blend_func_dst_alpha)
            {
                s_render_state.blend_func_src_rgb   = rs.blend_func_src_rgb;
                s_render_state.blend_func_src_alpha = rs.blend_func_src_alpha;
                s_render_state.blend_func_dst_rgb   = rs.blend_func_dst_rgb;
                s_render_state.blend_func_dst_alpha = rs.blend_func_dst_alpha;
                glBlendFuncSeparate(map_to_gl(s_render_state.blend_func_src_rgb),
                                    map_to_gl(s_render_state.blend_func_dst_rgb),
                                    map_to_gl(s_render_state.blend_func_src_alpha),
                                    map_to_gl(s_render_state.blend_func_dst_alpha));
            }
            if (s_render_state.blend_equation_rgb != rs.blend_equation_rgb ||
                s_render_state.blend_equation_alpha != rs.blend_equation_alpha)
            {
                s_render_state.blend_equation_rgb = rs.blend_equation_rgb;
                s_render_state.blend_equation_alpha = rs.blend_equation_alpha;
                glBlendEquationSeparate(map_to_gl(s_render_state.blend_equation_rgb),
                                        map_to_gl(s_render_state.blend_equation_alpha));
            }
            if (s_render_state.blend_color != rs.blend_color) {
                s_render_state.blend_color = rs.blend_color;
                glBlendColor(s_render_state.blend_color.r,
                             s_render_state.blend_color.g,
                             s_render_state.blend_color.b,
                             s_render_state.blend_color.a);
            }
        }
    }

} // namespace


bool init() {
    glewExperimental = true;
    glewInit();

    // initialize the reder state according to opengl's initial state
    s_render_state.cull_face_enabled = false;
    s_render_state.depth_test_enabled = false;
    s_render_state.depth_test_func = DepthTestFunc::Less;
    glEnable(GL_PROGRAM_POINT_SIZE);

    return true;
}


void free() {}


void clear(const glm::vec4& color, Framebuffer* fb) {
    if (s_clear_color != color) {
        s_clear_color = color;
        glClearColor(s_clear_color.x, s_clear_color.y, s_clear_color.z, s_clear_color.w);
    }
    auto fbi = static_cast<FramebufferImpl*>(fb);
    gl.bind_framebuffer(fbi ? fbi->m_handle : 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void draw(const RenderState& rs, Shader* shader, VertexArray* va, Framebuffer* fb) {
    auto vai = static_cast<VertexArrayImpl*>(va);
    auto fbi = static_cast<FramebufferImpl*>(fb);

    if (vai->m_count == 0) return;

    sync_render_state(rs);

    // only consider RenderState::viewport if it's valid
    Rect vp = { 0, 0, 0, 0 };
    if (rs.viewport.w != 0) vp = rs.viewport;
    else {
        if (fbi) {
            vp.w = fbi->m_width;
            vp.h = fbi->m_height;
        }
        else {
            vp.w = fx::screen_width();
            vp.h = fx::screen_height();
        }
    }
    if (memcmp(&s_render_state.viewport, &vp, sizeof(Rect)) != 0) {
        s_render_state.viewport = vp;
        glViewport(s_render_state.viewport.x,
                   s_render_state.viewport.y,
                   s_render_state.viewport.w,
                   s_render_state.viewport.h);
    }
    if (s_render_state.line_width != rs.line_width) {
        s_render_state.line_width = rs.line_width;
        glLineWidth(s_render_state.line_width);
    }


    // sync shader
    if (s_shader != static_cast<ShaderImpl*>(shader)) {
        s_shader = static_cast<ShaderImpl*>(shader);
        glUseProgram(s_shader->m_program);
    }
    s_shader->update_uniforms();

    gl.bind_vertex_array(vai->m_handle);

    gl.bind_framebuffer(fbi ? fbi->m_handle : 0);

    if (vai->m_indexed) {
        glDrawElements(map_to_gl(vai->m_primitive_type), vai->m_count, GL_UNSIGNED_INT,
                       reinterpret_cast<void*>(vai->m_first));
    }
    else {
        glDrawArrays(map_to_gl(vai->m_primitive_type), vai->m_first, vai->m_count);
    }
}


} // namespace
