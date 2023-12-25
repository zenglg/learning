; From: https://www.cnblogs.com/LoyenWang/p/13796537.html

; 1. Build tiny_kernel
; # nasm tiny_kernel.S -o tiny_kernel.bin
; 
; 2. Build tiny_qemu
; # gcc -o tiny_qemu tiny_qemu.c
; 
; 3. Run
; # ./tiny_qemu

start:

; hello
mov     al, 0x48
out     0xf1, al
mov     al, 0x65
out     0xf1, al
mov     al, 0x6c
out     0xf1, al
mov     al, 0x6c
out     0xf1, al
mov     al, 0x6f
out     0xf1, al
mov     al, 0x2c
out     0xf1, al

; world
mov     al, 0x77
out     0xf1, al
mov     al, 0x6f
out     0xf1, al
mov     al, 0x72
out     0xf1, al
mov     al, 0x6c
out     0xf1, al
mov     al, 0x64
out     0xf1, al

mov     al, 0x0a
out     0xf1, al

hlt
