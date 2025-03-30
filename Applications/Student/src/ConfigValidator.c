#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/ConfigValidator.h"



//TODO: add csv reader later? - Probs not
void init_config_spec(ConfigSpec* spec) {

}

/*
 * @param[in] Pointer to initialized ConfigSpec
 * @param[in] Pointer to single config_message (i.e. "ID: Columbia")
 * @param[out] ConfigValue struct containing the type and a union of its value
 **/
ConfigValue* validate_config_message(ConfigSpec* spec, char* config_message) {
    ConfigValue* configValue = (ConfigValue*)malloc(sizeof(ConfigValue));

    // Parse config message
    char** param_id, ** value_ptr;
    parse_config_message(config_message, param_id, value_ptr);

    // Find matching config spec
    ConfigItem* itemSpec;
    int i;
    bool found = false;
    while (i < spec->num_items && !found) {
        itemSpec = spec->configItems[i];
        if (itemSpec->param_id == **param_id) {
            found = true;
        }
    }

    // If matching expected param not found, return with valuetype INVALID
    if (!found) {
        configValue->type = INVALID;
        return configValue;
    }

    //TODO: make slightly more type safe?
    union ValueRange range = itemSpec->valueRange;
    switch (itemSpec->valueType) {
    case INTEGER:
        int value = atoi(*value_ptr);
        if (range.IntegerRange.min < value && value < range.IntegerRange.max) {
            configValue->type = INTEGER;
            configValue->int_value = value;
        }
    case DOUBLE:
        char* endptr;
        double value = strtod(*value_ptr, &endptr);

        //check for conversion errors
        if (endptr == *value_ptr || *endptr != '\0') {
            configValue->type = INVALID;
        }
        else if (range.IntegerRange.min < value && value < range.IntegerRange.max) {
            configValue->type = INTEGER;
            configValue->int_value = value;
        }
    case STRING:
    default:
        break;

    }

    return configValue;

}

void parse_config_message(char* message, char** param_id, char** value) {
    // Find the position of the colon
    const char* colon_pos = strchr(message, ':');
    if (colon_pos == NULL) {
        // Handle error: colon not found
        *param_id = NULL;
        *value = NULL;
        return;
    }

    // Calculate lengths of param_id and value
    size_t param_id_len = colon_pos - message;
    size_t value_len = strlen(colon_pos + 1);

    // Allocate memory for param_id and value
    *param_id = (char*)malloc(param_id_len + 1);
    *value = (char*)malloc(value_len + 1);

    if (*param_id == NULL || *value == NULL) {
        // Handle error: memory allocation failed
        free(*param_id);
        free(*value);
        *param_id = NULL;
        *value = NULL;
        return;
    }

    // Copy the strings into the allocated memory
    strncpy(*param_id, message, param_id_len);
    (*param_id)[param_id_len] = '\0'; // Null-terminate the string

    strcpy(*value, colon_pos + 1); // Copy the value string
}