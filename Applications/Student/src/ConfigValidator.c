#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "../include/ConfigValidator.h"

#define C1_ID "C1"
#define C2_ID "C2"
/*
* Array of config items to be copied into instance of config spec upon
*   initialization
* NOTE: C1 and C2 upper bounds are calculated off of MaxTime value. 
*       It is ASSUMED the maxtime parameter will be sent first. 
*       The max is preset to -1 so C1, C2 will be set to default until max
*       time is sent. After which, the method addCoefficientConfigs must 
*       be called.
*/
ConfigItem INIT_CONFIG_SPEC[NUM_CONFIG_ITEMS] = {
    {"ID", "N/A", STRING, {.StringRange = {{"Columbia", "Challenger", "Discovery", "Atlantis", "Endeavour"}, "Columbia"}} },
    {"Mass", "Tons", INTEGER, {.IntegerRange = {70, 90, 80}} },
    {"Cap", "N/A", INTEGER, {.IntegerRange = {1, 5, 3}} },
    {"MaxTime", "Sec", INTEGER, {.IntegerRange = {8, 10, 10}} },
    {"Fuel", "Gallons", DOUBLE, {.DoubleRange = {500, 1000, 500, 2}} },
    {"Vel", "MPH", INTEGER, {.IntegerRange = {10000, 20000, 20000}} },
    {"MPG", "MPG", INTEGER, {.IntegerRange = {5, 15, 5}} },
    {"FuelRate", "Gallons/Sec", DOUBLE, {.DoubleRange = {0.4, 2.8, -1.0, 1}} },
    {"C1", "N/A", INTEGER, {.IntegerRange = {1, -1, 1}} },  
    {"C2", "N/A", INTEGER, {.IntegerRange = {2, -1, 4}} }, 
    {"Arate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 1}} },
    {"Brate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 1}} },
    {"Erate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 1}} },
    {"Mrate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 1}} },
    {"Srate", "N/A", DOUBLE, {.DoubleRange = {0.5, 1.0, 0.9, 1}} }

};

/*
 *@brief Function iterates through ConfigSpec and updates the allowed max values of C1 and C2
 *@param Pointer to Config Spec
 *@param integer representing the provided maxTimeParam input to drone
**/
void addCoefficientConfigs(ConfigSpec *spec, int maxTime){
    // Find matching config spec
    ConfigItem* itemSpec;
    int i = 0;

    while (i < spec->num_items) {
        itemSpec = spec->configItems[i];
        if (itemSpec != NULL) {
            if(strcmp(itemSpec->param_id, C1_ID) == 0){
                spec->configItems[i]->valueRange.IntegerRange.max = floor(maxTime/2);
                // printf("Updated C1 MAX: %i\n", spec->configItems[i]->valueRange.IntegerRange.max);
            }
            else if(strcmp(itemSpec->param_id, C2_ID) == 0){
                spec->configItems[i]->valueRange.IntegerRange.max = floor(maxTime/2) - 1;
                // printf("Updated C2 MAX: %i\n", spec->configItems[i]->valueRange.IntegerRange.max);

            }
        }
        i++;
    }

}

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
            else if (range.IntegerRange.min <= int_value && int_value <= range.IntegerRange.max) {
                configValue->value.int_value = int_value;
            }
            else{
                configValue->value.int_value = range.IntegerRange.default_int;
            }

            sprintf(configValue->formated_value, "%i", configValue->value.int_value);
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
            else if (range.DoubleRange.min <= value_double && value_double <= range.DoubleRange.max) {
                configValue->value.double_value = value_double;
            }
            else{
                configValue->value.double_value = range.DoubleRange.default_double;
            }

            //store formated double according to config
            int precision = range.DoubleRange.precision;
            sprintf(configValue->formated_value, "%.*lf", precision, configValue->value.double_value);
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

            //create a copy of formatted value
            strcpy(configValue->formated_value, configValue->value.str_value);
            break;
        }
        default:
        {
            configValue->type = INVALID;
            break;
        }

    }

    //check if param_id was MaxTime
    if(strcmp(param_id, "MaxTime") == 0){
        addCoefficientConfigs(spec, configValue->value.int_value);
    }

    return configValue;

}
