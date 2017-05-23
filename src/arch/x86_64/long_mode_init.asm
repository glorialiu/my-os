global long_mode_start

extern kmain

extern idt_init

extern irq_c_handler

extern next_proc
extern curr_proc

global irq_gpf_handler
global irq_pf_handler
global irq_df_handler

global irq_handler0
global irq_handler1
global irq_handler2
global irq_handler3
global irq_handler4
global irq_handler5
global irq_handler6
global irq_handler7
global irq_handler8
global irq_handler9
global irq_handler10
global irq_handler11
global irq_handler12
global irq_handler13
global irq_handler14
global irq_handler15
global irq_handler16
global irq_handler17
global irq_handler18
global irq_handler19
global irq_handler20
global irq_handler21
global irq_handler22
global irq_handler23
global irq_handler24
global irq_handler25
global irq_handler26
global irq_handler27
global irq_handler28
global irq_handler29
global irq_handler30
global irq_handler31
global irq_handler32
global irq_handler33
global irq_handler34
global irq_handler35
global irq_handler36
global irq_handler37
global irq_handler38
global irq_handler39
global irq_handler40
global irq_handler41
global irq_handler42
global irq_handler43
global irq_handler44
global irq_handler45
global irq_handler46
global irq_handler47
global irq_handler48
global irq_handler49
global irq_handler50
global irq_handler51
global irq_handler52
global irq_handler53
global irq_handler54
global irq_handler55
global irq_handler56
global irq_handler57
global irq_handler58
global irq_handler59
global irq_handler60
global irq_handler61
global irq_handler62
global irq_handler63
global irq_handler64
global irq_handler65
global irq_handler66
global irq_handler67
global irq_handler68
global irq_handler69
global irq_handler70
global irq_handler71
global irq_handler72
global irq_handler73
global irq_handler74
global irq_handler75
global irq_handler76
global irq_handler77
global irq_handler78
global irq_handler79
global irq_handler80
global irq_handler81
global irq_handler82
global irq_handler83
global irq_handler84
global irq_handler85
global irq_handler86
global irq_handler87
global irq_handler88
global irq_handler89
global irq_handler90
global irq_handler91
global irq_handler92
global irq_handler93
global irq_handler94
global irq_handler95
global irq_handler96
global irq_handler97
global irq_handler98
global irq_handler99
global irq_handler100
global irq_handler101
global irq_handler102
global irq_handler103
global irq_handler104
global irq_handler105
global irq_handler106
global irq_handler107
global irq_handler108
global irq_handler109
global irq_handler110
global irq_handler111
global irq_handler112
global irq_handler113
global irq_handler114
global irq_handler115
global irq_handler116
global irq_handler117
global irq_handler118
global irq_handler119
global irq_handler120
global irq_handler121
global irq_handler122
global irq_handler123
global irq_handler124
global irq_handler125
global irq_handler126
global irq_handler127
global irq_handler128
global irq_handler129
global irq_handler130
global irq_handler131
global irq_handler132
global irq_handler133
global irq_handler134
global irq_handler135
global irq_handler136
global irq_handler137
global irq_handler138
global irq_handler139
global irq_handler140
global irq_handler141
global irq_handler142
global irq_handler143
global irq_handler144
global irq_handler145
global irq_handler146
global irq_handler147
global irq_handler148
global irq_handler149
global irq_handler150
global irq_handler151
global irq_handler152
global irq_handler153
global irq_handler154
global irq_handler155
global irq_handler156
global irq_handler157
global irq_handler158
global irq_handler159
global irq_handler160
global irq_handler161
global irq_handler162
global irq_handler163
global irq_handler164
global irq_handler165
global irq_handler166
global irq_handler167
global irq_handler168
global irq_handler169
global irq_handler170
global irq_handler171
global irq_handler172
global irq_handler173
global irq_handler174
global irq_handler175
global irq_handler176
global irq_handler177
global irq_handler178
global irq_handler179
global irq_handler180
global irq_handler181
global irq_handler182
global irq_handler183
global irq_handler184
global irq_handler185
global irq_handler186
global irq_handler187
global irq_handler188
global irq_handler189
global irq_handler190
global irq_handler191
global irq_handler192
global irq_handler193
global irq_handler194
global irq_handler195
global irq_handler196
global irq_handler197
global irq_handler198
global irq_handler199
global irq_handler200
global irq_handler201
global irq_handler202
global irq_handler203
global irq_handler204
global irq_handler205
global irq_handler206
global irq_handler207
global irq_handler208
global irq_handler209
global irq_handler210
global irq_handler211
global irq_handler212
global irq_handler213
global irq_handler214
global irq_handler215
global irq_handler216
global irq_handler217
global irq_handler218
global irq_handler219
global irq_handler220
global irq_handler221
global irq_handler222
global irq_handler223
global irq_handler224
global irq_handler225
global irq_handler226
global irq_handler227
global irq_handler228
global irq_handler229
global irq_handler230
global irq_handler231
global irq_handler232
global irq_handler233
global irq_handler234
global irq_handler235
global irq_handler236
global irq_handler237
global irq_handler238
global irq_handler239
global irq_handler240
global irq_handler241
global irq_handler242
global irq_handler243
global irq_handler244
global irq_handler245
global irq_handler246
global irq_handler247
global irq_handler248
global irq_handler249
global irq_handler250
global irq_handler251
global irq_handler252
global irq_handler253
global irq_handler254
global irq_handler255

