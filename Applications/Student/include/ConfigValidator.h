
#define NUM_CONFIG_ITEMS 15
#define MAX_PARAM_ID_LEN 20
#define MAX_VALUE_LEN 20
#define NUM_STRING_VALUES 5 //todo: less messy?

enum ValueType {
    INTEGER,
    DOUBLE,
    STRING,
    INVALID
};

typedef struct {
    int num_items;
    ConfigItem* configItems[NUM_CONFIG_ITEMS];
} ConfigSpec;

typedef struct {
    char param_id[MAX_PARAM_ID_LEN];
    char* unit;
    enum ValueType valueType;
    union ValueRange valueRange;
} ConfigItem;

union ValueRange
{
    struct {
        int min;
        int max;
        int default;
        int increment;
    } IntegerRange;

    struct {
        double min;
        double max;
        double default;
        double increment;
    } DoubleRange;

    struct {
        char values[NUM_STRING_VALUES][MAX_VALUE_LEN];
    } StringRange;
};

typedef struct {
    enum ValueType type;
    union Value {
        int int_value;
        char* str_value;
        double double_value;
    };
} ConfigValue;

void init_config_spec(ConfigSpec* spec);
ConfigValue* validate_config_message(ConfigSpec* spec, char* config_message);
void parse_config_message(char* message, char** param_id, char** value);