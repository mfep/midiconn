#include "MidiChannelNode.hpp"
#include "imgui.h"
#include "imnodes.h"

#include <iomanip>
#include <sstream>

namespace mc
{

void MidiChannelNode::render_internal()
{
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Channel map");
    imnodes::EndNodeTitleBar();
    for (size_t i = 0; i < 8; i++)
    {
        static int val[16];
        const char* items[] = { "None", "1", "2", "3", "4", "5", "6", "7", "8",
            "9", "10", "11", "12", "13", "14", "15", "16" };
        std::stringstream ss;
        ss << "from ch " << std::setw(2) << i * 2;
        ImGui::SetNextItemWidth(50);
        ImGui::Combo(ss.str().c_str(), &val[i * 2], items, 17);

        std::stringstream ss2;
        ss2 << "from ch " << std::setw(2) << i * 2 + 1;
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        ImGui::Combo(ss2.str().c_str(), &val[i * 2 + 1], items, 17);
    }
    
}

Node::channel_map MidiChannelNode::transform_channel_map(const channel_map& in_map)
{
    return {};
}

}
