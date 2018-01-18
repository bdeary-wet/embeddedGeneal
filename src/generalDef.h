#ifndef _GENERALDEF_H_
#define _GENERALDEF_H_


#ifdef TEST
#define START_PROTECTED {
#define END_PROTECTED }

#else
#define START_PROTECTED \
{ uint32_t protectedVar321 = __get_PRIMASK();\
  __set_PRIMASK(1);
  
#define END_PROTECTED \
__set_PRIMASK(protectedVar321); }

#endif

// array dimension
#define DIM(arr) (sizeof(arr)/sizeof(arr[0]))
    
// saturated unsigned inc and dec loose version, 
#define USAT_INC(ui) do{ui = (ui)+1 ? (ui)+1:(ui);}while(0)
#define USAT_DEC(ui) do{ui = (ui) ? (ui)-1:(ui);}while(0)




#endif


