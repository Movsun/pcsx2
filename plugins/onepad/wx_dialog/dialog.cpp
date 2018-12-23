/*  dialog.cpp
 *  PCSX2 Dev Team
 *  Copyright (C) 2015
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "dialog.h"
#include <array>

static std::string KeyName(int pad, int key, int keysym)
{
    // Mouse
    if (keysym < 10)
    {
        switch (keysym)
        {
            case 0:
                return "";
            case 1:
                return "Mouse Left";
            case 2:
                return "Mouse Middle";
            case 3:
                return "Mouse Right";
            default: // Use only number for extra button
                return "Mouse " + std::to_string(keysym);
        }
    }

    return std::string(XKeysymToString(keysym));
}

// Constructor of Dialog
Dialog::Dialog()
    : wxDialog(NULL,                                  // Parent
               wxID_ANY,                              // ID
               _T("OnePad configuration"),            // Title
               wxDefaultPosition,                     // Position
               wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT), // Width + Lenght
               // Style
               wxSYSTEM_MENU |
                   wxCAPTION |
                   wxCLOSE_BOX |
                   wxCLIP_CHILDREN)
{
    /*
     * Define the size and the position of each button :
     * padding[ButtonID] : Width, Height, x position, y position
    */
    std::array<std::array<int, 4>, BUTTONS_LENGTH> padding;

    padding[PAD_L1]         = { 218, 28,  50, 175 };
    padding[PAD_L2]         = { 218, 28,  50, 104 };
    padding[PAD_R1]         = { 218, 28, 726, 175 };
    padding[PAD_R2]         = { 218, 28, 726, 104 };
    padding[PAD_R3]         = { 218, 28, 498, 641 };

    padding[PAD_TRIANGLE]   = { 218, 28, 726, 246 };
    padding[PAD_CIRCLE]     = { 218, 28, 726, 319 };
    padding[PAD_CROSS]      = { 218, 28, 726, 391 };
    padding[PAD_SQUARE]     = { 218, 28, 726, 463 };

    padding[PAD_START]      = { 218, 28, 503, 34 };
    padding[PAD_SELECT]     = { 218, 28, 273, 34 };

    padding[Analog]         = { 218, 28, 50, 452 };

    // Directional Pad
    padding[PAD_UP]         = { 100, 25, 108, 290 };
    padding[PAD_DOWN]       = { 100, 25, 108, 340 };
    padding[PAD_RIGHT]      = { 109, 25, 159, 315 };
    padding[PAD_LEFT]       = { 109, 25,  50, 315 };

    // Left Joystick
    padding[PAD_L_UP]       = { 100, 25, 325, 527 };
    padding[PAD_L_DOWN]     = { 100, 25, 325, 577 };
    padding[PAD_L_RIGHT]    = { 109, 25, 377, 552 };
    padding[PAD_L_LEFT]     = { 109, 25, 268, 552 };

    padding[PAD_L3]         = { 218, 28, 268, 641 };

    // Right Joystick
    padding[PAD_R_UP]       = { 100, 25, 555, 527 };
    padding[PAD_R_DOWN]     = { 100, 25, 555, 577 };
    padding[PAD_R_RIGHT]    = { 109, 25, 607, 552 };
    padding[PAD_R_LEFT]     = { 109, 25, 498, 552 };

    padding[JoyL_config]    = { 180, 28,  50, 550 };
    padding[JoyR_config]    = { 180, 28, 764, 550 };
    padding[Gamepad_config] = { 180, 28,  50, 585 };
    padding[Set_all]        = { 180, 28, 764, 585 };

    padding[Apply]          = {  70, 28, 833, 642 };
    padding[Ok]             = {  70, 28, 913, 642 };
    padding[Cancel]         = {  70, 28, 753, 642 };

    // create a new Notebook
    m_tab_gamepad = new wxNotebook(this, wxID_ANY);
    for (int i = 0; i < GAMEPAD_NUMBER; ++i)
    {
        // Tabs panels
        m_pan_tabs[i] = new opPanel(
            m_tab_gamepad,
            wxID_ANY,
            wxDefaultPosition,
            wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT));

        // New page creation
        m_tab_gamepad->AddPage(
            m_pan_tabs[i],                           // Parent
            wxString::Format(_("Gamepad %i"), i));

        for (int j = 0; j < BUTTONS_LENGTH; ++j)
        {
            // Gamepad buttons
            m_bt_gamepad[i][j] = new wxButton(
                m_pan_tabs[i],                         // Parent
                wxID_HIGHEST + j + 1,                  // ID
                _T("Undefined"),                       // Label
                wxPoint(padding[j][2], padding[j][3]), // Position
                wxSize(padding[j][0], padding[j][1])   // Size
                );
        }

        // Set the other buttons labels
        m_bt_gamepad[i][JoyL_config]->SetLabel(_T("&Left Joystick Config"));
        m_bt_gamepad[i][JoyR_config]->SetLabel(_T("&Right Joystick Config"));
        m_bt_gamepad[i][Gamepad_config]->SetLabel(_T("&Gamepad Configuration"));
        m_bt_gamepad[i][Set_all]->SetLabel(_T("&Set All Buttons"));
        m_bt_gamepad[i][Cancel]->SetLabel(_T("&Cancel"));
        m_bt_gamepad[i][Apply]->SetLabel(_T("&Apply"));
        m_bt_gamepad[i][Ok]->SetLabel(_T("&Ok"));

        // Disable analog button (not yet supported)
        m_bt_gamepad[i][Analog]->Disable();
    }

    Bind(wxEVT_BUTTON, &Dialog::OnButtonClicked, this);

    for (int i = 0; i < GAMEPAD_NUMBER; ++i)
    {
        for (int j = 0; j < NB_IMG; ++j)
        {
            m_pressed[i][j] = false;
        }
    }
}

