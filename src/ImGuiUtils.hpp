#include "imgui.h"

namespace mc::utils
{

inline ImVec4 color_lerp(const ImVec4& a, const ImVec4& b, float percent)
{
    return {a.x * (1 - percent) + b.x * percent,
            a.y * (1 - percent) + b.y * percent,
            a.z * (1 - percent) + b.z * percent,
            a.w * (1 - percent) + b.w * percent};
}

} // namespace mc::utils