global irq_test_handler

global reloadSegments


extern GDT
extern CS_start

extern tags

section .text
bits 64
long_mode_start:

     ; print `OKAY` to screen, should never get here
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax   

    mov rdi, [tags]

    ; load 0 into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    


    call kmain
    hlt



    hlt



irq_gpf_handler:

    push rdi

    mov rdi, [rsp + 8]
    mov [rsp + 8], rsp

    add rsp, 8

    push rsi

    mov rsi, rdi
    mov rdi, 13

    jmp common_irq_handler_2 ; skip pushing rsi

irq_pf_handler:

    push rdi

    mov rdi, [rsp + 8]
    mov [rsp + 8], rsp

    add rsp, 8

    push rsi

    mov rsi, rdi
    mov rdi, 14

    jmp common_irq_handler_2 ; skip pushing rsi

irq_df_handler:

    push rdi

    mov rdi, [rsp + 8]
    mov [rsp + 8], rsp

    add rsp, 8

    push rsi

    mov rsi, rdi
    mov rdi, 8

    jmp common_irq_handler_2 ; skip pushing rsi


irq_test_handler:
    push rdi
    mov rdi, [rsp + 8]
    mov [rsp + 8], rsp
    add rsp, 8
    push rsi
    mov rsi, rdi
    mov rdi, 50

    jmp common_irq_handler_2 ; skip pushing rsi

common_irq_handler:
    push rsi
