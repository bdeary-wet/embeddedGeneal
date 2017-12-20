#include "optable.h"
#include <stdlib.h>


void OPT_InitTable(opTable_t *to, 
                   opTableEntry_t const *table, 
                   size_t tableLen, 
                   opTblEntryProcessor_f processor)
{
   to->table = table;
   to->tableLen = tableLen;
   to->processor = processor;
}

static int cmpfunc(const void * a, const void * b) {
    opTblKey_t dif = *(opTblKey_t*)a - *(opTblKey_t*)b;
    return  dif? ((dif&OP_KEY_MSB)?-1:1):0;
}

opTableEntry_t const *OPT_FindEntry(opTable_t const *to, opTblKey_t key)
{
    return bsearch(&key, 
                   to->table, 
                   to->tableLen, 
                   sizeof(opTableEntry_t),
                   cmpfunc);
}

opTableEntry_t const *OPT_ProcessViaTable(opTable_t const *to, 
                                    opTblKey_t key, 
                                    uintptr_t context)
{
    opTableEntry_t const *entry = OPT_FindEntry(to, key);
    if(entry && to->processor)
    {
        to->processor(entry, context);
    }
    return entry;
}
