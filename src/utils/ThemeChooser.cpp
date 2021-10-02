/*
 * Kilpatrick Audio Theme Chooser
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 */
#include "ThemeChooser.h"
#include "JsonHelper.h"
#include "../plugin.hpp"

int theme_changed = 0;

// create a theme chooser menu item
ThemeChooserMenuItem::ThemeChooserMenuItem(std::string name, int theme, ThemeChooserHandler *handler) : MenuItem() {
    this->text = name;
    this->rightText = CHECKMARK(theme == handler->themeChooserGetSelected());
    this->theme = theme;
    this->handler = handler;
}

// the menu item was selected
void ThemeChooserMenuItem::onAction(const event::Action &e) {
    handler->themeChooserSetSelected(theme);
}

// create a theme chooser to handle choosing themes
ThemeChooser::ThemeChooser(ModuleWidget *module_widget, std::string themeFilename,
        std::string defaultThemeName, const std::string& defaultFilename) {
    this->module_widget = module_widget;
    this->themeFilename = themeFilename;
    module_widget->setPanel(APP->window->loadSvg(defaultFilename));
    panels.push_back(module_widget->getPanel());
    panelNames.push_back(defaultThemeName);
    theme = 0;
    selectedTheme = -1;  // disable
    currentPanel = module_widget->getPanel();
    changeSource = 0;
}

// add a panel to choose
void ThemeChooser::addPanel(std::string themeName, const std::string& filename) {
    SvgPanel *panel = new SvgPanel();
    panel->setBackground(APP->window->loadSvg(filename));
    panel->visible = false;
    module_widget->addChild(panel);
    panels.push_back(panel);
    panelNames.push_back(themeName);
}

// init the panel with the loaded theme
void ThemeChooser::initPanel(void) {
    loadThemeFile();
    if(theme != 0) {
        changeTheme(theme);
    }
}

// add a set of theme chooser items to the menu
void ThemeChooser::populateThemeChooserMenuItems(Menu *menu) {
    menuHelperAddSpacer(menu);
    menuHelperAddLabel(menu, "Theme");
    for(int i = 0; i < (int)panels.size(); i ++) {
        menuHelperAddItem(menu, new ThemeChooserMenuItem(panelNames[i], i,
            (ThemeChooserHandler *)this));
    }
}

// get the currently selected theme
int ThemeChooser::themeChooserGetSelected(void) {
    return theme;
}

// handle the theme being chosen
void ThemeChooser::themeChooserSetSelected(int theme) {
    selectedTheme = theme;
    changeSource = 1;
}

// check if theme was changed
void ThemeChooser::step() {
    // see if we have selected a new theme from the menu
    if(selectedTheme != -1 && theme != selectedTheme) {
        changeTheme(selectedTheme);
        saveThemeFile();
        selectedTheme = -1;
        theme_changed = 1;
    }
    // reset theme change after 1 cycle if we were the source
    else if(changeSource) {
        changeSource = 0;
        theme_changed = 0;
    }

    // did another module change the global theme?
    if(!changeSource && theme_changed) {
        loadThemeFile();
        changeTheme(theme);  // change to theme but don't save back
    }
}

//
// private methods
//
// load the theme file for the plugin
void ThemeChooser::loadThemeFile(void) {
    json_t *j = NULL;

    // load the content
    std::string memFilename = asset::user(themeFilename);
    FILE *file = fopen(memFilename.c_str(), "r");

    // no file - init defaults and save
    if(!file) {
        goto error_init;
    }

    // file load error - init defaults and save
    json_error_t error;
    j = json_loadf(file, 0, &error);
    if(!j) {
        fclose(file);
        goto error_init;
    }

    if(jsonHelperLoadInt(j, "theme", &theme) == -1) {
        goto error_init;
    }

    fclose(file);
    json_decref(j);
    return;

error_init:
    theme = 0;
    saveThemeFile();
}

// save the theme file
void ThemeChooser::saveThemeFile(void) {
    json_t *j = json_object();

    jsonHelperSaveInt(j, "theme", theme);

    // save json objects
    std::string memFilename = asset::user(themeFilename);
    FILE *file = fopen(memFilename.c_str(), "w");
    if(file) {
        json_dumpf(j, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
        fclose(file);
    }
    json_decref(j);
}

// change themes - doesn't save the change
void ThemeChooser::changeTheme(int theme) {
    if(theme < 0 || theme >= (int)panels.size()) {
        return;
    }
    currentPanel->visible = 0;
    currentPanel = panels[theme];
    currentPanel->visible = 1;
    this->theme = theme;
}