common_irq_handler_2:
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    ;push rsp ;dont push the stack pointer (??)
    

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15


    call irq_c_handler

    mov rcx, [next_proc]
    mov rbx, [curr_proc]

    cmp rcx, rbx
    
    je no_swap_needed
    ; else, do context swap

    ; stack related stuff
    mov rax, [rsp + 0x0] ;save r15
    mov [rbx + 0x0], rax
    mov rax, [rsp + 0x08] ;save r14
    mov [rbx + 0x08], rax
    mov rax, [rsp + 0x10] ;save r13
    mov [rbx + 0x10], rax
    mov rax, [rsp + 0x18] ;save r12
    mov [rbx + 0x18], rax
    mov rax, [rsp + 0x20] ;save r11
    mov [rbx + 0x20], rax
    mov rax, [rsp + 0x28] ;save r10
    mov [rbx + 0x28], rax
    mov rax, [rsp + 0x30] ;save r9
    mov [rbx + 0x30], rax
    mov rax, [rsp + 0x38] ;save r8
    mov [rbx + 0x38], rax
    mov rax, [rsp + 0x40] ;save rbp
    mov [rbx + 0x40], rax
    mov rax, [rsp + 0x48] ;save rdx
    mov [rbx + 0x48], rax
    mov rax, [rsp + 0x50] ;save rcx
    mov [rbx + 0x50], rax
    mov rax, [rsp + 0x58] ;save rbx
    mov [rbx + 0x58], rax
    mov rax, [rsp + 0x60] ;save rax
    mov [rbx + 0x60], rax
    mov rax, [rsp + 0x68] ;save rsi
    mov [rbx + 0x68], rax
    mov rax, [rsp + 0x70] ;save rdi
    mov [rbx + 0x70], rax

    ; others
    mov [rbx + 0x88], ds
    mov [rbx + 0x90], es
    mov [rbx + 0x98], fs
    mov [rbx + 0x100], gs

    ;whats left, rsp, rip, flags to save. how? does it matter how i do this?
    ; do this by going back to the stack
    mov rax, [rsp + 0x78] ;save rip
    mov [rbx + 0x110], rax

    mov rax, [rsp + 0x80] ;save cs
    mov [rbx + 0x78], rax

    mov rax, [rsp + 0x88] ;save flags
    mov [rbx + 0x118], rax

    mov rax, [rsp + 0x90] ;save rsp
    mov [rbx + 0x108], rax

    mov rax, [rsp + 0x98] ;save ss
    mov [rbx + 0x80], rax

    
    ; restore hw context
    ; rcx is next_proc, rbx is curr_proc
    ; stack related stuff
    mov rax, [rcx + 0x0] ;load r15
    mov [rsp + 0x0], rax
    mov rax, [rcx + 0x08] ;load r14
    mov [rsp + 0x08], rax
    mov rax, [rcx + 0x10] ;load r13
    mov [rsp + 0x10], rax
    mov rax, [rcx + 0x18] ;load r12
    mov [rsp + 0x18], rax
    mov rax, [rcx + 0x20] ;load r11
    mov [rsp + 0x20], rax
    mov rax, [rcx + 0x28] ;load r10
    mov [rsp + 0x28], rax
    mov rax, [rcx + 0x30] ;load r9
    mov [rsp + 0x30], rax
    mov rax, [rcx + 0x38] ;load r8
    mov [rsp + 0x38], rax
    mov rax, [rcx + 0x40] ;load rbp
    mov [rsp + 0x40], rax
    mov rax, [rcx + 0x48] ;load rdx
    mov [rsp + 0x48], rax
    mov rax, [rcx + 0x50] ;load rcx
    mov [rsp + 0x50], rax
    mov rax, [rcx + 0x58] ;load rbx
    mov [rsp + 0x58], rax
    mov rax, [rcx + 0x60] ;load rax
    mov [rsp + 0x60], rax
    mov rax, [rcx + 0x68] ;load rsi
    mov [rsp + 0x68], rax
    mov rax, [rcx + 0x70] ;load rdi
    mov [rsp + 0x70], rax
   
    ; others
    mov ds, [rcx + 0x88]
    mov es, [rcx + 0x90]
    mov fs, [rcx + 0x98]
    mov gs, [rcx + 0x100]


    mov rax, [rcx + 0x110]; load rip
    mov [rsp + 0x78], rax

    mov rax, [rcx + 0x78] ;load cs
    mov [rsp + 0x80], rax

    mov rax, [rcx + 0x118];load flags
    mov [rsp + 0x88], rax

    mov rax, [rcx + 0x108] ;load rsp
    mov [rsp + 0x90], rax

    mov rax, [rcx + 0x80] ;load ss
    mov [rsp + 0x98], rax
    
    ; set curr proc to next proc
    mov [curr_proc], rcx
    hlt

no_swap_needed:    

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    ;pop rsp
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    pop rsi
    pop rdi

    iretq

irq_handler0:
    push rdi
    mov rdi, 0
    jmp common_irq_handler
irq_handler1:
    push rdi
    mov rdi, 1
    jmp common_irq_handler
irq_handler2:
    push rdi
    mov rdi, 2
    jmp common_irq_handler
irq_handler3:
    push rdi
    mov rdi, 3
    jmp common_irq_handler
irq_handler4:
    push rdi
    mov rdi, 4
    jmp common_irq_handler
irq_handler5:
    push rdi
    mov rdi, 5
    jmp common_irq_handler
irq_handler6:
    push rdi
    mov rdi, 6
    jmp common_irq_handler
irq_handler7:
    push rdi
    mov rdi, 7
    jmp common_irq_handler
irq_handler8:
    push rdi
    mov rdi, 8
    jmp common_irq_handler
irq_handler9:
    push rdi
    mov rdi, 9
    jmp common_irq_handler
irq_handler10:
    push rdi
    mov rdi, 10
    jmp common_irq_handler
irq_handler11:

    push rdi

    mov rdi, [rsp + 8]
    mov [rsp + 8], rsp

    add rsp, 8

    push rsi

    mov rsi, rdi
    mov rdi, 11

    jmp common_irq_handler_2 ; skip pushing rsi

irq_handler12:
    push rdi
    mov rdi, 12
    jmp common_irq_handler
