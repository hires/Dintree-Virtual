/*
 * Kilpatrick Audio Menu Helper
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 */
#ifndef MENU_HELPER
#define MENU_HELPER

#include "../plugin.hpp"

// add a spacer to a menu
void menuHelperAddSpacer(Menu *menu);

// add a label to a menu
void menuHelperAddLabel(Menu *menu, std::string text);

// add an item to the menu - just a front end for addChild()
void menuHelperAddItem(Menu *menu, MenuItem *item);

#endif
