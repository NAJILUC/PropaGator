/* Include files */

#include "blascompat32.h"
#include "boat_sfun.h"
#include "c6_boat.h"
#define CHARTINSTANCE_CHARTNUMBER      (chartInstance->chartNumber)
#define CHARTINSTANCE_INSTANCENUMBER   (chartInstance->instanceNumber)
#include "boat_sfun_debug_macros.h"

/* Type Definitions */

/* Named Constants */
#define CALL_EVENT                     (-1)

/* Variable Declarations */

/* Variable Definitions */
static const char * c6_debug_family_names[5] = { "nargin", "nargout", "C", "u",
  "y" };

/* Function Declarations */
static void initialize_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void initialize_params_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void enable_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void disable_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void c6_update_debugger_state_c6_boat(SFc6_boatInstanceStruct
  *chartInstance);
static const mxArray *get_sim_state_c6_boat(SFc6_boatInstanceStruct
  *chartInstance);
static void set_sim_state_c6_boat(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_st);
static void finalize_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void sf_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void c6_chartstep_c6_boat(SFc6_boatInstanceStruct *chartInstance);
static void initSimStructsc6_boat(SFc6_boatInstanceStruct *chartInstance);
static void init_script_number_translation(uint32_T c6_machineNumber, uint32_T
  c6_chartNumber);
static const mxArray *c6_sf_marshallOut(void *chartInstanceVoid, void *c6_inData);
static void c6_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_y, const char_T *c6_identifier, real_T c6_b_y[3]);
static void c6_b_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId, real_T c6_y[3]);
static void c6_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData);
static const mxArray *c6_b_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData);
static const mxArray *c6_c_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData);
static const mxArray *c6_d_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData);
static real_T c6_c_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId);
static void c6_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData);
static void c6_eml_scalar_eg(SFc6_boatInstanceStruct *chartInstance);
static const mxArray *c6_e_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData);
static int32_T c6_d_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId);
static void c6_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData);
static uint8_T c6_e_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_b_is_active_c6_boat, const char_T *c6_identifier);
static uint8_T c6_f_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId);
static void init_dsm_address_info(SFc6_boatInstanceStruct *chartInstance);

/* Function Definitions */
static void initialize_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
  chartInstance->c6_sfEvent = CALL_EVENT;
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  chartInstance->c6_is_active_c6_boat = 0U;
}

static void initialize_params_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
}

static void enable_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static void disable_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
}

static void c6_update_debugger_state_c6_boat(SFc6_boatInstanceStruct
  *chartInstance)
{
}

static const mxArray *get_sim_state_c6_boat(SFc6_boatInstanceStruct
  *chartInstance)
{
  const mxArray *c6_st;
  const mxArray *c6_y = NULL;
  int32_T c6_i0;
  real_T c6_u[3];
  const mxArray *c6_b_y = NULL;
  uint8_T c6_hoistedGlobal;
  uint8_T c6_b_u;
  const mxArray *c6_c_y = NULL;
  real_T (*c6_d_y)[3];
  c6_d_y = (real_T (*)[3])ssGetOutputPortSignal(chartInstance->S, 1);
  c6_st = NULL;
  c6_st = NULL;
  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_createcellarray(2), FALSE);
  for (c6_i0 = 0; c6_i0 < 3; c6_i0++) {
    c6_u[c6_i0] = (*c6_d_y)[c6_i0];
  }

  c6_b_y = NULL;
  sf_mex_assign(&c6_b_y, sf_mex_create("y", c6_u, 0, 0U, 1U, 0U, 1, 3), FALSE);
  sf_mex_setcell(c6_y, 0, c6_b_y);
  c6_hoistedGlobal = chartInstance->c6_is_active_c6_boat;
  c6_b_u = c6_hoistedGlobal;
  c6_c_y = NULL;
  sf_mex_assign(&c6_c_y, sf_mex_create("y", &c6_b_u, 3, 0U, 0U, 0U, 0), FALSE);
  sf_mex_setcell(c6_y, 1, c6_c_y);
  sf_mex_assign(&c6_st, c6_y, FALSE);
  return c6_st;
}

static void set_sim_state_c6_boat(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_st)
{
  const mxArray *c6_u;
  real_T c6_dv0[3];
  int32_T c6_i1;
  real_T (*c6_y)[3];
  c6_y = (real_T (*)[3])ssGetOutputPortSignal(chartInstance->S, 1);
  chartInstance->c6_doneDoubleBufferReInit = TRUE;
  c6_u = sf_mex_dup(c6_st);
  c6_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(c6_u, 0)), "y",
                      c6_dv0);
  for (c6_i1 = 0; c6_i1 < 3; c6_i1++) {
    (*c6_y)[c6_i1] = c6_dv0[c6_i1];
  }

  chartInstance->c6_is_active_c6_boat = c6_e_emlrt_marshallIn(chartInstance,
    sf_mex_dup(sf_mex_getcell(c6_u, 1)), "is_active_c6_boat");
  sf_mex_destroy(&c6_u);
  c6_update_debugger_state_c6_boat(chartInstance);
  sf_mex_destroy(&c6_st);
}

static void finalize_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
}