irq_handler13:
    push rdi
    mov rdi, 13
    jmp common_irq_handler
irq_handler14:
    push rdi
    mov rdi, 14
    jmp common_irq_handler
irq_handler15:
    push rdi
    mov rdi, 15
    jmp common_irq_handler
irq_handler16:
    push rdi
    mov rdi, 16
    jmp common_irq_handler
irq_handler17:
    push rdi
    mov rdi, 17
    jmp common_irq_handler
irq_handler18:
    push rdi
    mov rdi, 18
    jmp common_irq_handler
irq_handler19:
    push rdi
    mov rdi, 19
    jmp common_irq_handler
irq_handler20:
    push rdi
    mov rdi, 20
    jmp common_irq_handler
irq_handler21:
    push rdi
    mov rdi, 21
    jmp common_irq_handler
irq_handler22:
    push rdi
    mov rdi, 22
    jmp common_irq_handler
irq_handler23:
    push rdi
    mov rdi, 23
    jmp common_irq_handler
irq_handler24:
    push rdi
    mov rdi, 24
    jmp common_irq_handler
irq_handler25:
    push rdi
    mov rdi, 25
    jmp common_irq_handler
irq_handler26:
    push rdi
    mov rdi, 26
    jmp common_irq_handler
irq_handler27:
    push rdi
    mov rdi, 27
    jmp common_irq_handler
irq_handler28:
    push rdi
    mov rdi, 28
    jmp common_irq_handler
irq_handler29:
    push rdi
    mov rdi, 29
    jmp common_irq_handler
irq_handler30:
    push rdi
    mov rdi, 30
    jmp common_irq_handler
irq_handler31:
    push rdi
    mov rdi, 31
    jmp common_irq_handler
irq_handler32:
    push rdi
    mov rdi, 32
    jmp common_irq_handler
irq_handler33:
    push rdi
    mov rdi, 33
    jmp common_irq_handler
irq_handler34:
    push rdi
    mov rdi, 34
    jmp common_irq_handler
irq_handler35:
    push rdi
    mov rdi, 35
    jmp common_irq_handler
irq_handler36:
    push rdi
    mov rdi, 36
    jmp common_irq_handler
irq_handler37:
    push rdi
    mov rdi, 37
    jmp common_irq_handler
irq_handler38:
    push rdi
    mov rdi, 38
    jmp common_irq_handler
irq_handler39:
    push rdi
    mov rdi, 39
    jmp common_irq_handler
irq_handler40:
    push rdi
    mov rdi, 40
    jmp common_irq_handler
irq_handler41:
    push rdi
    mov rdi, 41
    jmp common_irq_handler
irq_handler42:
    push rdi
    mov rdi, 42
    jmp common_irq_handler
irq_handler43:
    push rdi
    mov rdi, 43
    jmp common_irq_handler
irq_handler44:
    push rdi
    mov rdi, 44
    jmp common_irq_handler
irq_handler45:
    push rdi
    mov rdi, 45
    jmp common_irq_handler
irq_handler46:
    push rdi
    mov rdi, 46
    jmp common_irq_handler
irq_handler47:
    push rdi
    mov rdi, 47
    jmp common_irq_handler
irq_handler48:
    push rdi
    mov rdi, 48
    jmp common_irq_handler
irq_handler49:
    push rdi
    mov rdi, 49
    jmp common_irq_handler
irq_handler50:
    push rdi
    mov rdi, 50
    jmp common_irq_handler
irq_handler51:
    push rdi
    mov rdi, 51
    jmp common_irq_handler
irq_handler52:
    push rdi
    mov rdi, 52
    jmp common_irq_handler
irq_handler53:
    push rdi
    mov rdi, 53
    jmp common_irq_handler
irq_handler54:
    push rdi
    mov rdi, 54
    jmp common_irq_handler
irq_handler55:
    push rdi
    mov rdi, 55
    jmp common_irq_handler
irq_handler56:
    push rdi
    mov rdi, 56
    jmp common_irq_handler
irq_handler57:
    push rdi
    mov rdi, 57
    jmp common_irq_handler
irq_handler58:
    push rdi
    mov rdi, 58
    jmp common_irq_handler
irq_handler59:
    push rdi
    mov rdi, 59
    jmp common_irq_handler
irq_handler60:
    push rdi
    mov rdi, 60
    jmp common_irq_handler
irq_handler61:
    push rdi
    mov rdi, 61
    jmp common_irq_handler
