#pragma once
#include <string>

struct SDL_Texture;
struct SDL_Renderer;

namespace mc
{

struct Texture
{
    SDL_Texture* m_texture{};

    Texture(SDL_Texture* texture) : m_texture(texture) {}

    Texture(const Texture&) = delete;
    Texture(Texture&& other)
    {
        m_texture       = other.m_texture;
        other.m_texture = nullptr;
    }

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other)
    {
        m_texture       = other.m_texture;
        other.m_texture = nullptr;
        return *this;
    }

    ~Texture();
};

struct ResourceLoader final
{
    ResourceLoader() = delete;

    static Texture load_texture(SDL_Renderer* renderer, const std::string& path);
    static void    load_fonts(const float scale);
};

} // namespace mc
