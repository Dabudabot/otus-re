#make_boot#

org 7c00h      ; set location counter.

; CODE
    
    lea si, s1
    call OUTPUT

    lea di, b1
    call INPUT
    
    call NEW_LINE
    
    lea si, s2
    call OUTPUT
     
    lea di, b2
    call INPUT
    
    call NEW_LINE
    
    lea si, sm
    call OUTPUT
    
    
    call CALC
    
    jmp EXIT
    
GET_SLAG:
    lea di, b1   ; b1 stored here
    inc di
    mov bl, [di] ; bl = size of b1
    mov bh, 0
    
    lea si, b2   ; b2 stored here
    inc si
    mov al, [si] ; al = size of b2
    
    ; if second number has more digits
    cmp al, bl
    jle GET_SLAG_RET
    ; then we will swap src and dst
    lea di, b2
    inc di
    mov bl, [di] ; bl = size of b2
    mov bh, 0
    
    lea si, b1
    inc si
    mov al, [si] ; al = size of b1
GET_SLAG_RET:
    mov ah, 0
    ret    
    
CALC:
    call GET_SLAG
    add di, bx   ; move to last digit
    add si, ax
CALC_LOOP:
    cmp ax, 0h
    je LAST_CHAR
    mov cl, [di]
    mov dl, [si]
    add cl, dl
LAST_CHAR:
    add cl, ch
    mov ch, 0h
    cmp cl, 0fh
    jle CALC_RES
    mov ch, 1h
    sub cl, 10h
CALC_RES:
    mov [di], cl
    dec di
    dec bx
    cmp ax, 0h
    je CALC_END
    dec si
    dec ax
    jnz CALC_LOOP
    cmp ch, 0h
    je CALC_END
    mov cl, [di]
    cmp bx, 0h
    jne LAST_CHAR
    mov al, 31h
    call OUTPUT_CHAR        
CALC_END:
    call GET_SLAG
    inc di
LOOP1:
    mov al, [di]
    cmp al, 30h ; if less 0 
    jl PARSE_BACK1 
    cmp al, 39h ; if less or equal 9
    jle NEXT
    cmp al, 41h ; if less A
    jl ERROR
    cmp al, 46h ; if less or equal F
    jle NEXT
    cmp al, 61h ; if less a
    jl ERROR
    cmp al, 66h ; if less or equal f
    jle NEXT
    jg ERROR
PARSE_BACK1:   
    add al, 30h
    cmp al, 3ah
    jl NEXT
    add al, 7h
NEXT:
    mov [si], al
    ; print digit
    call OUTPUT_CHAR
    inc di 
    dec bl 
    jnz LOOP1
    ret
    
OUTPUT_CHAR:
    mov ah, 0eh
    mov bh, 0h
    int 10h
    mov ah, 02h
    inc [col]
    cmp [col], 30h
    jl  OLD_LINE
    mov [col], 0h
    inc [row]
OLD_LINE:
    mov dh, [row]
    mov dl, [col]    
    int 10h
    ret
NEW_LINE:
    mov ah, 02h
    inc [row]
    mov [col], 0h
    mov dh, [row]
    mov dl, [col]    
    int 10h
    ret 
OUTPUT:
    mov al, [si]
    cmp al, 0h
    je OUTPUT_END
    call OUTPUT_CHAR
    inc si
    jmp OUTPUT
OUTPUT_END:
    ret
    
INPUT:
    mov cl, [di]
    inc di
    mov bl, [di]
    mov bh, 0h
INPUT_LOOP:    
    cmp cl, bl
    je INPUT_END
    call INPUT_CHAR
    cmp al, 0dh
    je INPUT_END 
    ; save
    inc bl
    add di, bx
    mov [di], al
    sub di, bx
    mov [di], bl
    ; print
    mov al, ah
    mov ah, 0eh
    int 10h
 mov ah, 02h
 inc [col]
    cmp [col], 30h
    jl  OLD_LINE2
    mov [col], 0h
    inc [row]
OLD_LINE2:
    mov dh, [row]
    mov dl, [col]    
    int 10h
    jmp INPUT_LOOP 
INPUT_END:    
    cmp bl, 0h
    je ERROR
    ret
INPUT_CHAR_END:
    ret
INPUT_CHAR:    
    mov ah, 0h
    int 16h
    cmp al, 0dh ; if enter
    je INPUT_CHAR_END
    mov ah, al
    cmp al, 30h ; if less 0 
    jl ERROR 
    cmp al, 39h ; if less or equal 9
    jle DECIMAL
    cmp al, 41h ; if less A
    jl ERROR
    cmp al, 46h ; if less or equal F
    jle HEX1
    cmp al, 61h ; if less a
    jl ERROR
    cmp al, 66h ; if less or equal f
    jle HEX2
ERROR:
    lea si, se
    call OUTPUT
    jmp EXIT
DECIMAL:
    sub al, 30h
    ret
HEX1:
    sub al, 37h
    ret
HEX2:
    sub al, 57h
    ret

; DATA
s1 db 'Slag1 = ', 0h
s2 db 'Slag2 = ', 0h
sm db 'Summa: ', 0h
se db 'some error', 0h

b1 db 6         ; buffer size
    db 00       ; buffer pointer
    db 6 dup(0) ; buffer itself
b2 db 6         ; buffer size
    db 00       ; buffer pointer
    db 6 dup(0) ; buffer itself
col db 0h       ; coursor col saved here
row db 0h       ; coursor row saved here

EXIT:
    INT 19h        ; reboot