irq_handler62:
    push rdi
    mov rdi, 62
    jmp common_irq_handler
irq_handler63:
    push rdi
    mov rdi, 63
    jmp common_irq_handler
irq_handler64:
    push rdi
    mov rdi, 64
    jmp common_irq_handler
irq_handler65:
    push rdi
    mov rdi, 65
    jmp common_irq_handler
irq_handler66:
    push rdi
    mov rdi, 66
    jmp common_irq_handler
irq_handler67:
    push rdi
    mov rdi, 67
    jmp common_irq_handler
irq_handler68:
    push rdi
    mov rdi, 68
    jmp common_irq_handler
irq_handler69:
    push rdi
    mov rdi, 69
    jmp common_irq_handler
irq_handler70:
    push rdi
    mov rdi, 70
    jmp common_irq_handler
irq_handler71:
    push rdi
    mov rdi, 71
    jmp common_irq_handler
irq_handler72:
    push rdi
    mov rdi, 72
    jmp common_irq_handler
irq_handler73:
    push rdi
    mov rdi, 73
    jmp common_irq_handler
irq_handler74:
    push rdi
    mov rdi, 74
    jmp common_irq_handler
irq_handler75:
    push rdi
    mov rdi, 75
    jmp common_irq_handler
irq_handler76:
    push rdi
    mov rdi, 76
    jmp common_irq_handler
irq_handler77:
    push rdi
    mov rdi, 77
    jmp common_irq_handler
irq_handler78:
    push rdi
    mov rdi, 78
    jmp common_irq_handler
irq_handler79:
    push rdi
    mov rdi, 79
    jmp common_irq_handler
irq_handler80:
    push rdi
    mov rdi, 80
    jmp common_irq_handler
irq_handler81:
    push rdi
    mov rdi, 81
    jmp common_irq_handler
irq_handler82:
    push rdi
    mov rdi, 82
    jmp common_irq_handler
irq_handler83:
    push rdi
    mov rdi, 83
    jmp common_irq_handler
irq_handler84:
    push rdi
    mov rdi, 84
    jmp common_irq_handler
irq_handler85:
    push rdi
    mov rdi, 85
    jmp common_irq_handler
irq_handler86:
    push rdi
    mov rdi, 86
    jmp common_irq_handler
irq_handler87:
    push rdi
    mov rdi, 87
    jmp common_irq_handler
irq_handler88:
    push rdi
    mov rdi, 88
    jmp common_irq_handler
irq_handler89:
    push rdi
    mov rdi, 89
    jmp common_irq_handler
irq_handler90:
    push rdi
    mov rdi, 90
    jmp common_irq_handler
irq_handler91:
    push rdi
    mov rdi, 91
    jmp common_irq_handler
irq_handler92:
    push rdi
    mov rdi, 92
    jmp common_irq_handler
irq_handler93:
    push rdi
    mov rdi, 93
    jmp common_irq_handler
irq_handler94:
    push rdi
    mov rdi, 94
    jmp common_irq_handler
irq_handler95:
    push rdi
    mov rdi, 95
    jmp common_irq_handler
irq_handler96:
    push rdi
    mov rdi, 96
    jmp common_irq_handler
irq_handler97:
    push rdi
    mov rdi, 97
    jmp common_irq_handler
irq_handler98:
    push rdi
    mov rdi, 98
    jmp common_irq_handler
irq_handler99:
    push rdi
    mov rdi, 99
    jmp common_irq_handler
irq_handler100:
    push rdi
    mov rdi, 100
    jmp common_irq_handler
irq_handler101:
    push rdi
    mov rdi, 101
    jmp common_irq_handler
irq_handler102:
    push rdi
    mov rdi, 102
    jmp common_irq_handler
irq_handler103:
    push rdi
    mov rdi, 103
    jmp common_irq_handler
irq_handler104:
    push rdi
    mov rdi, 104
    jmp common_irq_handler
irq_handler105:
    push rdi
    mov rdi, 105
    jmp common_irq_handler
irq_handler106:
    push rdi
    mov rdi, 106
    jmp common_irq_handler
irq_handler107:
    push rdi
    mov rdi, 107
    jmp common_irq_handler
irq_handler108:
    push rdi
    mov rdi, 108
    jmp common_irq_handler
irq_handler109:
    push rdi
    mov rdi, 109
    jmp common_irq_handler
