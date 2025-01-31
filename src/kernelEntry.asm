[bits 32]
[extern main]

call main

jmp $ ; Infinite loop

global ioPortOut
global ioPortIn

ioPortOut:
	mov dx, word [esp+4]
	mov al, byte [esp+8]
	out dx, al
	ret

ioPortIn:
	mov dx, word [esp+4]
	in al, dx
	ret
