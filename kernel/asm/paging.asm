global full_identity_map
global kernel_pagemap

section .data

align 4096
kernel_pt:
    times (512 * 8) * 2048 db 0

kernel_pd:
    .1:
    times 512 dq 0
    .2:
    times 512 dq 0
    .3:
    times 512 dq 0
    .4:
    times 512 dq 0

kernel_pdpt:
    times 512 dq 0

kernel_pml4:
kernel_pagemap:
    times 512 dq 0

section .text

bits 64

full_identity_map:

    ; Identity map all 4GB of memory for the kernel

    push rbx
    push rbp

    mov rdi, kernel_pt
    mov rax, 0x07
    mov rcx, 512 * 2048
    .loop:
        stosq
        add rax, 0x1000
        loop .loop

    mov rdi, kernel_pd.1
    mov rax, kernel_pt
    or rax, 0x07
    mov rcx, 512 * 4
    .loop1:
        stosq
        add rax, 0x1000
        loop .loop1

    mov rdi, kernel_pdpt
    mov rax, kernel_pd.1
    or rax, 0x07
    stosq
    mov rax, kernel_pd.2
    or rax, 0x07
    stosq
    mov rax, kernel_pd.3
    or rax, 0x07
    stosq
    mov rax, kernel_pd.4
    or rax, 0x07
    stosq

    mov rdi, kernel_pml4
    mov rax, kernel_pdpt
    or rax, 0x07
    stosq

    mov rdx, kernel_pml4
    mov cr3, rdx

    pop rbp
    pop rbx
    ret