irq_handler110:
    push rdi
    mov rdi, 110
    jmp common_irq_handler
irq_handler111:
    push rdi
    mov rdi, 111
    jmp common_irq_handler
irq_handler112:
    push rdi
    mov rdi, 112
    jmp common_irq_handler
irq_handler113:
    push rdi
    mov rdi, 113
    jmp common_irq_handler
irq_handler114:
    push rdi
    mov rdi, 114
    jmp common_irq_handler
irq_handler115:
    push rdi
    mov rdi, 115
    jmp common_irq_handler
irq_handler116:
    push rdi
    mov rdi, 116
    jmp common_irq_handler
irq_handler117:
    push rdi
    mov rdi, 117
    jmp common_irq_handler
irq_handler118:
    push rdi
    mov rdi, 118
    jmp common_irq_handler
irq_handler119:
    push rdi
    mov rdi, 119
    jmp common_irq_handler
irq_handler120:
    push rdi
    mov rdi, 120
    jmp common_irq_handler
irq_handler121:
    push rdi
    mov rdi, 121
    jmp common_irq_handler
irq_handler122:
    push rdi
    mov rdi, 122
    jmp common_irq_handler
irq_handler123:
    push rdi
    mov rdi, 123
    jmp common_irq_handler
irq_handler124:
    push rdi
    mov rdi, 124
    jmp common_irq_handler
irq_handler125:
    push rdi
    mov rdi, 125
    jmp common_irq_handler
irq_handler126:
    push rdi
    mov rdi, 126
    jmp common_irq_handler
irq_handler127:
    push rdi
    mov rdi, 127
    jmp common_irq_handler
irq_handler128:
    push rdi
    mov rdi, 128
    jmp common_irq_handler
irq_handler129:
    push rdi
    mov rdi, 129
    jmp common_irq_handler
irq_handler130:
    push rdi
    mov rdi, 130
    jmp common_irq_handler
irq_handler131:
    push rdi
    mov rdi, 131
    jmp common_irq_handler
irq_handler132:
    push rdi
    mov rdi, 132
    jmp common_irq_handler
irq_handler133:
    push rdi
    mov rdi, 133
    jmp common_irq_handler
irq_handler134:
    push rdi
    mov rdi, 134
    jmp common_irq_handler
irq_handler135:
    push rdi
    mov rdi, 135
    jmp common_irq_handler
irq_handler136:
    push rdi
    mov rdi, 136
    jmp common_irq_handler
irq_handler137:
    push rdi
    mov rdi, 137
    jmp common_irq_handler
irq_handler138:
    push rdi
    mov rdi, 138
    jmp common_irq_handler
irq_handler139:
    push rdi
    mov rdi, 139
    jmp common_irq_handler
irq_handler140:
    push rdi
    mov rdi, 140
    jmp common_irq_handler
irq_handler141:
    push rdi
    mov rdi, 141
    jmp common_irq_handler
irq_handler142:
    push rdi
    mov rdi, 142
    jmp common_irq_handler
irq_handler143:
    push rdi
    mov rdi, 143
    jmp common_irq_handler
irq_handler144:
    push rdi
    mov rdi, 144
    jmp common_irq_handler
irq_handler145:
    push rdi
    mov rdi, 145
    jmp common_irq_handler
irq_handler146:
    push rdi
    mov rdi, 146
    jmp common_irq_handler
irq_handler147:
    push rdi
    mov rdi, 147
    jmp common_irq_handler
irq_handler148:
    push rdi
    mov rdi, 148
    jmp common_irq_handler
irq_handler149:
    push rdi
    mov rdi, 149
    jmp common_irq_handler
irq_handler150:
    push rdi
    mov rdi, 150
    jmp common_irq_handler
irq_handler151:
    push rdi
    mov rdi, 151
    jmp common_irq_handler
irq_handler152:
    push rdi
    mov rdi, 152
    jmp common_irq_handler
irq_handler153:
    push rdi
    mov rdi, 153
    jmp common_irq_handler
irq_handler154:
    push rdi
    mov rdi, 154
    jmp common_irq_handler
irq_handler155:
    push rdi
    mov rdi, 155
    jmp common_irq_handler
irq_handler156:
    push rdi
    mov rdi, 156
    jmp common_irq_handler
irq_handler157:
    push rdi
    mov rdi, 157
    jmp common_irq_handler
irq_handler158:
    push rdi
    mov rdi, 158
    jmp common_irq_handler
