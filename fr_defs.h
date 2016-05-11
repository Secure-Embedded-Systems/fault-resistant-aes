#ifndef _FR_DEFS_
#define _FR_DEFS_


#define FR_STARTING_R_BITS  32
#define FR_STARTING_ROUNDS  10
#define FR_INCREMENT        1

//#define FR_ROUNDS           1

#define FR_USE_ALG_CHECKING
#define FR_USE_INSTR_CHECKING

#if !defined(FR_USE_ALG_CHECKING) && !defined(FR_USE_INSTR_CHECKING)
//#error "Must choose at least one type of checking"
#endif

#if !defined(FR_USE_INSTR_CHECKING)
#undef FR_STARTING_ROUNDS
#define FR_STARTING_ROUNDS 1
#endif

extern volatile word_t DATA_ERRORS;
extern volatile word_t CONTROL_ERRORS;
extern word_t CONTROL_SHIFT;
extern uint16_t CONTROL_VALS[10];
extern uint8_t FR_ROUNDS;


#endif
