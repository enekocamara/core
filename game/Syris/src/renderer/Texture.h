#pragma once
#include "../Libs.h"
#include <array>
#include <deque>
#include <glad/glad.h>

class IEntityTexture;
namespace ecs {

namespace textures {
struct Rectangle {
  glm::vec2 orig;
  glm::vec2 size;
};
static constexpr glm::fvec2 default_tyle_size = {16.f, 16.f};
struct Player {
  enum class Dir : int { Down, Up, Left, Right };
  static constexpr Rectangle getPlayerTextureRectangle(Dir dir, bool idle,
                                                       int index) {
    int layer = static_cast<int>(dir);
    int column = !(int)idle * 2 + index;
    return Rectangle{
        .orig = {default_tyle_size.x * column * 3 + default_tyle_size.x,
                 default_tyle_size.y * layer * 3 + default_tyle_size.y},
        .size = {default_tyle_size.x, default_tyle_size.y}};
  }
};

static constexpr Rectangle grass_0 = {
    .orig = {default_tyle_size.x, default_tyle_size.y},
    .size = {default_tyle_size.x, default_tyle_size.y}};
static constexpr Rectangle bush_with_berries = {
    .orig = {0.f, default_tyle_size.y * 3.f},
    .size = {default_tyle_size.x, default_tyle_size.y}};
static constexpr Rectangle bush_no_berries = {
    .orig = {default_tyle_size.x, default_tyle_size.y * 3},
    .size = {default_tyle_size.x, default_tyle_size.y}};
static constexpr Rectangle water_0 = {
    .orig = {0, 0}, .size = {default_tyle_size.x, default_tyle_size.y}};
}; // namespace textures
enum class TextureKeys : size_t { Player, Grass, Bush, Water, Max };

enum class TextureLayers : size_t {
  L0,
  L1,
  L2,
  Max,
};

struct TextureSrc {
  TextureKeys key;
  textures::Rectangle rect;
};

struct Texture2DBundle {
  TextureSrc src;
  glm::fvec2 size;
  ///     Color color;
  float rotation;
};

struct Texture2DCreateBundle {
  unsigned int internal_format;
  unsigned int image_format;

  unsigned int wrap_s;
  unsigned int wrap_t;

  unsigned int filter_min;
  unsigned int filter_max;
};

struct Texture2D {
    Texture2D() { m_id = std::numeric_limits<GLuint>::max(); }
    Texture2D(std::string path, Texture2DCreateBundle createBundle);
    void bind();
    Texture2D(const Texture2D& ref){
        *this = ref;
    }
    Texture2D& operator=(const Texture2D& ref){
        this->m_id = ref.m_id;
        this->m_bundle = ref.m_bundle;
        this->m_width = ref.m_width;
        this->m_height = ref.m_height;
        this->m_tex_channels = ref.m_tex_channels;
        return *this;
    }
    ~Texture2D();

    Texture2DCreateBundle m_bundle;
    int m_width, m_height, m_tex_channels;
    GLuint m_id;
};

class TextureAtlas {
public:
  TextureAtlas();
  ~TextureAtlas();
  Texture2D getTexture(TextureKeys) const;
  void init();

private:
  std::array<Texture2D, static_cast<size_t>(TextureKeys::Max)> textures;
};
} // namespace ecs