irq_handler159:
    push rdi
    mov rdi, 159
    jmp common_irq_handler
irq_handler160:
    push rdi
    mov rdi, 160
    jmp common_irq_handler
irq_handler161:
    push rdi
    mov rdi, 161
    jmp common_irq_handler
irq_handler162:
    push rdi
    mov rdi, 162
    jmp common_irq_handler
irq_handler163:
    push rdi
    mov rdi, 163
    jmp common_irq_handler
irq_handler164:
    push rdi
    mov rdi, 164
    jmp common_irq_handler
irq_handler165:
    push rdi
    mov rdi, 165
    jmp common_irq_handler
irq_handler166:
    push rdi
    mov rdi, 166
    jmp common_irq_handler
irq_handler167:
    push rdi
    mov rdi, 167
    jmp common_irq_handler
irq_handler168:
    push rdi
    mov rdi, 168
    jmp common_irq_handler
irq_handler169:
    push rdi
    mov rdi, 169
    jmp common_irq_handler
irq_handler170:
    push rdi
    mov rdi, 170
    jmp common_irq_handler
irq_handler171:
    push rdi
    mov rdi, 171
    jmp common_irq_handler
irq_handler172:
    push rdi
    mov rdi, 172
    jmp common_irq_handler
irq_handler173:
    push rdi
    mov rdi, 173
    jmp common_irq_handler
irq_handler174:
    push rdi
    mov rdi, 174
    jmp common_irq_handler
irq_handler175:
    push rdi
    mov rdi, 175
    jmp common_irq_handler
irq_handler176:
    push rdi
    mov rdi, 176
    jmp common_irq_handler
irq_handler177:
    push rdi
    mov rdi, 177
    jmp common_irq_handler
irq_handler178:
    push rdi
    mov rdi, 178
    jmp common_irq_handler
irq_handler179:
    push rdi
    mov rdi, 179
    jmp common_irq_handler
irq_handler180:
    push rdi
    mov rdi, 180
    jmp common_irq_handler
irq_handler181:
    push rdi
    mov rdi, 181
    jmp common_irq_handler
irq_handler182:
    push rdi
    mov rdi, 182
    jmp common_irq_handler
irq_handler183:
    push rdi
    mov rdi, 183
    jmp common_irq_handler
irq_handler184:
    push rdi
    mov rdi, 184
    jmp common_irq_handler
irq_handler185:
    push rdi
    mov rdi, 185
    jmp common_irq_handler
irq_handler186:
    push rdi
    mov rdi, 186
    jmp common_irq_handler
irq_handler187:
    push rdi
    mov rdi, 187
    jmp common_irq_handler
irq_handler188:
    push rdi
    mov rdi, 188
    jmp common_irq_handler
irq_handler189:
    push rdi
    mov rdi, 189
    jmp common_irq_handler
irq_handler190:
    push rdi
    mov rdi, 190
    jmp common_irq_handler
irq_handler191:
    push rdi
    mov rdi, 191
    jmp common_irq_handler
irq_handler192:
    push rdi
    mov rdi, 192
    jmp common_irq_handler
irq_handler193:
    push rdi
    mov rdi, 193
    jmp common_irq_handler
irq_handler194:
    push rdi
    mov rdi, 194
    jmp common_irq_handler
irq_handler195:
    push rdi
    mov rdi, 195
    jmp common_irq_handler
irq_handler196:
    push rdi
    mov rdi, 196
    jmp common_irq_handler
irq_handler197:
    push rdi
    mov rdi, 197
    jmp common_irq_handler
irq_handler198:
    push rdi
    mov rdi, 198
    jmp common_irq_handler
irq_handler199:
    push rdi
    mov rdi, 199
    jmp common_irq_handler
irq_handler200:
    push rdi
    mov rdi, 200
    jmp common_irq_handler
irq_handler201:
    push rdi
    mov rdi, 201
    jmp common_irq_handler
irq_handler202:
    push rdi
    mov rdi, 202
    jmp common_irq_handler
irq_handler203:
    push rdi
    mov rdi, 203
    jmp common_irq_handler
irq_handler204:
    push rdi
    mov rdi, 204
    jmp common_irq_handler
irq_handler205:
    push rdi
    mov rdi, 205
    jmp common_irq_handler
irq_handler206:
    push rdi
    mov rdi, 206
    jmp common_irq_handler