static void sf_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
  int32_T c6_i2;
  int32_T c6_i3;
  int32_T c6_i4;
  real_T (*c6_y)[3];
  real_T (*c6_u)[6];
  real_T (*c6_C)[18];
  c6_y = (real_T (*)[3])ssGetOutputPortSignal(chartInstance->S, 1);
  c6_u = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 1);
  c6_C = (real_T (*)[18])ssGetInputPortSignal(chartInstance->S, 0);
  _sfTime_ = (real_T)ssGetT(chartInstance->S);
  _SFD_CC_CALL(CHART_ENTER_SFUNCTION_TAG, 1U, chartInstance->c6_sfEvent);
  for (c6_i2 = 0; c6_i2 < 18; c6_i2++) {
    _SFD_DATA_RANGE_CHECK((*c6_C)[c6_i2], 0U);
  }

  for (c6_i3 = 0; c6_i3 < 6; c6_i3++) {
    _SFD_DATA_RANGE_CHECK((*c6_u)[c6_i3], 1U);
  }

  for (c6_i4 = 0; c6_i4 < 3; c6_i4++) {
    _SFD_DATA_RANGE_CHECK((*c6_y)[c6_i4], 2U);
  }

  chartInstance->c6_sfEvent = CALL_EVENT;
  c6_chartstep_c6_boat(chartInstance);
  sf_debug_check_for_state_inconsistency(_boatMachineNumber_,
    chartInstance->chartNumber, chartInstance->instanceNumber);
}

static void c6_chartstep_c6_boat(SFc6_boatInstanceStruct *chartInstance)
{
  int32_T c6_i5;
  real_T c6_C[18];
  int32_T c6_i6;
  real_T c6_u[6];
  uint32_T c6_debug_family_var_map[5];
  real_T c6_nargin = 2.0;
  real_T c6_nargout = 1.0;
  real_T c6_y[3];
  int32_T c6_i7;
  real_T c6_a[18];
  int32_T c6_i8;
  real_T c6_b[6];
  int32_T c6_i9;
  int32_T c6_i10;
  int32_T c6_i11;
  real_T c6_b_C[3];
  int32_T c6_i12;
  int32_T c6_i13;
  int32_T c6_i14;
  int32_T c6_i15;
  int32_T c6_i16;
  int32_T c6_i17;
  int32_T c6_i18;
  real_T (*c6_b_y)[3];
  real_T (*c6_b_u)[6];
  real_T (*c6_c_C)[18];
  c6_b_y = (real_T (*)[3])ssGetOutputPortSignal(chartInstance->S, 1);
  c6_b_u = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 1);
  c6_c_C = (real_T (*)[18])ssGetInputPortSignal(chartInstance->S, 0);
  _SFD_CC_CALL(CHART_ENTER_DURING_FUNCTION_TAG, 1U, chartInstance->c6_sfEvent);
  for (c6_i5 = 0; c6_i5 < 18; c6_i5++) {
    c6_C[c6_i5] = (*c6_c_C)[c6_i5];
  }

  for (c6_i6 = 0; c6_i6 < 6; c6_i6++) {
    c6_u[c6_i6] = (*c6_b_u)[c6_i6];
  }

  sf_debug_symbol_scope_push_eml(0U, 5U, 5U, c6_debug_family_names,
    c6_debug_family_var_map);
  sf_debug_symbol_scope_add_eml_importable(&c6_nargin, 0U, c6_d_sf_marshallOut,
    c6_b_sf_marshallIn);
  sf_debug_symbol_scope_add_eml_importable(&c6_nargout, 1U, c6_d_sf_marshallOut,
    c6_b_sf_marshallIn);
  sf_debug_symbol_scope_add_eml(c6_C, 2U, c6_c_sf_marshallOut);
  sf_debug_symbol_scope_add_eml(c6_u, 3U, c6_b_sf_marshallOut);
  sf_debug_symbol_scope_add_eml_importable(c6_y, 4U, c6_sf_marshallOut,
    c6_sf_marshallIn);
  CV_EML_FCN(0, 0);
  _SFD_EML_CALL(0U, chartInstance->c6_sfEvent, 4);
  for (c6_i7 = 0; c6_i7 < 18; c6_i7++) {
    c6_a[c6_i7] = c6_C[c6_i7];
  }

  for (c6_i8 = 0; c6_i8 < 6; c6_i8++) {
    c6_b[c6_i8] = c6_u[c6_i8];
  }

  c6_eml_scalar_eg(chartInstance);
  c6_eml_scalar_eg(chartInstance);
  for (c6_i9 = 0; c6_i9 < 3; c6_i9++) {
    c6_y[c6_i9] = 0.0;
  }

  for (c6_i10 = 0; c6_i10 < 3; c6_i10++) {
    c6_y[c6_i10] = 0.0;
  }

  for (c6_i11 = 0; c6_i11 < 3; c6_i11++) {
    c6_b_C[c6_i11] = c6_y[c6_i11];
  }

  for (c6_i12 = 0; c6_i12 < 3; c6_i12++) {
    c6_y[c6_i12] = c6_b_C[c6_i12];
  }

  for (c6_i13 = 0; c6_i13 < 3; c6_i13++) {
    c6_b_C[c6_i13] = c6_y[c6_i13];
  }

  for (c6_i14 = 0; c6_i14 < 3; c6_i14++) {
    c6_y[c6_i14] = c6_b_C[c6_i14];
  }

  for (c6_i15 = 0; c6_i15 < 3; c6_i15++) {
    c6_y[c6_i15] = 0.0;
    c6_i16 = 0;
    for (c6_i17 = 0; c6_i17 < 6; c6_i17++) {
      c6_y[c6_i15] += c6_a[c6_i16 + c6_i15] * c6_b[c6_i17];
      c6_i16 += 3;
    }
  }

  _SFD_EML_CALL(0U, chartInstance->c6_sfEvent, -4);
  sf_debug_symbol_scope_pop();
  for (c6_i18 = 0; c6_i18 < 3; c6_i18++) {
    (*c6_b_y)[c6_i18] = c6_y[c6_i18];
  }

  _SFD_CC_CALL(EXIT_OUT_OF_FUNCTION_TAG, 1U, chartInstance->c6_sfEvent);
}

