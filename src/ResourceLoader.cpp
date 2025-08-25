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
namespace
{

constexpr ImWchar droid_sans_glyph_ranges[]{
    // clang-format off
    0x20, 0x7e,
    0xa0, 0x17f,
    0x192, 0x192,
    0x1a0, 0x1a1,
    0x1af, 0x1b0,
    0x1f0, 0x1f0,
    0x1fa, 0x1ff,
    0x218, 0x21b,
    0x2bc, 0x2bc,
    0x2c6, 0x2c7,
    0x2c9, 0x2c9,
    0x2d8, 0x2dd,
    0x2f3, 0x2f3,
    0x300, 0x301,
    0x303, 0x303,
    0x309, 0x309,
    0x30f, 0x30f,
    0x323, 0x323,
    0x384, 0x38a,
    0x38c, 0x38c,
    0x38e, 0x3a1,
    0x3a3, 0x3ce,
    0x3d1, 0x3d2,
    0x3d6, 0x3d6,
    0x400, 0x486,
    0x488, 0x513,
    0x1e00, 0x1e01,
    0x1e3e, 0x1e3f,
    0x1e80, 0x1e85,
    0x1ea0, 0x1ef9,
    0x1f4d, 0x1f4d,
    0x2000, 0x200b,
    0x2013, 0x2015,
    0x2017, 0x201e,
    0x2020, 0x2022,
    0x2026, 0x2026,
    0x2030, 0x2030,
    0x2032, 0x2033,
    0x2039, 0x203a,
    0x203c, 0x203c,
    0x2044, 0x2044,
    0x207f, 0x207f,
    0x20a3, 0x20a4,
    0x20a7, 0x20a7,
    0x20ab, 0x20ac,
    0x2105, 0x2105,
    0x2113, 0x2113,
    0x2116, 0x2116,
    0x2122, 0x2122,
    0x2126, 0x2126,
    0x212e, 0x212e,
    0x215b, 0x215e,
    0x2202, 0x2202,
    0x2206, 0x2206,
    0x220f, 0x220f,
    0x2211, 0x2212,
    0x221a, 0x221a,
    0x221e, 0x221e,
    0x222b, 0x222b,
    0x2248, 0x2248,
    0x2260, 0x2260,
    0x2264, 0x2265,
    0x25ca, 0x25ca,
    0xfb01, 0xfb04,
    0xfeff, 0xfeff,
    0xfffc, 0xfffd,
    0
    // clang-format on
};

}

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
    [[maybe_unused]] static ImFont* font = [] {
        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        auto       embedded_fs = cmrc::midiconn_resources::get_filesystem();
        const auto font_file   = embedded_fs.open("fonts/DroidSans.ttf");

        ImFontConfig font_config{};
        font_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF(const_cast<char*>(font_file.begin()),
                                    std::distance(font_file.begin(), font_file.end()),
                                    16,
                                    &font_config,
                                    droid_sans_glyph_ranges);

        const auto    icons_file     = embedded_fs.open("fonts/forkawesome-webfont.ttf");
        const ImWchar icons_ranges[] = {ICON_MIN_FK, ICON_MAX_16_FK, 0};
        ImFontConfig  icons_config;
        icons_config.MergeMode            = true;
        icons_config.PixelSnapH           = true;
        icons_config.FontDataOwnedByAtlas = false;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(const_cast<char*>(icons_file.begin()),
                                    std::distance(icons_file.begin(), icons_file.end()),
                                    12,
                                    &icons_config,
                                    icons_ranges);
        ImGui::PushFont(font, 16);
        return font;
    }();
    ImGui::PushFont(nullptr, 16 * scale);
}

} // namespace mc