irq_handler207:
    push rdi
    mov rdi, 207
    jmp common_irq_handler
irq_handler208:
    push rdi
    mov rdi, 208
    jmp common_irq_handler
irq_handler209:
    push rdi
    mov rdi, 209
    jmp common_irq_handler
irq_handler210:
    push rdi
    mov rdi, 210
    jmp common_irq_handler
irq_handler211:
    push rdi
    mov rdi, 211
    jmp common_irq_handler
irq_handler212:
    push rdi
    mov rdi, 212
    jmp common_irq_handler
irq_handler213:
    push rdi
    mov rdi, 213
    jmp common_irq_handler
irq_handler214:
    push rdi
    mov rdi, 214
    jmp common_irq_handler
irq_handler215:
    push rdi
    mov rdi, 215
    jmp common_irq_handler
irq_handler216:
    push rdi
    mov rdi, 216
    jmp common_irq_handler
irq_handler217:
    push rdi
    mov rdi, 217
    jmp common_irq_handler
irq_handler218:
    push rdi
    mov rdi, 218
    jmp common_irq_handler
irq_handler219:
    push rdi
    mov rdi, 219
    jmp common_irq_handler
irq_handler220:
    push rdi
    mov rdi, 220
    jmp common_irq_handler
irq_handler221:
    push rdi
    mov rdi, 221
    jmp common_irq_handler
irq_handler222:
    push rdi
    mov rdi, 222
    jmp common_irq_handler
irq_handler223:
    push rdi
    mov rdi, 223
    jmp common_irq_handler
irq_handler224:
    push rdi
    mov rdi, 224
    jmp common_irq_handler
irq_handler225:
    push rdi
    mov rdi, 225
    jmp common_irq_handler
irq_handler226:
    push rdi
    mov rdi, 226
    jmp common_irq_handler
irq_handler227:
    push rdi
    mov rdi, 227
    jmp common_irq_handler
irq_handler228:
    push rdi
    mov rdi, 228
    jmp common_irq_handler
irq_handler229:
    push rdi
    mov rdi, 229
    jmp common_irq_handler
irq_handler230:
    push rdi
    mov rdi, 230
    jmp common_irq_handler
irq_handler231:
    push rdi
    mov rdi, 231
    jmp common_irq_handler
irq_handler232:
    push rdi
    mov rdi, 232
    jmp common_irq_handler
irq_handler233:
    push rdi
    mov rdi, 233
    jmp common_irq_handler
irq_handler234:
    push rdi
    mov rdi, 234
    jmp common_irq_handler
irq_handler235:
    push rdi
    mov rdi, 235
    jmp common_irq_handler
irq_handler236:
    push rdi
    mov rdi, 236
    jmp common_irq_handler
irq_handler237:
    push rdi
    mov rdi, 237
    jmp common_irq_handler
irq_handler238:
    push rdi
    mov rdi, 238
    jmp common_irq_handler
irq_handler239:
    push rdi
    mov rdi, 239
    jmp common_irq_handler
irq_handler240:
    push rdi
    mov rdi, 240
    jmp common_irq_handler
irq_handler241:
    push rdi
    mov rdi, 241
    jmp common_irq_handler
irq_handler242:
    push rdi
    mov rdi, 242
    jmp common_irq_handler
irq_handler243:
    push rdi
    mov rdi, 243
    jmp common_irq_handler
irq_handler244:
    push rdi
    mov rdi, 244
    jmp common_irq_handler
irq_handler245:
    push rdi
    mov rdi, 245
    jmp common_irq_handler
irq_handler246:
    push rdi
    mov rdi, 246
    jmp common_irq_handler
irq_handler247:
    push rdi
    mov rdi, 247
    jmp common_irq_handler
irq_handler248:
    push rdi
    mov rdi, 248
    jmp common_irq_handler
irq_handler249:
    push rdi
    mov rdi, 249
    jmp common_irq_handler
irq_handler250:
    push rdi
    mov rdi, 250
    jmp common_irq_handler
irq_handler251:
    push rdi
    mov rdi, 251
    jmp common_irq_handler
irq_handler252:
    push rdi
    mov rdi, 252
    jmp common_irq_handler
irq_handler253:
    push rdi
    mov rdi, 253
    jmp common_irq_handler
irq_handler254:
    push rdi
    mov rdi, 254
    jmp common_irq_handler
irq_handler255:
    push rdi
    mov rdi, 255
    jmp common_irq_handler
