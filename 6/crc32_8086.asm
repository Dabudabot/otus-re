#make_boot#

org 7c00h

; Code

START:

    push bp         ; prolog
    mov bp, sp
    
    lea si, buf     ; get data
    lea di, hash
    mov dx, [size]
    
    push di         ; pass agrs
    push dx         ; in rev order
    push si         ; cause stdcall

    call CRC32PROC
    
    mov sp, bp      ; epilog
    pop bp
    
    
    INT 19h         ; reboot

CRC32PROC:; CRC32PROC(int* buf, int size, int* hash)
    push bp             ; prolog
    mov bp, sp
    
    push 0ffffh         ; local var
    push 0ffffh         ; for calculated
    
    mov si, [bp+4]      ; buf
    mov dx, [bp+6]      ; size
    
    CRC32PROC_LOOP:
        
        ; find index & take element
        mov ax, [bp-4]
        mov bx, [si]
        mov bh, 0
        xor bx, ax      ; crc ^ *p
        mov al, 4
        mul bl          ; cause one element 8 byte
        lea di, crc32
        add di, ax
        
        ; crc >> 8
        mov ax, [bp-2]
        mov bx, ax
        shr ax, 8
        push ax
        mov ax, [bp-4]
        shr ax, 8
        mov ah, bl
        push ax
        
        ; crc = element ^ shifted
        
        mov ax, [di]
        pop bx
        xor ax, bx
        mov [bp-4], ax
        inc di
        inc di
        mov ax, [di]
        pop bx
        xor ax, bx
        mov [bp-2], ax
        
        ; next
        
        inc si
        dec dx
        jnz CRC32PROC_LOOP
    
    ; crc ^ 0xFFFFFFFF;
    xor [bp-2], 0ffffh
    xor [bp-4], 0ffffh
    
    ; save to mem
    mov di, [bp+8]
    mov ax, [bp-4]
    mov [di], ax
    inc di
    inc di
    mov ax, [bp-2]
    mov [di], ax
    
    mov sp, bp
    pop bp
    ret 6                    ; epilog


; Data

