%define REG_NONE 0
%define REG_A   1 
%define REG_X   2
%define REG_Y   4
%define REG_PC  8

%define INST_LD 1
%define INST_ADD 2
%define INST_ST 3
%define INST_INC 4
%define INST_DEC 5
%define INST_MV 6
%define INST_JZ 7
%define INST_JMP 8
%define INST_JNZ 9
%define INST_SUB 10
%define INST_RAND 11
%define INST_SPAWN 12
%define INST_LDC 13
%define INST_MUTATE 14

%macro regss 2
    db (%1 << 4 | %2)
%endmacro

%macro EMIT_INSTR 4
    ; instruction
    db %1
    regss %2, %3
    dw $0
    ; arg
    dd %4
%endmacro

%macro LDA 1
    EMIT_INSTR INST_LD, REG_A, REG_NONE, %1
%endmacro

%macro LDX 1
    EMIT_INSTR INST_LD, REG_X, REG_NONE, %1
%endmacro

%macro LDY 1
    EMIT_INSTR INST_LD, REG_Y, REG_NONE, %1
%endmacro

%macro LDAA 0
    EMIT_INSTR INST_LD, REG_A, REG_A, $0
%endmacro

%macro LDAX 0
    EMIT_INSTR INST_LD, REG_A, REG_X, $0
%endmacro

%macro LDAY 0
    EMIT_INSTR INST_LD, REG_A, REG_Y, $0
%endmacro

%macro LDXA 0
    EMIT_INSTR INST_LD, REG_X, REG_A, $0
%endmacro

%macro LDXX 0
    EMIT_INSTR INST_LD, REG_X, REG_X, $0
%endmacro

%macro LDXY 0
    EMIT_INSTR INST_LD, REG_X, REG_Y, $0
%endmacro

%macro LDYA 0
    EMIT_INSTR INST_LD, REG_Y, REG_A, $0
%endmacro

%macro LDYX 0
    EMIT_INSTR INST_LD, REG_Y, REG_X, $0
%endmacro

%macro LDYY 0
    EMIT_INSTR INST_LD, REG_Y, REG_Y, $0
%endmacro

%macro MVAP 0
    EMIT_INSTR INST_MV, REG_A, REG_PC, $0
%endmacro

%macro STA 1
    EMIT_INSTR INST_ST, REG_A, REG_NONE, %1
%endmacro

%macro RANDA 0
    EMIT_INSTR INST_RAND, REG_A, REG_NONE, $0
%endmacro

%macro RANDX 0
    EMIT_INSTR INST_RAND, REG_X, REG_NONE, $0
%endmacro

%macro LDCX 1
    EMIT_INSTR INST_LDC, REG_X, REG_NONE, %1
%endmacro

%macro LDCA 1
    EMIT_INSTR INST_LDC, REG_A, REG_NONE, %1
%endmacro

%macro LDCY 1
    EMIT_INSTR INST_LDC, REG_Y, REG_NONE, %1
%endmacro

%macro ADDAY 0
    EMIT_INSTR INST_ADD, REG_A, REG_Y, $0
%endmacro

%macro ADDXY 0
    EMIT_INSTR INST_ADD, REG_X, REG_Y, $0
%endmacro

%macro STAX 0
    EMIT_INSTR INST_ST, REG_A, REG_X, $0
%endmacro

%macro STXA 0
    EMIT_INSTR INST_ST, REG_X, REG_A, $0
%endmacro

%macro DECX 0
    EMIT_INSTR INST_DEC, REG_X, REG_NONE, $0
%endmacro

%macro DECY 0
    EMIT_INSTR INST_DEC, REG_Y, REG_NONE, $0
%endmacro

%macro JNZY 1
    EMIT_INSTR INST_JNZ, REG_Y, REG_NONE, %1
%endmacro

%macro JMP 1
    EMIT_INSTR INST_JMP, REG_NONE, REG_NONE, %1
%endmacro

%macro SPAWNA 0
    EMIT_INSTR INST_SPAWN, REG_A, REG_NONE, $0
%endmacro

%macro SPAWNX 0
    EMIT_INSTR INST_SPAWN, REG_X, REG_NONE, $0
%endmacro

%macro SPAWN 1
    EMIT_INSTR INST_SPAWN, REG_NONE, REG_NONE, %1
%endmacro


%macro MUTATEY 0
    EMIT_INSTR INST_MUTATE, REG_Y, REG_NONE, $0
%endmacro



%macro MUTATEA 0
    EMIT_INSTR INST_MUTATE, REG_A, REG_NONE, $0
%endmacro

%macro SUBA 1
    EMIT_INSTR INST_SUB, REG_A, REG_NONE, %1
%endmacro



section .data
start:
lol:
    RANDX
    LDCY (end-start)>>2
copy:
    DECY
    DECX
    MVAP
    SUBA 0xa-0x2
    ADDAY
    LDAA
    MUTATEA
    STXA
    JNZY ($-copy)>>2
    SPAWNX
    JMP ($-lol)>>2
end:
