#make_boot#

org 7c00h      ; set location counter.

mov ax, 3
int 10h      ; clrscr

mov ax, 0xb800
mov es, ax
lea si, str
xor di, di
mov ah, 7           ; scr attribute
L:
    lodsb           ; si -> al
    stosw           ; al -> di
    test al, al     ; if 0
    jnz L
    
INT 19h        ; reboot

str db 'hello world', 0