void Dialog::InitDialog()
{
    GamePad::EnumerateGamePads(s_vgamePad); // activate gamepads
    LoadConfig();                           // Load configuration from the ini file
    repopulate();                           // Set label and fit simulated key array
}

/****************************************/
/*********** Events functions ***********/
/****************************************/

void Dialog::OnButtonClicked(wxCommandEvent &event)
{
    // Display a message each time the button is clicked
    wxButton *bt_tmp = (wxButton *)event.GetEventObject(); // get the button object
    int bt_id = bt_tmp->GetId() - wxID_HIGHEST - 1;        // get the real ID
    int gamepad_id = m_tab_gamepad->GetSelection();        // get the tab ID (equivalent to the gamepad id)

    switch (bt_id)
    {
        case Gamepad_config:
        {
            GamepadConfiguration gamepad_config(gamepad_id, this);

            gamepad_config.InitGamepadConfiguration();
            gamepad_config.ShowModal();
            break;
        }

        case JoyL_config:
        {
            JoystickConfiguration joystick_config(gamepad_id, true, this);

            joystick_config.InitJoystickConfiguration();
            joystick_config.ShowModal();
            break;
        }

        case JoyR_config:
        {
            JoystickConfiguration joystick_config(gamepad_id, false, this);

            joystick_config.InitJoystickConfiguration();
            joystick_config.ShowModal();
            break;
        }

        case Set_all:
            for (int i = 0; i < MAX_KEYS; ++i)
            {
                bt_tmp = m_bt_gamepad[gamepad_id][i];

                set_tab_img_visible(gamepad_id, i, true);
                m_pan_tabs[gamepad_id]->Refresh();
                m_pan_tabs[gamepad_id]->Update();

                config_key(gamepad_id, i);

                set_tab_img_visible(gamepad_id, i, false);
                m_pan_tabs[gamepad_id]->Refresh();
                m_pan_tabs[gamepad_id]->Update();

                usleep(500000); // give enough time to the user to release the button
            }
            break;

        case Ok:
            SaveConfig();
            Close();
            break;

        case Apply:
            SaveConfig();
            break;

        case Cancel:
            Close();
            break;

        default:
            if (bt_id >= 0 && bt_id <= PAD_R_LEFT) // if the button ID is a gamepad button
            {
                bt_tmp->Disable();
                config_key(gamepad_id, bt_id);
                bt_tmp->Enable();
            }
            break;
    }
}

/****************************************/
/*********** Methods functions **********/
/****************************************/

void Dialog::config_key(int pad, int key)
{
    bool captured = false;
    u32 key_pressed = 0;

    while (!captured)
    {
        if (PollX11KeyboardMouseEvent(key_pressed))
        {
            // special case for keyboard/mouse to handle multiple keys
            // Note: key_pressed == 0 when ESC is hit to abort the capture
            if (key_pressed > 0)
            {
                clear_key(pad, key);
                set_keyboard_key(pad, key_pressed, key);
                m_simulatedKeys[pad][key] = key_pressed;
            }
            captured = true;
        }
    }

    m_bt_gamepad[pad][key]->SetLabel(KeyName(pad, key, m_simulatedKeys[pad][key]).c_str());
}

