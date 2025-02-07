/* Produced by CVXGEN, 2015-07-07 00:46:46 -0400.  */
/* CVXGEN is Copyright (C) 2006-2012 Jacob Mattingley, jem@cvxgen.com. */
/* The code in this file is Copyright (C) 2006-2012 Jacob Mattingley. */
/* CVXGEN, or solvers produced by CVXGEN, cannot be used for commercial */
/* applications without prior written permission from Jacob Mattingley. */

/* Filename: csolve.c. */
/* Description: mex-able file for running cvxgen solver. */
#include "mex.h"
#include "solver.h"
Vars vars;
Params params;
Workspace work;
Settings settings;
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  int i, j;
  mxArray *xm, *cell, *xm_cell;
  double *src;
  double *dest;
  double *dest_cell;
  int valid_vars;
  int steps;
  int this_var_errors;
  int warned_diags;
  int prepare_for_c = 0;
  int extra_solves;
  const char *status_names[] = {"optval", "gap", "steps", "converged"};
  mwSize dims1x1of1[1] = {1};
  mwSize dims[1];
  const char *var_names[] = {"da", "du"};
  const int num_var_names = 2;
  /* Avoid compiler warnings of unused variables by using a dummy assignment. */
  warned_diags = j = 0;
  extra_solves = 0;
  set_defaults();
  /* Check we got the right number of arguments. */
  if (nrhs == 0)
    mexErrMsgTxt("Not enough arguments: You need to specify at least the parameters.\n");
  if (nrhs > 1) {
    /* Assume that the second argument is the settings. */
    if (mxGetField(prhs[1], 0, "eps") != NULL)
      settings.eps = *mxGetPr(mxGetField(prhs[1], 0, "eps"));
    if (mxGetField(prhs[1], 0, "max_iters") != NULL)
      settings.max_iters = *mxGetPr(mxGetField(prhs[1], 0, "max_iters"));
    if (mxGetField(prhs[1], 0, "refine_steps") != NULL)
      settings.refine_steps = *mxGetPr(mxGetField(prhs[1], 0, "refine_steps"));
    if (mxGetField(prhs[1], 0, "verbose") != NULL)
      settings.verbose = *mxGetPr(mxGetField(prhs[1], 0, "verbose"));
    if (mxGetField(prhs[1], 0, "better_start") != NULL)
      settings.better_start = *mxGetPr(mxGetField(prhs[1], 0, "better_start"));
    if (mxGetField(prhs[1], 0, "verbose_refinement") != NULL)
      settings.verbose_refinement = *mxGetPr(mxGetField(prhs[1], 0,
            "verbose_refinement"));
    if (mxGetField(prhs[1], 0, "debug") != NULL)
      settings.debug = *mxGetPr(mxGetField(prhs[1], 0, "debug"));
    if (mxGetField(prhs[1], 0, "kkt_reg") != NULL)
      settings.kkt_reg = *mxGetPr(mxGetField(prhs[1], 0, "kkt_reg"));
    if (mxGetField(prhs[1], 0, "s_init") != NULL)
      settings.s_init = *mxGetPr(mxGetField(prhs[1], 0, "s_init"));
    if (mxGetField(prhs[1], 0, "z_init") != NULL)
      settings.z_init = *mxGetPr(mxGetField(prhs[1], 0, "z_init"));
    if (mxGetField(prhs[1], 0, "resid_tol") != NULL)
      settings.resid_tol = *mxGetPr(mxGetField(prhs[1], 0, "resid_tol"));
    if (mxGetField(prhs[1], 0, "extra_solves") != NULL)
      extra_solves = *mxGetPr(mxGetField(prhs[1], 0, "extra_solves"));
    else
      extra_solves = 0;
    if (mxGetField(prhs[1], 0, "prepare_for_c") != NULL)
      prepare_for_c = *mxGetPr(mxGetField(prhs[1], 0, "prepare_for_c"));
  }
  valid_vars = 0;
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "Ba");
  if (xm == NULL) {
    printf("could not find params.Ba.\n");
  } else {
    if (!((mxGetM(xm) == 3) && (mxGetN(xm) == 2))) {
      printf("Ba must be size (3,2), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter Ba must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter Ba must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter Ba must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.Ba;
      src = mxGetPr(xm);
      for (i = 0; i < 6; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "Ohm");
  if (xm == NULL) {
    printf("could not find params.Ohm.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 2))) {
      printf("Ohm must be size (2,2), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter Ohm must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter Ohm must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter Ohm must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.Ohm;
      src = mxGetPr(xm);
      for (i = 0; i < 4; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "Q");
  if (xm == NULL) {
    printf("could not find params.Q.\n");
  } else {
    if (!((mxGetM(xm) == 3) && (mxGetN(xm) == 3))) {
      printf("Q must be size (3,3), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter Q must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter Q must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter Q must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.Q;
      src = mxGetPr(xm);
      for (i = 0; i < 9; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "a_0");
  if (xm == NULL) {
    printf("could not find params.a_0.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("a_0 must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter a_0 must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter a_0 must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter a_0 must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.a_0;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "a_max");
  if (xm == NULL) {
    printf("could not find params.a_max.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("a_max must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter a_max must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter a_max must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter a_max must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.a_max;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "a_min");
  if (xm == NULL) {
    printf("could not find params.a_min.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("a_min must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter a_min must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter a_min must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter a_min must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.a_min;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "da_max");
  if (xm == NULL) {
    printf("could not find params.da_max.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("da_max must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter da_max must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter da_max must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter da_max must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.da_max;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "jBa_u");
  if (xm == NULL) {
    printf("could not find params.jBa_u.\n");
  } else {
    if (!((mxGetM(xm) == 3) && (mxGetN(xm) == 2))) {
      printf("jBa_u must be size (3,2), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter jBa_u must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter jBa_u must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter jBa_u must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.jBa_u;
      src = mxGetPr(xm);
      for (i = 0; i < 6; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "tau");
  if (xm == NULL) {
    printf("could not find params.tau.\n");
  } else {
    if (!((mxGetM(xm) == 3) && (mxGetN(xm) == 1))) {
      printf("tau must be size (3,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter tau must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter tau must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter tau must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.tau;
      src = mxGetPr(xm);
      for (i = 0; i < 3; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "u_0");
  if (xm == NULL) {
    printf("could not find params.u_0.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("u_0 must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter u_0 must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter u_0 must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter u_0 must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.u_0;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "u_max");
  if (xm == NULL) {
    printf("could not find params.u_max.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("u_max must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter u_max must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter u_max must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter u_max must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.u_max;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  this_var_errors = 0;
  xm = mxGetField(prhs[0], 0, "u_min");
  if (xm == NULL) {
    printf("could not find params.u_min.\n");
  } else {
    if (!((mxGetM(xm) == 2) && (mxGetN(xm) == 1))) {
      printf("u_min must be size (2,1), not (%d,%d).\n", mxGetM(xm), mxGetN(xm));
      this_var_errors++;
    }
    if (mxIsComplex(xm)) {
      printf("parameter u_min must be real.\n");
      this_var_errors++;
    }
    if (!mxIsClass(xm, "double")) {
      printf("parameter u_min must be a full matrix of doubles.\n");
      this_var_errors++;
    }
    if (mxIsSparse(xm)) {
      printf("parameter u_min must be a full matrix.\n");
      this_var_errors++;
    }
    if (this_var_errors == 0) {
      dest = params.u_min;
      src = mxGetPr(xm);
      for (i = 0; i < 2; i++)
        *dest++ = *src++;
      valid_vars++;
    }
  }
  if (valid_vars != 12) {
    printf("Error: %d parameters are invalid.\n", 12 - valid_vars);
    mexErrMsgTxt("invalid parameters found.");
  }
  if (prepare_for_c) {
    printf("settings.prepare_for_c == 1. thus, outputting for C.\n");
    for (i = 0; i < 4; i++)
      printf("  params.Ohm[%d] = %.6g;\n", i, params.Ohm[i]);
    for (i = 0; i < 3; i++)
      printf("  params.tau[%d] = %.6g;\n", i, params.tau[i]);
    for (i = 0; i < 6; i++)
      printf("  params.Ba[%d] = %.6g;\n", i, params.Ba[i]);
    for (i = 0; i < 2; i++)
      printf("  params.u_0[%d] = %.6g;\n", i, params.u_0[i]);
    for (i = 0; i < 6; i++)
      printf("  params.jBa_u[%d] = %.6g;\n", i, params.jBa_u[i]);
    for (i = 0; i < 9; i++)
      printf("  params.Q[%d] = %.6g;\n", i, params.Q[i]);
    for (i = 0; i < 2; i++)
      printf("  params.u_min[%d] = %.6g;\n", i, params.u_min[i]);
    for (i = 0; i < 2; i++)
      printf("  params.u_max[%d] = %.6g;\n", i, params.u_max[i]);
    for (i = 0; i < 2; i++)
      printf("  params.a_min[%d] = %.6g;\n", i, params.a_min[i]);
    for (i = 0; i < 2; i++)
      printf("  params.a_0[%d] = %.6g;\n", i, params.a_0[i]);
    for (i = 0; i < 2; i++)
      printf("  params.a_max[%d] = %.6g;\n", i, params.a_max[i]);
    for (i = 0; i < 2; i++)
      printf("  params.da_max[%d] = %.6g;\n", i, params.da_max[i]);
  }
  /* Perform the actual solve in here. */
  steps = solve();
  /* For profiling purposes, allow extra silent solves if desired. */
  settings.verbose = 0;
  for (i = 0; i < extra_solves; i++)
    solve();
  /* Update the status variables. */
  plhs[1] = mxCreateStructArray(1, dims1x1of1, 4, status_names);
  xm = mxCreateDoubleMatrix(1, 1, mxREAL);
  mxSetField(plhs[1], 0, "optval", xm);
  *mxGetPr(xm) = work.optval;
  xm = mxCreateDoubleMatrix(1, 1, mxREAL);
  mxSetField(plhs[1], 0, "gap", xm);
  *mxGetPr(xm) = work.gap;
  xm = mxCreateDoubleMatrix(1, 1, mxREAL);
  mxSetField(plhs[1], 0, "steps", xm);
  *mxGetPr(xm) = steps;
  xm = mxCreateDoubleMatrix(1, 1, mxREAL);
  mxSetField(plhs[1], 0, "converged", xm);
  *mxGetPr(xm) = work.converged;
  /* Extract variable values. */
  plhs[0] = mxCreateStructArray(1, dims1x1of1, num_var_names, var_names);
  xm = mxCreateDoubleMatrix(2, 1, mxREAL);
  mxSetField(plhs[0], 0, "da", xm);
  dest = mxGetPr(xm);
  src = vars.da;
  for (i = 0; i < 2; i++) {
    *dest++ = *src++;
  }
  xm = mxCreateDoubleMatrix(2, 1, mxREAL);
  mxSetField(plhs[0], 0, "du", xm);
  dest = mxGetPr(xm);
  src = vars.du;
  for (i = 0; i < 2; i++) {
    *dest++ = *src++;
  }
}