static void initSimStructsc6_boat(SFc6_boatInstanceStruct *chartInstance)
{
}

static void init_script_number_translation(uint32_T c6_machineNumber, uint32_T
  c6_chartNumber)
{
}

static const mxArray *c6_sf_marshallOut(void *chartInstanceVoid, void *c6_inData)
{
  const mxArray *c6_mxArrayOutData = NULL;
  int32_T c6_i19;
  real_T c6_b_inData[3];
  int32_T c6_i20;
  real_T c6_u[3];
  const mxArray *c6_y = NULL;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_mxArrayOutData = NULL;
  for (c6_i19 = 0; c6_i19 < 3; c6_i19++) {
    c6_b_inData[c6_i19] = (*(real_T (*)[3])c6_inData)[c6_i19];
  }

  for (c6_i20 = 0; c6_i20 < 3; c6_i20++) {
    c6_u[c6_i20] = c6_b_inData[c6_i20];
  }

  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_create("y", c6_u, 0, 0U, 1U, 0U, 1, 3), FALSE);
  sf_mex_assign(&c6_mxArrayOutData, c6_y, FALSE);
  return c6_mxArrayOutData;
}

static void c6_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_y, const char_T *c6_identifier, real_T c6_b_y[3])
{
  emlrtMsgIdentifier c6_thisId;
  c6_thisId.fIdentifier = c6_identifier;
  c6_thisId.fParent = NULL;
  c6_b_emlrt_marshallIn(chartInstance, sf_mex_dup(c6_y), &c6_thisId, c6_b_y);
  sf_mex_destroy(&c6_y);
}

static void c6_b_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance, const
  mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId, real_T c6_y[3])
{
  real_T c6_dv1[3];
  int32_T c6_i21;
  sf_mex_import(c6_parentId, sf_mex_dup(c6_u), c6_dv1, 1, 0, 0U, 1, 0U, 1, 3);
  for (c6_i21 = 0; c6_i21 < 3; c6_i21++) {
    c6_y[c6_i21] = c6_dv1[c6_i21];
  }

  sf_mex_destroy(&c6_u);
}

static void c6_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData)
{
  const mxArray *c6_y;
  const char_T *c6_identifier;
  emlrtMsgIdentifier c6_thisId;
  real_T c6_b_y[3];
  int32_T c6_i22;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_y = sf_mex_dup(c6_mxArrayInData);
  c6_identifier = c6_varName;
  c6_thisId.fIdentifier = c6_identifier;
  c6_thisId.fParent = NULL;
  c6_b_emlrt_marshallIn(chartInstance, sf_mex_dup(c6_y), &c6_thisId, c6_b_y);
  sf_mex_destroy(&c6_y);
  for (c6_i22 = 0; c6_i22 < 3; c6_i22++) {
    (*(real_T (*)[3])c6_outData)[c6_i22] = c6_b_y[c6_i22];
  }

  sf_mex_destroy(&c6_mxArrayInData);
}

static const mxArray *c6_b_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData)
{
  const mxArray *c6_mxArrayOutData = NULL;
  int32_T c6_i23;
  real_T c6_b_inData[6];
  int32_T c6_i24;
  real_T c6_u[6];
  const mxArray *c6_y = NULL;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_mxArrayOutData = NULL;
  for (c6_i23 = 0; c6_i23 < 6; c6_i23++) {
    c6_b_inData[c6_i23] = (*(real_T (*)[6])c6_inData)[c6_i23];
  }

  for (c6_i24 = 0; c6_i24 < 6; c6_i24++) {
    c6_u[c6_i24] = c6_b_inData[c6_i24];
  }

  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_create("y", c6_u, 0, 0U, 1U, 0U, 1, 6), FALSE);
  sf_mex_assign(&c6_mxArrayOutData, c6_y, FALSE);
  return c6_mxArrayOutData;
}

static const mxArray *c6_c_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData)
{
  const mxArray *c6_mxArrayOutData = NULL;
  int32_T c6_i25;
  int32_T c6_i26;
  int32_T c6_i27;
  real_T c6_b_inData[18];
  int32_T c6_i28;
  int32_T c6_i29;
  int32_T c6_i30;
  real_T c6_u[18];
  const mxArray *c6_y = NULL;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_mxArrayOutData = NULL;
  c6_i25 = 0;
  for (c6_i26 = 0; c6_i26 < 6; c6_i26++) {
    for (c6_i27 = 0; c6_i27 < 3; c6_i27++) {
      c6_b_inData[c6_i27 + c6_i25] = (*(real_T (*)[18])c6_inData)[c6_i27 +
        c6_i25];
    }

    c6_i25 += 3;
  }

  c6_i28 = 0;
  for (c6_i29 = 0; c6_i29 < 6; c6_i29++) {
    for (c6_i30 = 0; c6_i30 < 3; c6_i30++) {
      c6_u[c6_i30 + c6_i28] = c6_b_inData[c6_i30 + c6_i28];
    }

    c6_i28 += 3;
  }

  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_create("y", c6_u, 0, 0U, 1U, 0U, 2, 3, 6), FALSE);
  sf_mex_assign(&c6_mxArrayOutData, c6_y, FALSE);
  return c6_mxArrayOutData;
}

