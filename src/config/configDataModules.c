#include <string.h>
#include "configHelpers.h"
#include "configDataModules.h"
#include "../structs/rpiNode.h"
#include "../utils/nameValue.h"

void config_data_modules(json_t *dataModules) {

    if (json_is_array(dataModules)) {
        
        size_t module_index, component_index, nv_index;
        json_t  *element, *sub_element, *nv_element, *m_name, *c_name, *s_name,
                *s_value, *start, *verbose, *uniqueId, *components, *settings;
        
        // DataModules
        json_array_foreach(dataModules, module_index, element) {
            m_name = json_object_get(element, "name");
            start = json_object_get(element, "start");
            verbose = json_object_get(element, "verbose");
            uniqueId = json_object_get(element, "uniqueId");
            components = json_object_get(element, "components");
     
            // Set Module @ module_index
            DataModule *dest = &rpiNode.data[module_index];
            dest->flag = 1;
            dest->start = get_true_false(0, start);
            dest->verbose = get_true_false(0, verbose);
            dest->uniqueId = get_integer(dest->uniqueId, uniqueId);
            rpiNode.data_count++;
            SAFE_STRCPY(dest->name, get_string("id", dest->name, m_name, RPI_MAX_DATACONTAINER_NAME));
     
            // Components
            if (json_is_array(components)) {
                json_array_foreach(components, component_index, sub_element) {
                    c_name = json_object_get(sub_element, "name");
                    settings = json_object_get(sub_element, "settings");
                    dest->components_count++;
                    //Set Component @ component_index
                    DataModule *c_dest = &rpiNode.data[component_index];
                    SAFE_STRCPY(c_dest->name, get_string("id", c_dest->name, c_name, RPI_MAX_DATACOMPONENT_NAME));
     
                    // Component settings
                    if (json_is_array(settings)) {
                        json_array_foreach(settings, nv_index, nv_element) {
                            s_name = json_object_get(nv_element, "name");
                            s_value = json_object_get(nv_element, "value");
     
                            // Add/set to settings pointer list
                            if (json_is_string(s_name) && json_is_string(s_value)) {
                                namevalue_add(
                                    &rpiNode.data[module_index].components[component_index].settings,
                                    json_string_value(s_name), json_string_value(s_value)
                                );
                                //printf("--%s\n", rpiNode.data[module_index].components[component_index].settings->name);
                            }
                        }
                    }
                }
            }
        }
    }

}