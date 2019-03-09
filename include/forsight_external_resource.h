#ifndef FORSIGHT_EXTERNAL_RESOURCE_H
#define FORSIGHT_EXTERNAL_RESOURCE_H

#include "forsight_basint.h"

typedef enum _KeyVariableType
{
    KEYTYPE_NONE = 0,
    KEYTYPE_CHAR,
    KEYTYPE_INT16,
    KEYTYPE_INT32,
    KEYTYPE_INT64,
    KEYTYPE_DOUBLE,
    KEYTYPE_BYTEARRAY,
}KeyVariableType;

typedef struct key_variable_t {
    char             key_name[LAB_LEN];      // KeyName
    long             base_address;  // BaseAddress
    KeyVariableType  key_type;      // Type
	int              key_size;      // ByteSize
} key_variable ;



int import_external_resource(char *filename, 
				vector<key_variable>& vecKeyVariables);

#endif
