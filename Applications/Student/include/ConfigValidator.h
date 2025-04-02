#ifndef _CONFIGVALIDATOR_H
#define _CONFIGVALIDATOR_H
#ifdef __cplusplus
extern "C" {
#endif

#define NUM_CONFIG_ITEMS 15
#define MAX_PARAM_ID_LEN 20
#define MAX_VALUE_LEN 20
#define NUM_STRING_VALUES 5 //todo: less messy?


// MyEnum.h
#ifndef VALUETYPE_ENUM
#define VALUETYPE_ENUM
enum ValueType {
    INTEGER,
    DOUBLE,
    STRING,
    INVALID
};
#endif
 

union ValueRange
{
    struct {
        int min;
        int max;
        int default_int;
        int increment;
    } IntegerRange;

    struct {
        double min;
        double max;
        double default_double;
        double increment;
    } DoubleRange;

    struct {
        char values[NUM_STRING_VALUES][MAX_VALUE_LEN];
        char *default_str;
    } StringRange;
};

typedef struct {
    char param_id[MAX_PARAM_ID_LEN];
    char* unit;
    enum ValueType valueType;
    union ValueRange valueRange;
} ConfigItem;

typedef struct {
    int num_items;
    ConfigItem* configItems[NUM_CONFIG_ITEMS];
} ConfigSpec;

typedef struct {
    enum ValueType type;
    union {
        int int_value;
        char* str_value;
        double double_value;
    } value;
} ConfigValue;

ConfigSpec * init_config_spec();
ConfigValue* validate_config_message(ConfigSpec* spec, char* param_id, char* value);
void parse_config_message(char* message, char** param_id, char** value);
char * parse_till_separator(char** message, char separator);
#ifdef __cplusplus
}
#endif
#endif