crc32 dd 00000000h, 77073096h, ee0e612ch, 990951bah, 076dc419h, 706af48fh
dd e963a535h, 9e6495a3h, 0edb8832h, 79dcb8a4h, e0d5e91eh, 97d2d988h
dd 09b64c2bh, 7eb17cbdh, e7b82d07h, 90bf1d91h, 1db71064h, 6ab020f2h
dd f3b97148h, 84be41deh, 1adad47dh, 6ddde4ebh, f4d4b551h, 83d385c7h
dd 136c9856h, 646ba8c0h, fd62f97ah, 8a65c9ech, 14015c4fh, 63066cd9h
dd fa0f3d63h, 8d080df5h, 3b6e20c8h, 4c69105eh, d56041e4h, a2677172h
dd 3c03e4d1h, 4b04d447h, d20d85fdh, a50ab56bh, 35b5a8fah, 42b2986ch
dd dbbbc9d6h, acbcf940h, 32d86ce3h, 45df5c75h, dcd60dcfh, abd13d59h
dd 26d930ach, 51de003ah, c8d75180h, bfd06116h, 21b4f4b5h, 56b3c423h
dd cfba9599h, b8bda50fh, 2802b89eh, 5f058808h, c60cd9b2h, b10be924h
dd 2f6f7c87h, 58684c11h, c1611dabh, b6662d3dh, 76dc4190h, 01db7106h
dd 98d220bch, efd5102ah, 71b18589h, 06b6b51fh, 9fbfe4a5h, e8b8d433h
dd 7807c9a2h, 0f00f934h, 9609a88eh, e10e9818h, 7f6a0dbbh, 086d3d2dh
dd 91646c97h, e6635c01h, 6b6b51f4h, 1c6c6162h, 856530d8h, f262004eh
dd 6c0695edh, 1b01a57bh, 8208f4c1h, f50fc457h, 65b0d9c6h, 12b7e950h
dd 8bbeb8eah, fcb9887ch, 62dd1ddfh, 15da2d49h, 8cd37cf3h, fbd44c65h
dd 4db26158h, 3ab551ceh, a3bc0074h, d4bb30e2h, 4adfa541h, 3dd895d7h
dd a4d1c46dh, d3d6f4fbh, 4369e96ah, 346ed9fch, ad678846h, da60b8d0h
dd 44042d73h, 33031de5h, aa0a4c5fh, dd0d7cc9h, 5005713ch, 270241aah
dd be0b1010h, c90c2086h, 5768b525h, 206f85b3h, b966d409h, ce61e49fh
dd 5edef90eh, 29d9c998h, b0d09822h, c7d7a8b4h, 59b33d17h, 2eb40d81h
dd b7bd5c3bh, c0ba6cadh, edb88320h, 9abfb3b6h, 03b6e20ch, 74b1d29ah
dd ead54739h, 9dd277afh, 04db2615h, 73dc1683h, e3630b12h, 94643b84h
dd 0d6d6a3eh, 7a6a5aa8h, e40ecf0bh, 9309ff9dh, 0a00ae27h, 7d079eb1h
dd f00f9344h, 8708a3d2h, 1e01f268h, 6906c2feh, f762575dh, 806567cbh
dd 196c3671h, 6e6b06e7h, fed41b76h, 89d32be0h, 10da7a5ah, 67dd4acch
dd f9b9df6fh, 8ebeeff9h, 17b7be43h, 60b08ed5h, d6d6a3e8h, a1d1937eh
dd 38d8c2c4h, 4fdff252h, d1bb67f1h, a6bc5767h, 3fb506ddh, 48b2364bh
dd d80d2bdah, af0a1b4ch, 36034af6h, 41047a60h, df60efc3h, a867df55h
dd 316e8eefh, 4669be79h, cb61b38ch, bc66831ah, 256fd2a0h, 5268e236h
dd cc0c7795h, bb0b4703h, 220216b9h, 5505262fh, c5ba3bbeh, b2bd0b28h
dd 2bb45a92h, 5cb36a04h, c2d7ffa7h, b5d0cf31h, 2cd99e8bh, 5bdeae1dh
dd 9b64c2b0h, ec63f226h, 756aa39ch, 026d930ah, 9c0906a9h, eb0e363fh
dd 72076785h, 05005713h, 95bf4a82h, e2b87a14h, 7bb12baeh, 0cb61b38h
dd 92d28e9bh, e5d5be0dh, 7cdcefb7h, 0bdbdf21h, 86d3d2d4h, f1d4e242h
dd 68ddb3f8h, 1fda836eh, 81be16cdh, f6b9265bh, 6fb077e1h, 18b74777h
dd 88085ae6h, ff0f6a70h, 66063bcah, 11010b5ch, 8f659effh, f862ae69h
dd 616bffd3h, 166ccf45h, a00ae278h, d70dd2eeh, 4e048354h, 3903b3c2h
dd a7672661h, d06016f7h, 4969474dh, 3e6e77dbh, aed16a4ah, d9d65adch
dd 40df0b66h, 37d83bf0h, a9bcae53h, debb9ec5h, 47b2cf7fh, 30b5ffe9h
dd bdbdf21ch, cabac28ah, 53b39330h, 24b4a3a6h, bad03605h, cdd70693h
dd 54de5729h, 23d967bfh, b3667a2eh, c4614ab8h, 5d681b02h, 2a6f2b94h
dd b40bbe37h, c30c8ea1h, 5a05df1bh, 2d02ef8dh

hash dd 0ffffffffh
buf db "326XP5i1WFhbI214SLYzx9BWar07ChDwHJjNYQklAQvzTQSWES19x32XdJ5Nfg27OrTinoWMKc26HT4QKpFbzAufpFBb7T2B8V6PP407I02sL6wMo9yPLmWcW4JjZEXYbg1YEOreaEWPi9daAdYG0eRnRTafUBG20KKctwP9g3hqYyZjWycOdXlg88m3fy7uxQvfYhgKbUpWFlmav8clxu9eCvobkcxVB08UBaPCiQkgrQZKXpjk5adZfOa7u9S7"
size dd 256


INT 19h        ; reboot