void Dialog::clear_key(int pad, int key)
{
    // Erase the keyboard binded key
    u32 keysim = m_simulatedKeys[pad][key];
    m_simulatedKeys[pad][key] = 0;

    // erase gamepad entry (keysim map)
    g_conf.keysym_map[pad].erase(keysim);
}

// Set button values
void Dialog::repopulate()
{
    for (int gamepad_id = 0; gamepad_id < GAMEPAD_NUMBER; ++gamepad_id)
    {
        // keyboard/mouse key
        for (const auto &it : g_conf.keysym_map[gamepad_id])
        {
            int keysym = it.first;
            int key = it.second;

            m_bt_gamepad[gamepad_id][key]->SetLabel(KeyName(gamepad_id, key, keysym).c_str());

            m_simulatedKeys[gamepad_id][key] = keysym;
        }
    }
}

// We could probably just make a table and avoid the switch statement...
void Dialog::set_tab_img_visible(int gamepad_id, int i, bool visible)
{
    if (visible)
    {
        switch (i)
        {
            case PAD_L_UP: // Left joystick (Up) ↑
                m_pan_tabs[gamepad_id]->ShowImg(img_l_arrow_up);
                break;

            case PAD_L_RIGHT: // Left joystick (Right) →
                m_pan_tabs[gamepad_id]->ShowImg(img_l_arrow_right);
                break;

            case PAD_L_DOWN: // Left joystick (Down) ↓
                m_pan_tabs[gamepad_id]->ShowImg(img_l_arrow_bottom);
                break;

            case PAD_L_LEFT: // Left joystick (Left) ←
                m_pan_tabs[gamepad_id]->ShowImg(img_l_arrow_left);
                break;

            case PAD_R_UP: // Right joystick (Up) ↑
                m_pan_tabs[gamepad_id]->ShowImg(img_r_arrow_up);
                break;

            case PAD_R_RIGHT: // Right joystick (Right) →
                m_pan_tabs[gamepad_id]->ShowImg(img_r_arrow_right);
                break;

            case PAD_R_DOWN: // Right joystick (Down) ↓
                m_pan_tabs[gamepad_id]->ShowImg(img_r_arrow_bottom);
                break;

            case PAD_R_LEFT: // Right joystick (Left) ←
                m_pan_tabs[gamepad_id]->ShowImg(img_r_arrow_left);
                break;

            default:
                m_pan_tabs[gamepad_id]->ShowImg(i);
                break;
        }
    }
    else
    {
        switch (i)
        {
            case PAD_L_UP: // Left joystick (Up) ↑
                m_pan_tabs[gamepad_id]->HideImg(img_l_arrow_up);
                break;

            case PAD_L_RIGHT: // Left joystick (Right) →
                m_pan_tabs[gamepad_id]->HideImg(img_l_arrow_right);
                break;

            case PAD_L_DOWN: // Left joystick (Down) ↓
                m_pan_tabs[gamepad_id]->HideImg(img_l_arrow_bottom);
                break;

            case PAD_L_LEFT: // Left joystick (Left) ←
                m_pan_tabs[gamepad_id]->HideImg(img_l_arrow_left);
                break;

            case PAD_R_UP: // Right joystick (Up) ↑
                m_pan_tabs[gamepad_id]->HideImg(img_r_arrow_up);
                break;

            case PAD_R_RIGHT: // Right joystick (Right) →
                m_pan_tabs[gamepad_id]->HideImg(img_r_arrow_right);
                break;

            case PAD_R_DOWN: // Right joystick (Down) ↓
                m_pan_tabs[gamepad_id]->HideImg(img_r_arrow_bottom);
                break;

            case PAD_R_LEFT: // Right joystick (Left) ←
                m_pan_tabs[gamepad_id]->HideImg(img_r_arrow_left);
                break;

            default:
                m_pan_tabs[gamepad_id]->HideImg(i);
                break;
        }
    }
}

// Main
void DisplayDialog()
{
    if (g_conf.ftw)
    {
        wxString info("The OnePad GUI is provided to map the keyboard/mouse to the virtual PS2 pad.\n\n"
                      "Gamepads/Joysticks are plug and play. The active gamepad can be selected in the 'Gamepad Configuration' panel.\n\n"
                      "If you prefer to manually map your gamepad, you should use the 'onepad-legacy' plugin instead.");

        wxMessageDialog ftw(nullptr, info);
        ftw.ShowModal();

        g_conf.ftw = 0;
        SaveConfig();
    }

    Dialog dialog;

    dialog.InitDialog();
    dialog.ShowModal();
}
