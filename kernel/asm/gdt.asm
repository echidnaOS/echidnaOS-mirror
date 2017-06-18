global load_GDT
global load_TSS
global set_userspace

section .data

align 4
GDT:
    dw .GDTEnd - .GDTStart - 1	; GDT size
    dd .GDTStart				; GDT start

    align 4
    .GDTStart:

    .NullDescriptor:
        dw 0x0000			; Limit
        dw 0x0000			; Base (low 16 bits)
        db 0x00				; Base (mid 8 bits)
        db 00000000b		; Access
        db 00000000b		; Granularity
        db 0x00				; Base (high 8 bits)

    .KernelCode:
        dw 0xFFFF			; Limit
        dw 0x0000			; Base (low 16 bits)
        db 0x00				; Base (mid 8 bits)
        db 10011010b		; Access
        db 11001111b		; Granularity
        db 0x00				; Base (high 8 bits)

    .KernelData:
        dw 0xFFFF			; Limit
        dw 0x0000			; Base (low 16 bits)
        db 0x00				; Base (mid 8 bits)
        db 10010010b		; Access
        db 11001111b		; Granularity
        db 0x00				; Base (high 8 bits)

    .UserCode:
.llowc  dw 0x0000			; Limit
.blowc  dw 0x0000			; Base (low 16 bits)
.bmidc  db 0x00				; Base (mid 8 bits)
        db 11111010b		; Access
.granc  db 11000000b		; Granularity
.bhighc db 0x00				; Base (high 8 bits)

    .UserData:
.llowd  dw 0x0000			; Limit
.blowd  dw 0x0000			; Base (low 16 bits)
.bmidd  db 0x00				; Base (mid 8 bits)
        db 11110010b		; Access
.grand  db 11000000b		; Granularity
.bhighd db 0x00				; Base (high 8 bits)

    .UnrealCode:
        dw 0xFFFF			; Limit
        dw 0x0000			; Base (low 16 bits)
        db 0x00				; Base (mid 8 bits)
        db 10011010b		; Access
        db 10001111b		; Granularity
        db 0x00				; Base (high 8 bits)

    .UnrealData:
        dw 0xFFFF			; Limit
        dw 0x0000			; Base (low 16 bits)
        db 0x00				; Base (mid 8 bits)
        db 10010010b		; Access
        db 10001111b		; Granularity
        db 0x00				; Base (high 8 bits)

    .TSS:
.llowt  dw 0x0000			; Limit
.blowt  dw 0x0000			; Base (low 16 bits)
.bmidt  db 0x00				; Base (mid 8 bits)
        db 11101001b		; Access
        db 00000000b		; Granularity
.bhight db 0x00				; Base (high 8 bits)

    .GDTEnd:

align 4
TSS:
    dd 0
    dd 0xEFFFF0
    dd 0x10
    times 23 dd 0
    .end:

section .text

bits 32

load_GDT:
    lgdt [GDT]
    jmp 0x08:.load_cs
    .load_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

load_TSS:
    mov eax, TSS                ; fetch base
    mov word [GDT.blowt], ax
    shr eax, 16
    mov byte [GDT.bmidt], al
    mov byte [GDT.bhight], ah

    mov eax, (TSS.end - TSS)    ; fetch limit
    mov word [GDT.llowt], ax
    
    mov ax, 0x3B
    ltr ax
    
    ret    

; void set_userspace(uint32_t base, uint32_t page_count);
set_userspace:
    mov eax, dword [esp+4]      ; fetch base
    mov word [GDT.blowc], ax
    mov word [GDT.blowd], ax
    shr eax, 16
    mov byte [GDT.bmidc], al
    mov byte [GDT.bmidd], al
    mov byte [GDT.bhighc], ah
    mov byte [GDT.bhighd], ah

    mov eax, dword [esp+8]      ; fetch limit
    dec eax
    mov word [GDT.llowc], ax
    mov word [GDT.llowd], ax
    shr eax, 16
    and al, 00001111b
    and byte [GDT.granc], 11110000b
    and byte [GDT.grand], 11110000b
    or byte [GDT.granc], al
    or byte [GDT.grand], al
    
    ret