static const mxArray *c6_d_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData)
{
  const mxArray *c6_mxArrayOutData = NULL;
  real_T c6_u;
  const mxArray *c6_y = NULL;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_mxArrayOutData = NULL;
  c6_u = *(real_T *)c6_inData;
  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_create("y", &c6_u, 0, 0U, 0U, 0U, 0), FALSE);
  sf_mex_assign(&c6_mxArrayOutData, c6_y, FALSE);
  return c6_mxArrayOutData;
}

static real_T c6_c_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId)
{
  real_T c6_y;
  real_T c6_d0;
  sf_mex_import(c6_parentId, sf_mex_dup(c6_u), &c6_d0, 1, 0, 0U, 0, 0U, 0);
  c6_y = c6_d0;
  sf_mex_destroy(&c6_u);
  return c6_y;
}

static void c6_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData)
{
  const mxArray *c6_nargout;
  const char_T *c6_identifier;
  emlrtMsgIdentifier c6_thisId;
  real_T c6_y;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_nargout = sf_mex_dup(c6_mxArrayInData);
  c6_identifier = c6_varName;
  c6_thisId.fIdentifier = c6_identifier;
  c6_thisId.fParent = NULL;
  c6_y = c6_c_emlrt_marshallIn(chartInstance, sf_mex_dup(c6_nargout), &c6_thisId);
  sf_mex_destroy(&c6_nargout);
  *(real_T *)c6_outData = c6_y;
  sf_mex_destroy(&c6_mxArrayInData);
}

const mxArray *sf_c6_boat_get_eml_resolved_functions_info(void)
{
  const mxArray *c6_nameCaptureInfo;
  c6_ResolvedFunctionInfo c6_info[8];
  c6_ResolvedFunctionInfo (*c6_b_info)[8];
  const mxArray *c6_m0 = NULL;
  int32_T c6_i31;
  c6_ResolvedFunctionInfo *c6_r0;
  c6_nameCaptureInfo = NULL;
  c6_nameCaptureInfo = NULL;
  c6_b_info = (c6_ResolvedFunctionInfo (*)[8])c6_info;
  (*c6_b_info)[0].context = "";
  (*c6_b_info)[0].name = "mtimes";
  (*c6_b_info)[0].dominantType = "double";
  (*c6_b_info)[0].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/ops/mtimes.m";
  (*c6_b_info)[0].fileTimeLo = 1289541292U;
  (*c6_b_info)[0].fileTimeHi = 0U;
  (*c6_b_info)[0].mFileTimeLo = 0U;
  (*c6_b_info)[0].mFileTimeHi = 0U;
  (*c6_b_info)[1].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/ops/mtimes.m";
  (*c6_b_info)[1].name = "eml_index_class";
  (*c6_b_info)[1].dominantType = "";
  (*c6_b_info)[1].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  (*c6_b_info)[1].fileTimeLo = 1286840378U;
  (*c6_b_info)[1].fileTimeHi = 0U;
  (*c6_b_info)[1].mFileTimeLo = 0U;
  (*c6_b_info)[1].mFileTimeHi = 0U;
  (*c6_b_info)[2].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/ops/mtimes.m";
  (*c6_b_info)[2].name = "eml_scalar_eg";
  (*c6_b_info)[2].dominantType = "double";
  (*c6_b_info)[2].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/eml_scalar_eg.m";
  (*c6_b_info)[2].fileTimeLo = 1286840396U;
  (*c6_b_info)[2].fileTimeHi = 0U;
  (*c6_b_info)[2].mFileTimeLo = 0U;
  (*c6_b_info)[2].mFileTimeHi = 0U;
  (*c6_b_info)[3].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/ops/mtimes.m";
  (*c6_b_info)[3].name = "eml_xgemm";
  (*c6_b_info)[3].dominantType = "int32";
  (*c6_b_info)[3].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/eml_xgemm.m";
  (*c6_b_info)[3].fileTimeLo = 1299098372U;
  (*c6_b_info)[3].fileTimeHi = 0U;
  (*c6_b_info)[3].mFileTimeLo = 0U;
  (*c6_b_info)[3].mFileTimeHi = 0U;
  (*c6_b_info)[4].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/eml_xgemm.m";
  (*c6_b_info)[4].name = "eml_blas_inline";
  (*c6_b_info)[4].dominantType = "";
  (*c6_b_info)[4].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/eml_blas_inline.m";
  (*c6_b_info)[4].fileTimeLo = 1299098368U;
  (*c6_b_info)[4].fileTimeHi = 0U;
  (*c6_b_info)[4].mFileTimeLo = 0U;
  (*c6_b_info)[4].mFileTimeHi = 0U;
  (*c6_b_info)[5].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/external/eml_blas_xgemm.m!below_threshold";
  (*c6_b_info)[5].name = "mtimes";
  (*c6_b_info)[5].dominantType = "double";
  (*c6_b_info)[5].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/ops/mtimes.m";
  (*c6_b_info)[5].fileTimeLo = 1289541292U;
  (*c6_b_info)[5].fileTimeHi = 0U;
  (*c6_b_info)[5].mFileTimeLo = 0U;
  (*c6_b_info)[5].mFileTimeHi = 0U;
  (*c6_b_info)[6].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/external/eml_blas_xgemm.m";
  (*c6_b_info)[6].name = "eml_scalar_eg";
  (*c6_b_info)[6].dominantType = "double";
  (*c6_b_info)[6].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/eml_scalar_eg.m";
  (*c6_b_info)[6].fileTimeLo = 1286840396U;
  (*c6_b_info)[6].fileTimeHi = 0U;
  (*c6_b_info)[6].mFileTimeLo = 0U;
  (*c6_b_info)[6].mFileTimeHi = 0U;
  (*c6_b_info)[7].context =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/external/eml_blas_xgemm.m";
  (*c6_b_info)[7].name = "eml_refblas_xgemm";
  (*c6_b_info)[7].dominantType = "int32";
  (*c6_b_info)[7].resolved =
    "[ILXE]C:/Program Files/MATLAB/R2012a/toolbox/eml/lib/matlab/eml/blas/refblas/eml_refblas_xgemm.m";
  (*c6_b_info)[7].fileTimeLo = 1299098374U;
  (*c6_b_info)[7].fileTimeHi = 0U;
  (*c6_b_info)[7].mFileTimeLo = 0U;
  (*c6_b_info)[7].mFileTimeHi = 0U;
  sf_mex_assign(&c6_m0, sf_mex_createstruct("nameCaptureInfo", 1, 8), FALSE);
  for (c6_i31 = 0; c6_i31 < 8; c6_i31++) {
    c6_r0 = &c6_info[c6_i31];
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", c6_r0->context, 15,
      0U, 0U, 0U, 2, 1, strlen(c6_r0->context)), "context", "nameCaptureInfo",
                    c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", c6_r0->name, 15, 0U,
      0U, 0U, 2, 1, strlen(c6_r0->name)), "name", "nameCaptureInfo", c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", c6_r0->dominantType,
      15, 0U, 0U, 0U, 2, 1, strlen(c6_r0->dominantType)), "dominantType",
                    "nameCaptureInfo", c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", c6_r0->resolved, 15,
      0U, 0U, 0U, 2, 1, strlen(c6_r0->resolved)), "resolved", "nameCaptureInfo",
                    c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", &c6_r0->fileTimeLo,
      7, 0U, 0U, 0U, 0), "fileTimeLo", "nameCaptureInfo", c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", &c6_r0->fileTimeHi,
      7, 0U, 0U, 0U, 0), "fileTimeHi", "nameCaptureInfo", c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", &c6_r0->mFileTimeLo,
      7, 0U, 0U, 0U, 0), "mFileTimeLo", "nameCaptureInfo", c6_i31);
    sf_mex_addfield(c6_m0, sf_mex_create("nameCaptureInfo", &c6_r0->mFileTimeHi,
      7, 0U, 0U, 0U, 0), "mFileTimeHi", "nameCaptureInfo", c6_i31);
  }

  sf_mex_assign(&c6_nameCaptureInfo, c6_m0, FALSE);
  sf_mex_emlrtNameCapturePostProcessR2012a(&c6_nameCaptureInfo);
  return c6_nameCaptureInfo;
}

