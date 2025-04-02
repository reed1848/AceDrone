#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#include "../include/ConfigValidator.h"


/*
* Array of config items to be copied into instance of config spec upon
*   initialization
*/
ConfigItem INIT_CONFIG_SPEC[NUM_CONFIG_ITEMS] = {
    {"ID", "N/A", STRING, {.StringRange = {{"Columbia", "Challenger", "Discovery", "Atlantis", "Endeavour"}, "Columbia"}} },
    {"Mass", "Tons", INTEGER, {.IntegerRange = {70, 90, 80, 1}} },
    {"Cap", "N/A", INTEGER, {.IntegerRange = {1, 5, 3, 1}} },
    {"MaxTime", "Sec", INTEGER, {.IntegerRange = {8, 10, 10, 1}} },
    {"Fuel", "Gallons", DOUBLE, {.DoubleRange = {500, 1000, 500, 0.1}} },
    {"Vel", "MPH", INTEGER, {.IntegerRange = {10000, 20000, 20000, 1}} },
    {"MPG", "MPG", INTEGER, {.IntegerRange = {5, 15, 5, 1}} },
    {"FuelRate", "Gallons/Sec", DOUBLE, {.DoubleRange = {0.4, 2.8, -1.0, 0.1}} },
    {"C1", "N/A", INTEGER, {.IntegerRange = {1, -1, 1, 1}} },
    {"C2", "N/A", INTEGER, {.IntegerRange = {2, -1, 4, 1}} },
    {"Arate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 0.1}} },
    {"Brate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 0.1}} },
    {"Erate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 0.1}} },
    {"Mrate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 0.1}} },
    {"Srate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 0.1}} }

};


/*
 * Initializes and returns an instance of ConfigSpec which stores
 * validation info on possible input parameters.
*/
ConfigSpec * init_config_spec(){
    ConfigSpec *spec = (ConfigSpec*)malloc(sizeof(ConfigSpec));
    spec->num_items = NUM_CONFIG_ITEMS;

    for(int i=0; i < NUM_CONFIG_ITEMS; i++){
        spec->configItems[i] = &(INIT_CONFIG_SPEC[i]);
        //memcpy(spec->configItems[i], &INIT_CONFIG_SPEC[i], sizeof(INIT_CONFIG_SPEC[i]));
    }

    return spec;
}

/*
 * @brief given a ConfigSpec and param id and value, validates the parameter and
 *        value.  Returns default value if given value is out of valid range.
 * @param Pointer to initialized ConfigSpec
 * @param Pointer to param ID
 * @param opinter to param value
 * @return ConfigValue struct containing the type and a union of its value
 **/
ConfigValue* validate_config_message(ConfigSpec* spec, const char* param_id, const char* value) {
    ConfigValue* configValue = (ConfigValue*)malloc(sizeof(ConfigValue));

    // Find matching config spec
    ConfigItem* itemSpec;
    int i = 0;
    bool found = false;
    while (i < spec->num_items && !found) {
        itemSpec = spec->configItems[i];
        if (itemSpec != NULL && strcmp(itemSpec->param_id, param_id) == 0) {
            found = true;
            break;
        }
        i++;
    }

    // If no matching expected param not found, return with valuetype INVALID
    if (!found) {
        configValue->type = INVALID;
        return configValue;
    }

    //Validate value
    //TODO: make slightly more type safe?
    union ValueRange range = itemSpec->valueRange;
    switch (itemSpec->valueType) {
        char *endptr;
        case INTEGER:
        {
            //attempt to cast string to int
            configValue->type = INTEGER;
            int int_value = (int)strtol(value, &endptr, 10);

            //error checking

            if(*endptr != '\0'){
                printf("Invalid characters found string to int cast: %s\n", endptr);

            }
            //validity checking
            else if (range.IntegerRange.min < int_value && int_value < range.IntegerRange.max) {
                configValue->value.int_value = int_value;
            }
            else{
                configValue->value.int_value = range.IntegerRange.default_int;
            }
            break;
        }
        case DOUBLE:
        {
            //attempt to cast string to double
            configValue->type = DOUBLE;
            double value_double = strtod(value, &endptr);

            //error checking
            if(*endptr != '\0'){
                configValue->value.double_value = range.DoubleRange.default_double;

            }
            //check for conversion errors
            if (endptr == value || *endptr != '\0') {
                configValue->value.double_value = range.DoubleRange.default_double;
            }
            else if (range.IntegerRange.min < value_double && value_double < range.IntegerRange.max) {
                configValue->value.double_value = value_double;
            }
            else{
                configValue->value.double_value = range.DoubleRange.default_double;
            }
            break;
        }
        case STRING:
        {
            //Copy string value
            configValue->type = STRING;
            configValue->value.str_value = (char *)malloc(MAX_VALUE_LEN * sizeof(char));

            //search to see if value is valid based on configSpec
            int num_options = sizeof(range.StringRange.values) / sizeof(range.StringRange.values[0]);
            bool found = false;
            for(int i=0; i < num_options; i++){
                char *temp = range.StringRange.values[i];
                //int matches = strcmp(value, temp);
                if(strcmp(value, temp) == 0){
                    strcpy(configValue->value.str_value, value);
                    found = true;
                }
            }

            //if not found, set to default
            if(!found){
                strcpy(configValue->value.str_value, range.StringRange.default_str);
            }
            break;
        }
        default:
        {
            configValue->type = INVALID;
            break;
        }
    }

    return configValue;

}




// //Test code
// int main(){
//     ConfigSpec *configSpec = init_config_spec();
//     char *test_lines[] = {"Start:Config", "End:Config", "ID:Bad", "ID:Discovery", "Mass:60", "Mass:80", "Mass:", "Arate:0.3", "Arate: 0.0", "Arate:Bad", "Arate:1.0"};
    
//     for(int i = 0; i < 11; i++){
//         char temp[20], **line;
//         strcpy(temp, test_lines[i]);
//         line = (char**)malloc(sizeof(char**));
//         *line = temp;

//         char *param_id = (char*)malloc(sizeof(char*));
//         char *value = (char*)malloc(sizeof(char*));
//         parse_config_message(*line, &param_id, &value);
//         // param_id = parse_till_separator(line, ':');
//         // value = parse_till_separator(line, '\n');
        
//         ConfigValue *item = (ConfigValue*)malloc(sizeof(ConfigValue));
//         item = validate_config_message(configSpec, param_id, value);
//         switch(item->type){
//             case STRING:
//                 printf("Parsed Parameter %s as String: %s\n", param_id, item->str_value);
//                 break;
//             case INTEGER:
//                 printf("Parsed Parameter %s as String: %i\n", param_id, item->int_value);
//                 break;
//             case DOUBLE:
//                 printf("Parsed Parameter %s as String: %lf\n", param_id, item->double_value);
//                 break;
//             default:
//                 printf("Parameter %s not parsed correctly\n", param_id);
//                 break;
//         }
//     }
// }