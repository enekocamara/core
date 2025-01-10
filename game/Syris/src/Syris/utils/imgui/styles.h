#pragma once
#include "Syris/include/imgui.h"
#include <string>
#include <locale>
#include <codecvt>

namespace Syris{
    struct ImGuiUniqueStyle{

    };
    inline std::string WStringToUTF8(const std::wstring &wstr)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wstr);
    }
    inline bool centered_button(const char *label)
    {
        // Get the size of the label text
        ImVec2 textSize = ImGui::CalcTextSize(label);

        // Add padding to match ImGui's default button style
        ImVec2 padding = ImGui::GetStyle().FramePadding;
        ImVec2 buttonSize = ImVec2(textSize.x + padding.x * 2, textSize.y + padding.y * 2);

        // Get the available space in the window
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        ImVec2 cursorPos = ImGui::GetCursorPos();

        // Calculate position to center the button
        float centeredX = (windowSize.x - buttonSize.x) * 0.5f + cursorPos.x;
        float centeredY = (windowSize.y - buttonSize.y) * 0.5f + cursorPos.y;

        // Set the position and draw the button
        ImGui::SetCursorPos(ImVec2(centeredX, centeredY));
        return ImGui::Button(label, buttonSize);
    }
}