static void c6_eml_scalar_eg(SFc6_boatInstanceStruct *chartInstance)
{
}

static const mxArray *c6_e_sf_marshallOut(void *chartInstanceVoid, void
  *c6_inData)
{
  const mxArray *c6_mxArrayOutData = NULL;
  int32_T c6_u;
  const mxArray *c6_y = NULL;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_mxArrayOutData = NULL;
  c6_u = *(int32_T *)c6_inData;
  c6_y = NULL;
  sf_mex_assign(&c6_y, sf_mex_create("y", &c6_u, 6, 0U, 0U, 0U, 0), FALSE);
  sf_mex_assign(&c6_mxArrayOutData, c6_y, FALSE);
  return c6_mxArrayOutData;
}

static int32_T c6_d_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId)
{
  int32_T c6_y;
  int32_T c6_i32;
  sf_mex_import(c6_parentId, sf_mex_dup(c6_u), &c6_i32, 1, 6, 0U, 0, 0U, 0);
  c6_y = c6_i32;
  sf_mex_destroy(&c6_u);
  return c6_y;
}

static void c6_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c6_mxArrayInData, const char_T *c6_varName, void *c6_outData)
{
  const mxArray *c6_b_sfEvent;
  const char_T *c6_identifier;
  emlrtMsgIdentifier c6_thisId;
  int32_T c6_y;
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)chartInstanceVoid;
  c6_b_sfEvent = sf_mex_dup(c6_mxArrayInData);
  c6_identifier = c6_varName;
  c6_thisId.fIdentifier = c6_identifier;
  c6_thisId.fParent = NULL;
  c6_y = c6_d_emlrt_marshallIn(chartInstance, sf_mex_dup(c6_b_sfEvent),
    &c6_thisId);
  sf_mex_destroy(&c6_b_sfEvent);
  *(int32_T *)c6_outData = c6_y;
  sf_mex_destroy(&c6_mxArrayInData);
}

static uint8_T c6_e_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_b_is_active_c6_boat, const char_T *c6_identifier)
{
  uint8_T c6_y;
  emlrtMsgIdentifier c6_thisId;
  c6_thisId.fIdentifier = c6_identifier;
  c6_thisId.fParent = NULL;
  c6_y = c6_f_emlrt_marshallIn(chartInstance, sf_mex_dup(c6_b_is_active_c6_boat),
    &c6_thisId);
  sf_mex_destroy(&c6_b_is_active_c6_boat);
  return c6_y;
}

static uint8_T c6_f_emlrt_marshallIn(SFc6_boatInstanceStruct *chartInstance,
  const mxArray *c6_u, const emlrtMsgIdentifier *c6_parentId)
{
  uint8_T c6_y;
  uint8_T c6_u0;
  sf_mex_import(c6_parentId, sf_mex_dup(c6_u), &c6_u0, 1, 3, 0U, 0, 0U, 0);
  c6_y = c6_u0;
  sf_mex_destroy(&c6_u);
  return c6_y;
}

static void init_dsm_address_info(SFc6_boatInstanceStruct *chartInstance)
{
}

