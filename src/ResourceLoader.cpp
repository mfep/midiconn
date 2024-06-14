#include "ResourceLoader.hpp"

#include "IconsForkAwesome.h"
#include "SDL2/SDL.h"
#include "cmrc/cmrc.hpp"
#include "imgui.h"
#include "spng.h"

#include <cstddef>
#include <vector>

CMRC_DECLARE(midiconn_resources);
bool ImGui_ImplSDLRenderer2_CreateFontsTexture();

namespace mc
{

Texture::~Texture()
{
    if (m_texture != nullptr)
    {
        SDL_DestroyTexture(m_texture);
    }
}

Texture ResourceLoader::load_texture(SDL_Renderer* renderer, const std::string& path)
{
    auto       embedded_fs = cmrc::midiconn_resources::get_filesystem();
    const auto image_file  = embedded_fs.open(path);

    auto ctx = spng_ctx_new(0);
    if (ctx == nullptr)
    {
        throw std::runtime_error("Error creating SPNG context");
    }
    int error = spng_set_png_buffer(ctx, image_file.begin(), image_file.size());
    if (error)
    {
        throw std::runtime_error("Error setting SPNG buffer");
    }
    size_t image_bytes{};
    error = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &image_bytes);
    if (error)
    {
        throw std::runtime_error("Error getting SPNG image size");
    }
    std::vector<unsigned char> image_data;
    image_data.resize(image_bytes);
    error = spng_decode_image(ctx, image_data.data(), image_bytes, SPNG_FMT_RGBA8, 0);
    if (error)
    {
        throw std::runtime_error("Error decoding SPNG image");
    }
    spng_ihdr ihdr{};
    error = spng_get_ihdr(ctx, &ihdr);
    if (error)
    {
        throw std::runtime_error("Error getting SPNG ihdr");
    }
    spng_ctx_free(ctx);

    auto texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, ihdr.width, ihdr.height);
    error = SDL_UpdateTexture(texture, nullptr, image_data.data(), ihdr.width * 4);
    if (error)
    {
        throw std::runtime_error("Error updating SDL texture");
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    if (error)
    {
        throw std::runtime_error("Error setting SDL texture blending mode");
    }

    return {texture};
}

void ResourceLoader::load_fonts(const float scale)
{
    auto& io = ImGui::GetIO();
    io.Fonts->Clear();

    auto       embedded_fs = cmrc::midiconn_resources::get_filesystem();
    const auto font_file   = embedded_fs.open("fonts/DroidSans.ttf");

    ImFontConfig font_config{};
    font_config.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(const_cast<char*>(font_file.begin()),
                                   std::distance(font_file.begin(), font_file.end()),
                                   16 * scale,
                                   &font_config);

    const auto    icons_file     = embedded_fs.open("fonts/forkawesome-webfont.ttf");
    const ImWchar icons_ranges[] = {ICON_MIN_FK, ICON_MAX_16_FK, 0};
    ImFontConfig  icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(const_cast<char*>(icons_file.begin()),
                                   std::distance(icons_file.begin(), icons_file.end()),
                                   12 * scale,
                                   &icons_config,
                                   icons_ranges);

    // workaround, otherwise the fonts won't rebuild properly
    ImGui_ImplSDLRenderer2_CreateFontsTexture();
}

} // namespace mc
