section .text
global switch_context

; switch_context(uint32_t* prev_esp, uint32_t next_esp)
; args on stack: [esp+4] = prev_esp ptr, [esp+8] = next_esp value
switch_context:
    ; save callee-saved registers
    push ebx
    push esi
    push edi
    push ebp

    ; save current esp into *prev_esp
    mov eax, [esp+20]       ; prev_esp ptr (past 4 pushes + return addr)
    mov [eax], esp

    ; load next esp
    mov esp, [esp+24]       ; next_esp value (past 4 pushes + return addr + prev_esp)

    ; restore next task's registers
    pop ebp
    pop edi
    pop esi
    pop ebx
    ret