/* SFunction Glue Code */
void sf_c6_boat_get_check_sum(mxArray *plhs[])
{
  ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(685222521U);
  ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(3290945335U);
  ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(3776767575U);
  ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(1003272856U);
}

mxArray *sf_c6_boat_get_autoinheritance_info(void)
{
  const char *autoinheritanceFields[] = { "checksum", "inputs", "parameters",
    "outputs", "locals" };

  mxArray *mxAutoinheritanceInfo = mxCreateStructMatrix(1,1,5,
    autoinheritanceFields);

  {
    mxArray *mxChecksum = mxCreateString("Bc1WR2cZJ4CmZscFJDVKLD");
    mxSetField(mxAutoinheritanceInfo,0,"checksum",mxChecksum);
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,2,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(3);
      pr[1] = (double)(6);
      mxSetField(mxData,0,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt" };

      mxArray *mxType = mxCreateStructMatrix(1,1,2,typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxData,0,"type",mxType);
    }

    mxSetField(mxData,0,"complexity",mxCreateDoubleScalar(0));

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(6);
      pr[1] = (double)(1);
      mxSetField(mxData,1,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt" };

      mxArray *mxType = mxCreateStructMatrix(1,1,2,typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxData,1,"type",mxType);
    }

    mxSetField(mxData,1,"complexity",mxCreateDoubleScalar(0));
    mxSetField(mxAutoinheritanceInfo,0,"inputs",mxData);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"parameters",mxCreateDoubleMatrix(0,0,
                mxREAL));
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,1,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,2,mxREAL);
      double *pr = mxGetPr(mxSize);
      pr[0] = (double)(3);
      pr[1] = (double)(1);
      mxSetField(mxData,0,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt" };

      mxArray *mxType = mxCreateStructMatrix(1,1,2,typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxData,0,"type",mxType);
    }

    mxSetField(mxData,0,"complexity",mxCreateDoubleScalar(0));
    mxSetField(mxAutoinheritanceInfo,0,"outputs",mxData);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"locals",mxCreateDoubleMatrix(0,0,mxREAL));
  }

  return(mxAutoinheritanceInfo);
}

static const mxArray *sf_get_sim_state_info_c6_boat(void)
{
  const char *infoFields[] = { "chartChecksum", "varInfo" };

  mxArray *mxInfo = mxCreateStructMatrix(1, 1, 2, infoFields);
  const char *infoEncStr[] = {
    "100 S1x2'type','srcId','name','auxInfo'{{M[1],M[5],T\"y\",},{M[8],M[0],T\"is_active_c6_boat\",}}"
  };

  mxArray *mxVarInfo = sf_mex_decode_encoded_mx_struct_array(infoEncStr, 2, 10);
  mxArray *mxChecksum = mxCreateDoubleMatrix(1, 4, mxREAL);
  sf_c6_boat_get_check_sum(&mxChecksum);
  mxSetField(mxInfo, 0, infoFields[0], mxChecksum);
  mxSetField(mxInfo, 0, infoFields[1], mxVarInfo);
  return mxInfo;
}

static void chart_debug_initialization(SimStruct *S, unsigned int
  fullDebuggerInitialization)
{
  if (!sim_mode_is_rtw_gen(S)) {
    SFc6_boatInstanceStruct *chartInstance;
    chartInstance = (SFc6_boatInstanceStruct *) ((ChartInfoStruct *)
      (ssGetUserData(S)))->chartInstance;
    if (ssIsFirstInitCond(S) && fullDebuggerInitialization==1) {
      /* do this only if simulation is starting */
      {
        unsigned int chartAlreadyPresent;
        chartAlreadyPresent = sf_debug_initialize_chart(_boatMachineNumber_,
          6,
          1,
          1,
          3,
          0,
          0,
          0,
          0,
          0,
          &(chartInstance->chartNumber),
          &(chartInstance->instanceNumber),
          ssGetPath(S),
          (void *)S);
        if (chartAlreadyPresent==0) {
          /* this is the first instance */
          init_script_number_translation(_boatMachineNumber_,
            chartInstance->chartNumber);
          sf_debug_set_chart_disable_implicit_casting(_boatMachineNumber_,
            chartInstance->chartNumber,1);
          sf_debug_set_chart_event_thresholds(_boatMachineNumber_,
            chartInstance->chartNumber,
            0,
            0,
            0);
          _SFD_SET_DATA_PROPS(0,1,1,0,"C");
          _SFD_SET_DATA_PROPS(1,1,1,0,"u");
          _SFD_SET_DATA_PROPS(2,2,0,1,"y");
          _SFD_STATE_INFO(0,0,2);
          _SFD_CH_SUBSTATE_COUNT(0);
          _SFD_CH_SUBSTATE_DECOMP(0);
        }

        _SFD_CV_INIT_CHART(0,0,0,0);

        {
          _SFD_CV_INIT_STATE(0,0,0,0,0,0,NULL,NULL);
        }

        _SFD_CV_INIT_TRANS(0,0,NULL,NULL,0,NULL);

        /* Initialization of MATLAB Function Model Coverage */
        _SFD_CV_INIT_EML(0,1,1,0,0,0,0,0,0,0);
        _SFD_CV_INIT_EML_FCN(0,0,"eML_blk_kernel",0,-1,41);
        _SFD_TRANS_COV_WTS(0,0,0,1,0);
        if (chartAlreadyPresent==0) {
          _SFD_TRANS_COV_MAPS(0,
                              0,NULL,NULL,
                              0,NULL,NULL,
                              1,NULL,NULL,
                              0,NULL,NULL);
        }

        {
          unsigned int dimVector[2];
          dimVector[0]= 3;
          dimVector[1]= 6;
          _SFD_SET_DATA_COMPILED_PROPS(0,SF_DOUBLE,2,&(dimVector[0]),0,0,0,0.0,
            1.0,0,0,(MexFcnForType)c6_c_sf_marshallOut,(MexInFcnForType)NULL);
        }

        {
          unsigned int dimVector[1];
          dimVector[0]= 6;
          _SFD_SET_DATA_COMPILED_PROPS(1,SF_DOUBLE,1,&(dimVector[0]),0,0,0,0.0,
            1.0,0,0,(MexFcnForType)c6_b_sf_marshallOut,(MexInFcnForType)NULL);
        }

        {
          unsigned int dimVector[1];
          dimVector[0]= 3;
          _SFD_SET_DATA_COMPILED_PROPS(2,SF_DOUBLE,1,&(dimVector[0]),0,0,0,0.0,
            1.0,0,0,(MexFcnForType)c6_sf_marshallOut,(MexInFcnForType)
            c6_sf_marshallIn);
        }

        {
          real_T (*c6_C)[18];
          real_T (*c6_u)[6];
          real_T (*c6_y)[3];
          c6_y = (real_T (*)[3])ssGetOutputPortSignal(chartInstance->S, 1);
          c6_u = (real_T (*)[6])ssGetInputPortSignal(chartInstance->S, 1);
          c6_C = (real_T (*)[18])ssGetInputPortSignal(chartInstance->S, 0);
          _SFD_SET_DATA_VALUE_PTR(0U, *c6_C);
          _SFD_SET_DATA_VALUE_PTR(1U, *c6_u);
          _SFD_SET_DATA_VALUE_PTR(2U, *c6_y);
        }
      }
    } else {
      sf_debug_reset_current_state_configuration(_boatMachineNumber_,
        chartInstance->chartNumber,chartInstance->instanceNumber);
    }
  }
}

