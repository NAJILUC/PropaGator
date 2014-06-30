#ifndef __c8_BoatLibraryTest_h__
#define __c8_BoatLibraryTest_h__

/* Include files */
#include "sfc_sf.h"
#include "sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc8_BoatLibraryTestInstanceStruct
#define typedef_SFc8_BoatLibraryTestInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  uint32_T chartNumber;
  uint32_T instanceNumber;
  int32_T c8_sfEvent;
  boolean_T c8_isStable;
  boolean_T c8_doneDoubleBufferReInit;
  uint8_T c8_is_active_c8_BoatLibraryTest;
} SFc8_BoatLibraryTestInstanceStruct;

#endif                                 /*typedef_SFc8_BoatLibraryTestInstanceStruct*/

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c8_BoatLibraryTest_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c8_BoatLibraryTest_get_check_sum(mxArray *plhs[]);
extern void c8_BoatLibraryTest_method_dispatcher(SimStruct *S, int_T method,
  void *data);

#endif
