[bits 16]
[org 0x7c00]

start:
	mov [BOOT_DRIVE], dl
	mov bx, msg
	call print_string	
	mov bx, msg2
	call print_string
	call load_disk_sections
	jmp enter_protected_mode

load_disk_sections:
	mov bx, 0x1000
	mov dh, 16 ; Load 9 sectors of the disk (stocking kernel.bin) which is the size of kernel.bin divided by 512 (the size of a sector), rounded up
	mov dl, [BOOT_DRIVE] ; First hard drive
	mov ah, 0x02 ; BIOS read sectors function
	mov al, dh ; How many sectors
	mov cl, 0x02 ; reading from sector 2 ..
	mov ch, 0x00 ; .. at the cylinder 0 ..
	mov dh, 0x00 ; .. and at the head 0
	int 0x13
	jc .error_reading
	mov bx, okMsg
	jmp .load_disk_sectors_end
.error_reading:
	mov bx, errMsg
.load_disk_sectors_end:
	call print_string
	ret

msg db 'Hello world!', 0x0d, 0x0a, 0
msg2 db 'Welcome on the first bootloader of Pytom!', 0x0d, 0x0a, 0
okMsg db 'Finished reading disk', 0x0d, 0x0a, 0
errMsg db 'Error during disk reading', 0x0d, 0x0a, 0
db 0

print_string:
.next_char:
	mov al, [bx]
	cmp al, 0
	je .end
	mov ah, 0x0E 
	int 0x10
	add bx, 1
	jmp .next_char
.end:
	ret

gdt_start:
	dq 0x0 ; 64 bits at 0

gdt_code:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00

gdt_data:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00
gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

enter_protected_mode:
	cli ; disable interrupts

	lgdt [gdt_descriptor] ; load gdt with base adress and limit

	mov eax, cr0
	or eax, 1 ; Set PE (bit 0) to 1 to enter in protected mode
	mov cr0, eax

	jmp 0x08:protected_mode_start ; 0x08 is the selector for the code segment

[bits 32]

protected_mode_start:
	mov ax, 0x10 ; 0x10 is the selector for the data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	; mov esp, 0x7C00
	mov ebp, 0x90000
	mov esp, ebp

	; mov al, 'X'
	; mov ah, 3 ; cyan
	; mov edx, 0xb8000
	; mov [edx], ax

	jmp 0x08:0x1000 ; Start address of the kernelEntry.asm 
	; call main
	
	jmp $ ; Infinite loop, never executed normally

BOOT_DRIVE db 0

times 510 - ($ - $$) db 0
dw 0xaa55
