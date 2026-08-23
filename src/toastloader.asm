bits 64
global _start
extern dotshirp_main

section .data
    gop_guid:
        dd 0x9042A9A5
        dw 0x47FC, 0xD4D3
        db 0xB8, 0x9C, 0xE2, 0x6E, 0x4F, 0x3B, 0x8C, 0xA1
    mmap_key: dq 0
    handoff:
        dq 0, 0, 0, 0, 0
        dq 0, 0, 0, 0, 0
        dd 1, 0

section .text
_start:
    mov [rel handoff + 80], rdx

    mov rcx, [rdx + 64]
    lea rdx, [rel gop_guid]
    xor r8, r8
    lea r9, [rel gop_ptr]
    sub rsp, 40
    call [rcx + 160]
    add rsp, 40
    test rax, rax
    jnz .no_gop

    mov rcx, [rel gop_ptr]
    mov rcx, [rcx]
    mov rdx, 0
    xor r8, r8
    xor r9, r9
    lea r10, [rel fb_info]
    sub rsp, 48
    call [rcx + 40]
    add rsp, 48
    test rax, rax
    jnz .no_gop

    mov rax, [rel fb_info]
    mov [rel handoff], rax
    mov eax, [rel fb_info + 8]
    mov [rel handoff + 8], eax
    mov eax, [rel fb_info + 12]
    mov [rel handoff + 12], eax
    mov eax, [rel fb_info + 16]
    mov [rel handoff + 16], eax
    mov eax, [rel fb_info + 20]
    mov [rel handoff + 20], eax
    or dword [rel handoff + 88], 1

.no_gop:
    lea rdi, [rel handoff]
    call dotshirp_main
    cli
    hlt

section .bss
    gop_ptr: resq 1
    fb_info: resb 32