static const char* sf_get_instance_specialization()
{
  return "tf5joZ0JMkPs7fTC4QeRNB";
}

static void sf_opaque_initialize_c6_boat(void *chartInstanceVar)
{
  chart_debug_initialization(((SFc6_boatInstanceStruct*) chartInstanceVar)->S,0);
  initialize_params_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
  initialize_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_enable_c6_boat(void *chartInstanceVar)
{
  enable_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_disable_c6_boat(void *chartInstanceVar)
{
  disable_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_gateway_c6_boat(void *chartInstanceVar)
{
  sf_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
}

extern const mxArray* sf_internal_get_sim_state_c6_boat(SimStruct* S)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_raw2high");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = (mxArray*) get_sim_state_c6_boat((SFc6_boatInstanceStruct*)
    chartInfo->chartInstance);         /* raw sim ctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c6_boat();/* state var info */
  mxError = sf_mex_call_matlab(1, plhs, 4, prhs, "sfprivate");
  mxDestroyArray(prhs[0]);
  mxDestroyArray(prhs[1]);
  mxDestroyArray(prhs[2]);
  mxDestroyArray(prhs[3]);
  if (mxError || plhs[0] == NULL) {
    sf_mex_error_message("Stateflow Internal Error: \nError calling 'chart_simctx_raw2high'.\n");
  }

  return plhs[0];
}

extern void sf_internal_set_sim_state_c6_boat(SimStruct* S, const mxArray *st)
{
  ChartInfoStruct *chartInfo = (ChartInfoStruct*) ssGetUserData(S);
  mxArray *plhs[1] = { NULL };

  mxArray *prhs[4];
  int mxError = 0;
  prhs[0] = mxCreateString("chart_simctx_high2raw");
  prhs[1] = mxCreateDoubleScalar(ssGetSFuncBlockHandle(S));
  prhs[2] = mxDuplicateArray(st);      /* high level simctx */
  prhs[3] = (mxArray*) sf_get_sim_state_info_c6_boat();/* state var info */
  mxError = sf_mex_call_matlab(1, plhs, 4, prhs, "sfprivate");
  mxDestroyArray(prhs[0]);
  mxDestroyArray(prhs[1]);
  mxDestroyArray(prhs[2]);
  mxDestroyArray(prhs[3]);
  if (mxError || plhs[0] == NULL) {
    sf_mex_error_message("Stateflow Internal Error: \nError calling 'chart_simctx_high2raw'.\n");
  }

  set_sim_state_c6_boat((SFc6_boatInstanceStruct*)chartInfo->chartInstance,
                        mxDuplicateArray(plhs[0]));
  mxDestroyArray(plhs[0]);
}

static const mxArray* sf_opaque_get_sim_state_c6_boat(SimStruct* S)
{
  return sf_internal_get_sim_state_c6_boat(S);
}

static void sf_opaque_set_sim_state_c6_boat(SimStruct* S, const mxArray *st)
{
  sf_internal_set_sim_state_c6_boat(S, st);
}

static void sf_opaque_terminate_c6_boat(void *chartInstanceVar)
{
  if (chartInstanceVar!=NULL) {
    SimStruct *S = ((SFc6_boatInstanceStruct*) chartInstanceVar)->S;
    if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
      sf_clear_rtw_identifier(S);
    }

    finalize_c6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
    free((void *)chartInstanceVar);
    ssSetUserData(S,NULL);
  }

  unload_boat_optimization_info();
}

static void sf_opaque_init_subchart_simstructs(void *chartInstanceVar)
{
  initSimStructsc6_boat((SFc6_boatInstanceStruct*) chartInstanceVar);
}

