;	# nasm boot.asm -o boot.bin
;	# dd conv=sync if=boot.bin of=floppy bs=1440k count=1
;	# qemu-system-x86_64 floppy

;%define	_BOOT_DEBUG_
%ifdef	_BOOT_DEBUG_
	org 0100h
%else
	org 07c00h
%endif
	mov ax, cs
	mov ds, ax
	mov es, ax
	call DispStr
	jmp $
DispStr:
	mov ax, BootMessage
	mov bp, ax
	mov cx, len
	mov ax, 01301h
	mov bx, 000ch
	mov dl, 0
	mov dh, 1
	int 10h
	ret
BootMessage:       db  "Hello, world!"
len:               equ $-BootMessage
times	510-($-$$) db  0

dw	0xaa55
