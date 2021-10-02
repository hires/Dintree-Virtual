/*
 * Kilpatrick Audio Theme Chooser
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 */
#ifndef THEME_CHOOSER_H
#define THEME_CHOOSER_H

#include "MenuHelper.h"

// a callback handler for choosing themes
struct ThemeChooserHandler {
    // get the currently selected theme
    virtual int themeChooserGetSelected(void) { return 0; }

    // handle the theme being chosen
    virtual void themeChooserSetSelected(int theme) { }
};

// a theme chooser menu item
struct ThemeChooserMenuItem : MenuItem {
    int theme;
    ThemeChooserHandler *handler;

    // create a theme chooser menu item
    ThemeChooserMenuItem(std::string name, int theme, ThemeChooserHandler *handler);

    // the menu item was selected
    void onAction(const event::Action &e) override;
};

// theme chooser to handle loading and selecting themes
struct ThemeChooser : ThemeChooserHandler {
private:
    ModuleWidget *module_widget;
    std::vector<widget::Widget*> panels;
    std::vector<std::string> panelNames;
    widget::Widget *currentPanel;
    std::string themeFilename;
    int theme;  // current theme
    int selectedTheme;  // theme selected from menu
    int changeSource;

    // load the theme file for the plugin
    void loadThemeFile(void);

    // save the theme file
    void saveThemeFile(void);

    // change themes - doesn't save the change
    void changeTheme(int theme);

public:
    // create a theme chooser to handle choosing themes
    ThemeChooser(ModuleWidget *module_widget, std::string themeFilename,
            std::string defaultThemeName, const std::string& defaultFilename);

    // destructor
    virtual ~ThemeChooser() { }

    // add a panel to choose
    void addPanel(std::string themeName, const std::string& filename);

    // init the panel with the loaded theme
    void initPanel(void);

    // add a set of theme chooser items to the menu
    void populateThemeChooserMenuItems(Menu *menu);

    // get the currently selected theme
    int themeChooserGetSelected(void) override;

    // handle the theme being chosen
    void themeChooserSetSelected(int theme) override;

    // check if theme was changed
    void step();
};

#endif
