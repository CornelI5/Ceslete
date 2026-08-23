bits 64
global _start
extern dotshirp_main

section .data
    handoff:
        dq 0
        dd 0
        dd 0
        dd 0
        dd 0
        dq 0
        dq 0
        dq 0
        dq 0
        dq 0
        dd 1
        dd 0

section .text
_start:
    lea rdi, [rel handoff]
    call dotshirp_main
    cli
    hlt
