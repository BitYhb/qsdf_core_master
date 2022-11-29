#pragma once

namespace Core::Constants {

// actions
const char DEFAULT[] = "QSDF.Default";
const char UNDO[] = "QSDF.Undo";
const char REDO[] = "QSDF.Redo";

const int TITLEBAR_HEIGHT = 32;                    // 标题栏高度
const int TITLEBAR_LOGO_WIDTH = 14;                // 标题栏LOGO宽度
const int TITLEBAR_LOGO_HEIGHT = 14;               // 标题栏LOGO高度
const int PATIENT_INFORMATION_CARD_WIDTH = 168;    // 病例信息面板宽度
const int PATIENT_INFORMATION_CARD_HEIGHT = 82;    // 病例信息面板高度
const int UNIVERSAL_TOOLBAR_WIDTH = 54;            // 通用工具栏宽度
const int SYSTEM_TOOLBAR_WIDTH = 328;              // 系统工具栏宽度
const int TOOLSPANEL_WIDTH = SYSTEM_TOOLBAR_WIDTH; // 工具面板宽度

// toolbar
const char G_GENERAL[] = "QSDF.General";
const char G_SYSTEM[] = "QSDF.System";

// General tools menu groups
const char G_GENERAL_DEFAULT[] = "QSDF.General.Default";
const char G_GENERAL_OTHER[] = "QSDF.General.Other";

// System tools menu groups(default)
const char G_SYSTEM_SAVE[] = "QSDF.System.Save";

// Contexts
const char C_GLOBAL[] = "Global Context";

const char SETTINGS_THEME[] = "Core/Theme";
const char DEFAULT_THEME[] = "flat";
const char DEFAULT_DARK_THEME[] = "flat-dark";

} // namespace Core::Constants
