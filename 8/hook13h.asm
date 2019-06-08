#make_boot#

org 7c00h      ; set location counter.

mov ax, 0xffff
mov sp, ax

mov ah, 2
mov al, 1
mov dx, 1
mov cx, 1
lea bx, buf
int 13h                 ; read from floppy_1

cli
mov ax, [4eh]
mov [cs:Int_13+2], ax    ; look line 49 second 0

mov ax, [4ch]
mov [cs:Int_13], ax      ; loog line 46 first 0

mov [4eh], cs
mov ax, newInt13
mov [4ch], ax
sti


mov ah, 2
mov al, 1
mov dx, 1
mov cx, 1
lea bx, buf             ; int13 hooked
int 13h                 ; read from floppy_1

xor ax, ax
int 20h

newInt13:
    cmp ah, 2
    jz exec
    
    db 0xea
Int_13:
    dw 0,0
exec:
    pushf
    call far [Int_13]
    jc intRet
    
    pusha
    pushf
    
    xor byte es:[bx], 0x90  ;code
    
    popf
    popa
intRet:    
    iret
      

db 510-($-start) dup(0), 0x55, 0xaa
buf db 0xff dup(0)

INT 19h        ; reboot 