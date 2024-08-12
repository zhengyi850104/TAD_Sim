/*
 * Engine_private.h
 *
 * Code generation for model "Engine".
 *
 * Model version              : 2.21
 * Simulink Coder version : 9.8 (R2022b) 13-May-2022
 * C++ source code generated on : Wed Dec  6 13:01:57 2023
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Engine_private_h_
#define RTW_HEADER_Engine_private_h_
#include "Engine.h"
#include "Engine_types.h"
#include "multiword_types.h"
#include "rtwtypes.h"

/* Private macros used by the generated code to access rtModel */
#ifndef rtmIsMajorTimeStep
#  define rtmIsMajorTimeStep(rtm) (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
#  define rtmIsMinorTimeStep(rtm) (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmSetTPtr
#  define rtmSetTPtr(rtm, val) ((rtm)->Timing.t = (val))
#endif

extern real_T look2_binlcapw(real_T u0, real_T u1, const real_T bp0[], const real_T bp1[], const real_T table[],
                             const uint32_T maxIndex[], uint32_T stride);

/* private model entry point functions */
extern void Engine_derivatives();

#endif /* RTW_HEADER_Engine_private_h_ */
