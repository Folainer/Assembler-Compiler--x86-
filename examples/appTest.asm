data segment
    Data1 db 1010b
    Data2 db "HELLO_WORLD", 0
    Data3 dw 10d, 11d
    Data4 dd 10b 
    Data5 dd 10
    Year equ 365
    condition equ 1
data ends

code segment
    empty dd 0

begin: 
    if condition
    mov eax, 8h
    else 
    mov al, 0h
    endif
    mov cl, 0h
    test: mov eax, 20
    inc eax
    inc bl
    mov ebx, 20h
    add eax, ebx
    mov eax, Year
    xor cl, Data2[ebx+edi]
    or Data2[ebx+edi+10], 11110101b
    jbe next
    mov eax, 4
    mov edx, 0
next: 
    dec Data2[ebp+edi+4]
    and ds:Data2[ebx+esi], bl
    movsb
    jbe begin

code ends
end begin


