#include "stdafx.h"
#include "Extern/imgui/imgui.h"
#include "imguiConsole.h"
#include <string>
#include <iostream>

void ExctractSigArgs(const std::string& c, std::string& sig, std::string& args)
{
    int i = 0;
    while (i < c.size() && c[i] != ' ') i++;
    sig = c.substr(0, i);
    args = (((int)c.size() - i - 1) > 0) ? c.substr(i + 1, c.size() - i) : std::string();
}

void Console::Process()
{
    for (Command* c : m_Commands)
    {
        delete c;
    }
    m_Commands.clear();

    if (!m_Open)
        return;
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console", &m_Open))
    {
        ImGui::End();
        return;
    }

    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            m_Open = false;
        ImGui::EndPopup();
    }

    //ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    //ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

    // TODO: display items starting from the bottom

    //if (ImGui::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } ImGui::SameLine();
    //if (ImGui::SmallButton("Add Dummy Error")) { AddLog("[error] something went wrong"); } ImGui::SameLine();
    //if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    /*if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }*/

    // Options, Filter
    /*if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();*/

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
    if (ImGui::BeginPopupContextWindow())
    {
        //if (ImGui::Selectable("Clear")) ClearLog();
        ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
    // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
    // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
    //     ImGuiListClipper clipper(Items.Size);
    //     while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
    // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
    // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
    // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < m_Log.size(); i++)
    {
        const char* item = m_Log[i].c_str();
        //if (!Filter.PassFilter(item))
        //    continue;

        // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
        //bool pop_color = false;
        //if (strstr(item, "[error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
        //else if (strncmp(item, "# ", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
        ImGui::TextUnformatted(item);
        //if (pop_color)
        //    ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    //if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
    //    ImGui::SetScrollHereY(1.0f);
    //ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    if (ImGui::InputText("Input", m_InputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue /*| ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory*/, 0/*&TextEditCallbackStub*/, (void*)this))
    {
        std::string c = m_InputBuffer;
        m_Log.push_back(c);
        std::string sig;
        std::string args;
        ExctractSigArgs(c, sig, args);
        Produce(sig, args);
        //char* s = m_InputBuffer;
        //Strtrim(s);
        //if (s[0])
        //    ExecCommand(s);

        strcpy(m_InputBuffer, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}