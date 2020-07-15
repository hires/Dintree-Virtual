/*
 * Dintree Plugin Top
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2020: Andrew Kilpatrick
 *
 * This file is part of Dintree-Virtual.
 *
 * Dintree-Virtual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dintree-Virtual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dintree-Virtual.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "plugin.hpp"

Plugin* pluginInstance;

// init!
void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelV100_Scanner);
    p->addModel(modelV101_Dual_Envelope);
    p->addModel(modelV102_Output_Mixer);
    p->addModel(modelV103_Reverb_Delay);
    p->addModel(modelV104_Four_Vs);
    p->addModel(modelV105_Quad_CV_Proc);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

// load module defaults for Dintree modules
void loadDefaults(struct ModuleDefaults *def) {
    // load the defaults
	std::string defaultsFilename = asset::user("Dintree.json");
	FILE *file = fopen(defaultsFilename.c_str(), "r");
    // no file - save defaults
	if(!file) {
        def->darkTheme = 0;
        saveDefaults(def);
        return;
	}

	json_error_t error;
	json_t *j = json_loadf(file, 0, &error);
	if(!j) {
		// invalid setting json file
		fclose(file);
        def->darkTheme = 0;
        saveDefaults(def);
        return;
	}

	json_t *jDarkTheme = json_object_get(j, "darkTheme");
	if(jDarkTheme) {
        def->darkTheme = json_boolean_value(jDarkTheme);
    }

	fclose(file);
	json_decref(j);
}

// save module defaults for Dintree modules
void saveDefaults(struct ModuleDefaults *def) {
	json_t *j = json_object();

    // create JSON object from supplied data
	json_object_set_new(j, "darkTheme", json_boolean(def->darkTheme));

    // save it
	std::string defaultsFilename = asset::user("Dintree.json");
	FILE *file = fopen(defaultsFilename.c_str(), "w");
	if(file) {
		json_dumpf(j, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
		fclose(file);
	}
	json_decref(j);
}
