; simple adder of two hex number (5 char length)

data segment
    s1 db 'Slag1 = $'
    s2 db 0dh, 0ah, 'Slag2 = $'
    s3 db 0dh, 0ah, 'Summa: $'
    se db 0dh, 0ah, 'some error$'
    ; buffer 1 contains slag1 and answer after calc
    b1 db 6         ; buffer size
        db 00       ; buffer pointer
        db 6 dup(0) ; buffer itself
    ; buffer 2 contains slag2  
    b2 db 6         ; buffer2 size
        db 00       ; buffer2 pointer
        db 6 dup(0) ; buffer2 itself
ends

code segment
start:
    ; set segment registers:
    mov ax, data
    mov ds, ax
    mov es, ax

    ; print "slag1 = "        
    lea dx, s1
    mov ah, 9
    int 21h
    
    ; input slag    
    mov ah, 0ah
    mov dx, offset b1
    int 21h
    
    ; print "slag2 = "
    lea dx, s2
    mov ah, 9
    int 21h
    
    ; input slag2    
    mov ah, 0ah
    mov dx, offset b2
    int 21h
    
    ; print "summa: "
    lea dx, s3
    mov ah, 9
    int 21h

    ; ------Calculation phase------    
    ; -----------------------------
    mov dh, 0    ; addition one stored here
    
    lea di, b1   ; b1 stored here
    inc di
    mov bl, [di] ; bl = size of b1
    mov bh, 0
    
    lea si, b2   ; b2 stored here
    inc si
    mov al, [si] ; al = size of b2
    mov ah, 0
    
    ; if second number has more digits
    cmp al, bl
    jle CALC
    ; then we will swap src and dst
    lea di, b2
    inc di
    mov bl, [di] ; bl = size of b2
    mov bh, 0
    
    lea si, b1
    inc si
    mov al, [si] ; al = size of b1
    mov ah, 0
    
    
CALC:
    add di, bx   ; move to last digit
    add si, ax
    
CALC_LOOP:
    ; parse char of b1 to hex
    mov ch, 2h
    mov cl, [si]
    jmp PARSE
DECIMAL:
    sub cl, 30h
    dec ch
    jnz PARSE1_DONE
    jmp PARSE2_DONE
HEX1:
    sub cl, 37h
    dec ch
    jnz PARSE1_DONE
    jmp PARSE2_DONE
HEX2:
    sub cl, 57h
    dec ch
    jnz PARSE1_DONE
    jmp PARSE2_DONE
PARSE:
    cmp cl, 30h ; if less 0 
    jl ERROR 
    cmp cl, 39h ; if less or equal 9
    jle DECIMAL
    cmp cl, 41h ; if less A
    jl ERROR
    cmp cl, 46h ; if less or equal F
    jle HEX1
    cmp cl, 61h ; if less a
    jl ERROR
    cmp cl, 66h ; if less or equal f
    jle HEX2
ERROR:
    lea dx, se
    mov ah, 9
    int 21h
    jmp EXIT
PARSE1_DONE:
    mov [si], cl    
    mov cl, [di]
    jmp PARSE
PARSE2_DONE:
    mov [di], cl
    cmp ax, 0h
    je LAST_CHAR    
    mov dl, [si]
    add cl, dl   ; add slags 
LAST_CHAR:
    add cl, dh   ; add one if exist
    mov dh, 0h
    cmp cl, 0fh  ; additional one?
    jle CALC_RES
    mov dh, 1h
    sub cl, 10h    
CALC_RES:
    mov [di], cl
    dec di
    dec si
    dec bx
    cmp ax, 0h
    je CALC_END
    dec ax
    jnz CALC_LOOP
    cmp dh, 0h
    je CALC_END
    mov ch, 1h
    mov cl, [di]
    cmp bx, 0h
    jne PARSE
    ; print digit
    mov ah, 2
    mov dl, 31h
    int 21h 
CALC_END:    
    
    ; checking the answer
    lea di, b1   ; b1 stored here
    inc di
    mov bl, [di] ; bl = size of b1
    mov bh, 0
    
    lea si, b2   ; b2 stored here
    inc si
    mov al, [si] ; al = size of b2
    mov ah, 0
    
    ; if second number has more digits
    cmp al, bl
    jle PARSE_BACK
    ; then we will swap src and dst
    lea di, b2
    inc di
    mov bl, [di] ; bl = size of b2
    mov bh, 0
    
    lea si, b1
    inc si
    mov al, [si] ; al = size of b1
    mov ah, 0
    
    ; parse hex to char
PARSE_BACK:
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
    mov ah, 2
    mov dl, al
    int 21h 
    inc di 
    dec bl 
    jnz LOOP1
      
EXIT:
    ; exit to operating system.
    mov ax, 4c00h
    int 21h    
ends

end start ; set entry point and stop the assembler.
