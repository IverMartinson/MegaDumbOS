org 0x1000
bits 32

.loop:
    lodsb
    or al, al
    jz .done

    mov [edi], al
    inc edi

    mov [edi], bl
    inc edi
    jmp .loop

.done:
    ret

mov bl, 6

mov edi, 0xb8000 + 320
mov esi, kernelloadedMsg
call .loop

kernelloadedMsg db "Kernel loaded!"