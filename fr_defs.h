#ifndef _FR_DEFS_
#define _FR_DEFS_


#define FR_STARTING_R_BITS  1
#define FR_INCREMENT        1

#define FR_ROUNDS           1

#define FR_USE_ALG_CHECKING
#define FR_USE_INSTR_CHECKING

#if !defined(FR_USE_ALG_CHECKING) && !defined(FR_USE_INSTR_CHECKING)
//#error "Must choose at least one type of checking"
#endif

// FR_ROUNDS = 10
#if FR_ROUNDS == 10
#define CONTROL_CONST   0x271
#elif FR_ROUNDS == 3
#define CONTROL_CONST   0xc1
#elif FR_ROUNDS == 2
#define CONTROL_CONST   0x88
#elif FR_ROUNDS == 1
#define CONTROL_CONST   0x3d
#endif

#if !defined(FR_USE_INSTR_CHECKING)
#undef FR_ROUNDS
#define FR_ROUNDS 1
#endif

extern word_t DATA_ERRORS;
extern word_t CONTROL_ERRORS;
extern word_t CONTROL_SHIFT;


#endif
