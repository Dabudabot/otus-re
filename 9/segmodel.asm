use16
CURRENT_OFFSET EQU 0x7c00
org CURRENT_OFFSET
NEW_SEG EQU 0x1000
start:  
    mov ax, 0xffff
    mov sp, ax

    mov ax,3
    int 10h
        cld
        xor ax, ax

        mov ds, ax
        mov si, CURRENT_OFFSET
        
        mov di, ax      
        mov ax, NEW_SEG
        mov es, ax
        
        mov cx, 0x200
        mov bx, cx
        rep movsb

        mov ah, 2
        mov al, 1
        mov dx, 0
        mov cx, 2
        int 13h

        push NEW_SEG
        push NEW_OFFSET-CURRENT_OFFSET
        retf
        
NEW_OFFSET:
        in al, 0x92
        or al, 2
        out 0x92, al
		
        cli
        mov al, 0x8F
        out 0x70, al
        in al, 0x71

xchg bx, bx
        mov eax, NEW_SEG
        shl eax, 4                      ;phisical addres = segmeng*0x10 + offset
        add eax, GDT-CURRENT_OFFSET
        mov [(GDTR-CURRENT_OFFSET) + 2], eax             ;phisical address of GDT table

        lgdt fword [GDTR-CURRENT_OFFSET]
        mov eax, cr0 
        or al, 1
        mov cr0, eax

xchg bx, bx
        jmp fword 8: pm_Main-CURRENT_OFFSET
        
align 8
GDT:
        ;NULL - descriptor 0
        dq 0    

		;code - descriptor 1, selector 1<<3 = 8
        dw 0x0fff ;size
        ;0x1000:0x0 = 0x1000*0x10+0 = 0x0001 0000
        dw 0x0000   ;low
        dw 0x9a01
        dw 0x0040

        ;data - descriptor 2, selector 2<<3 = 0x10
        dw 0xffff ;size
        ;0x1000:0x0 = 0x1000*0x10+0 = 0x0001 0000
        dw 0x0000   ;low
        dw 0x9210
        dw 0x00cf

        ;video - descriptor 3, selector 3<<3 = 0x18
        dw 0xffff ;size
        ;0xB800 - segment of video => 0x000b 8000 - lenear address
        dw 0x8000   ;low
        dw 0x920b
        dw 0x0040

GDTR:
        dw (GDTR-CURRENT_OFFSET - GDT - 1)
        dd ?

db 510-($-start) dup(0x90), 0x55, 0xaa

use32
pm_Main:
        mov ax, 0x10
        mov ds, ax
        mov fs, ax
        mov ss, ax
        mov esp, -1
        mov ax, 18h
        mov gs, ax

push 0x11223344
pop ebx

        mov esi, Hello-CURRENT_OFFSET
        call print
        hlt
        jmp $
        
print: 
        pushad  
        mov ah, 7
        xor ebx, ebx
        xchg ebx, ebx
puts:
        mov al, byte [cs:esi+ebx]
        mov [gs:(ebx*2)], ax
        inc ebx 
        test al, al
        jnz puts
        popad
        hlt
        ret

Hello db 'pm_Main', 0
db 510-($-pm_Main) dup(0), 0x55, 0xaa