extern unsigned int sf_machine_global_initializer_called(void);
static void mdlProcessParameters_c6_boat(SimStruct *S)
{
  int i;
  for (i=0;i<ssGetNumRunTimeParams(S);i++) {
    if (ssGetSFcnParamTunable(S,i)) {
      ssUpdateDlgParamAsRunTimeParam(S,i);
    }
  }

  if (sf_machine_global_initializer_called()) {
    initialize_params_c6_boat((SFc6_boatInstanceStruct*)(((ChartInfoStruct *)
      ssGetUserData(S))->chartInstance));
  }
}

static void mdlSetWorkWidths_c6_boat(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
    mxArray *infoStruct = load_boat_optimization_info();
    int_T chartIsInlinable =
      (int_T)sf_is_chart_inlinable(S,sf_get_instance_specialization(),infoStruct,
      6);
    ssSetStateflowIsInlinable(S,chartIsInlinable);
    ssSetRTWCG(S,sf_rtw_info_uint_prop(S,sf_get_instance_specialization(),
                infoStruct,6,"RTWCG"));
    ssSetEnableFcnIsTrivial(S,1);
    ssSetDisableFcnIsTrivial(S,1);
    ssSetNotMultipleInlinable(S,sf_rtw_info_uint_prop(S,
      sf_get_instance_specialization(),infoStruct,6,
      "gatewayCannotBeInlinedMultipleTimes"));
    if (chartIsInlinable) {
      ssSetInputPortOptimOpts(S, 0, SS_REUSABLE_AND_LOCAL);
      ssSetInputPortOptimOpts(S, 1, SS_REUSABLE_AND_LOCAL);
      sf_mark_chart_expressionable_inputs(S,sf_get_instance_specialization(),
        infoStruct,6,2);
      sf_mark_chart_reusable_outputs(S,sf_get_instance_specialization(),
        infoStruct,6,1);
    }

    sf_set_rtw_dwork_info(S,sf_get_instance_specialization(),infoStruct,6);
    ssSetHasSubFunctions(S,!(chartIsInlinable));
  } else {
  }

  ssSetOptions(S,ssGetOptions(S)|SS_OPTION_WORKS_WITH_CODE_REUSE);
  ssSetChecksum0(S,(1206294952U));
  ssSetChecksum1(S,(1273004468U));
  ssSetChecksum2(S,(3295459203U));
  ssSetChecksum3(S,(1416633881U));
  ssSetmdlDerivatives(S, NULL);
  ssSetExplicitFCSSCtrl(S,1);
}

static void mdlRTW_c6_boat(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S)) {
    ssWriteRTWStrParam(S, "StateflowChartType", "Embedded MATLAB");
  }
}

static void mdlStart_c6_boat(SimStruct *S)
{
  SFc6_boatInstanceStruct *chartInstance;
  chartInstance = (SFc6_boatInstanceStruct *)malloc(sizeof
    (SFc6_boatInstanceStruct));
  memset(chartInstance, 0, sizeof(SFc6_boatInstanceStruct));
  if (chartInstance==NULL) {
    sf_mex_error_message("Could not allocate memory for chart instance.");
  }

  chartInstance->chartInfo.chartInstance = chartInstance;
  chartInstance->chartInfo.isEMLChart = 1;
  chartInstance->chartInfo.chartInitialized = 0;
  chartInstance->chartInfo.sFunctionGateway = sf_opaque_gateway_c6_boat;
  chartInstance->chartInfo.initializeChart = sf_opaque_initialize_c6_boat;
  chartInstance->chartInfo.terminateChart = sf_opaque_terminate_c6_boat;
  chartInstance->chartInfo.enableChart = sf_opaque_enable_c6_boat;
  chartInstance->chartInfo.disableChart = sf_opaque_disable_c6_boat;
  chartInstance->chartInfo.getSimState = sf_opaque_get_sim_state_c6_boat;
  chartInstance->chartInfo.setSimState = sf_opaque_set_sim_state_c6_boat;
  chartInstance->chartInfo.getSimStateInfo = sf_get_sim_state_info_c6_boat;
  chartInstance->chartInfo.zeroCrossings = NULL;
  chartInstance->chartInfo.outputs = NULL;
  chartInstance->chartInfo.derivatives = NULL;
  chartInstance->chartInfo.mdlRTW = mdlRTW_c6_boat;
  chartInstance->chartInfo.mdlStart = mdlStart_c6_boat;
  chartInstance->chartInfo.mdlSetWorkWidths = mdlSetWorkWidths_c6_boat;
  chartInstance->chartInfo.extModeExec = NULL;
  chartInstance->chartInfo.restoreLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.restoreBeforeLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.storeCurrentConfiguration = NULL;
  chartInstance->S = S;
  ssSetUserData(S,(void *)(&(chartInstance->chartInfo)));/* register the chart instance with simstruct */
  init_dsm_address_info(chartInstance);
  if (!sim_mode_is_rtw_gen(S)) {
  }

  sf_opaque_init_subchart_simstructs(chartInstance->chartInfo.chartInstance);
  chart_debug_initialization(S,1);
}

void c6_boat_method_dispatcher(SimStruct *S, int_T method, void *data)
{
  switch (method) {
   case SS_CALL_MDL_START:
    mdlStart_c6_boat(S);
    break;

   case SS_CALL_MDL_SET_WORK_WIDTHS:
    mdlSetWorkWidths_c6_boat(S);
    break;

   case SS_CALL_MDL_PROCESS_PARAMETERS:
    mdlProcessParameters_c6_boat(S);
    break;

   default:
    /* Unhandled method */
    sf_mex_error_message("Stateflow Internal Error:\n"
                         "Error calling c6_boat_method_dispatcher.\n"
                         "Can't handle method %d.\n", method);
    break;
  }
}
