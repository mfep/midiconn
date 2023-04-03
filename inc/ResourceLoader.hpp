#pragma once
#include <string>

class SDL_Texture;
class SDL_Renderer;

namespace mc
{

struct Texture
{
    SDL_Texture* m_texture{};

    Texture(const Texture&) = delete;
    Texture(Texture&&)      = default;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&)      = default;

    ~Texture();
};

struct ResourceLoader final
{
    ResourceLoader() = delete;

    static Texture load_texture(SDL_Renderer* renderer, const std::string& path);
    static void    load_fonts(const float scale);
};

} // namespace mc
