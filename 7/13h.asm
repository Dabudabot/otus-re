#make_boot#

org 7c00h      ; set location counter.

; READ

mov ah, 2     ; function read
mov al, 1     ; amount of sectors
mov dh, 0     ; head 
mov dl, 1     ; floppy 1
mov ch, 0     ; cylinder
mov cl, 1     ; sector number
lea bx, buf   ; buffer to read to
int 13h

; OPERATE

mov cx, 255
mov si, bx
mov di, si
L:
    lodsb        ; read to al and go next
    xor al, 0x90
    stosb        ; write to di and go next
    loop L

; WRITE

mov ah, 3     ; function write
mov al, 1     ; amount of sectors
mov dh, 0     ; head 
mov dl, 1     ; floppy 1
mov ch, 0     ; cylinder
mov cl, 2     ; sector number
lea bx, buf   ; buffer to write from
int 13h

INT 19h        ; reboot

buf db 0xff dup(0)