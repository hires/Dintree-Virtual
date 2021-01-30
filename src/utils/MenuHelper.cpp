/*
 * Kilpatrick Audio Menu Helper
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 */
 #include "../plugin.hpp"
#include "MenuHelper.h"

// add a spacer to a menu
void menuHelperAddSpacer(Menu *menu) {
    menu->addChild(new MenuLabel());
}

// add a label to a menu
void menuHelperAddLabel(Menu *menu, std::string text) {
    MenuLabel *label = new MenuLabel();
    label->text = text;
    menu->addChild(label);
}

// add an item to the menu - just a front end for addChild()
void menuHelperAddItem(Menu *menu, MenuItem *item) {
    menu->addChild(item);
}
