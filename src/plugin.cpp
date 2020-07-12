#include "plugin.hpp"

Plugin* pluginInstance;

// init!
void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelV100_Scanner);

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
