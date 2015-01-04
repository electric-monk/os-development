[BITS 32]

[EXTERN trap]
[GLOBAL vectors]
[GLOBAL trapret]

SECTION .text

%define CONSTANT(name, val)     name equ val
%include "StandardPC_traps_shared.inc"

alltraps:
  ; Build trap frame.
  push ds
  push es
  push fs
  push gs
  pusha
  
  ; Set up data and per-cpu segments.
  mov ax, SEG_KDATA<<3
  mov ds, ax
  mov es, ax
  mov ax, SEG_KCPU<<3
  mov fs, ax
  mov gs, ax

  ; Call trap(tf), where tf=%esp
  push esp
  call trap
  add esp, 0x4 ; pop

  ; Return falls through to trapret...
trapret:
  popa
  pop gs
  pop fs
  pop es
  pop ds
  add esp, 0x8  ; trapno and errcode
  iret

; handlers
vector0:
  push 0
  push 0
  jmp alltraps
vector1:
  push 0
  push 1
  jmp alltraps
vector2:
  push 0
  push 2
  jmp alltraps
vector3:
  push 0
  push 3
  jmp alltraps
vector4:
  push 0
  push 4
  jmp alltraps
vector5:
  push 0
  push 5
  jmp alltraps
vector6:
  push 0
  push 6
  jmp alltraps
vector7:
  push 0
  push 7
  jmp alltraps
vector8:
  push 8
  jmp alltraps
vector9:
  push 0
  push 9
  jmp alltraps
vector10:
  push 10
  jmp alltraps
vector11:
  push 11
  jmp alltraps
vector12:
  push 12
  jmp alltraps
vector13:
  push 13
  jmp alltraps
vector14:
  push 14
  jmp alltraps
vector15:
  push 0
  push 15
  jmp alltraps
vector16:
  push 0
  push 16
  jmp alltraps
vector17:
  push 17
  jmp alltraps
vector18:
  push 0
  push 18
  jmp alltraps
vector19:
  push 0
  push 19
  jmp alltraps
vector20:
  push 0
  push 20
  jmp alltraps
vector21:
  push 0
  push 21
  jmp alltraps
vector22:
  push 0
  push 22
  jmp alltraps
vector23:
  push 0
  push 23
  jmp alltraps
vector24:
  push 0
  push 24
  jmp alltraps
vector25:
  push 0
  push 25
  jmp alltraps
vector26:
  push 0
  push 26
  jmp alltraps
vector27:
  push 0
  push 27
  jmp alltraps
vector28:
  push 0
  push 28
  jmp alltraps
vector29:
  push 0
  push 29
  jmp alltraps
vector30:
  push 0
  push 30
  jmp alltraps
vector31:
  push 0
  push 31
  jmp alltraps
vector32:
  push 0
  push 32
  jmp alltraps
vector33:
  push 0
  push 33
  jmp alltraps
vector34:
  push 0
  push 34
  jmp alltraps
vector35:
  push 0
  push 35
  jmp alltraps
vector36:
  push 0
  push 36
  jmp alltraps
vector37:
  push 0
  push 37
  jmp alltraps
vector38:
  push 0
  push 38
  jmp alltraps
vector39:
  push 0
  push 39
  jmp alltraps
vector40:
  push 0
  push 40
  jmp alltraps
vector41:
  push 0
  push 41
  jmp alltraps
vector42:
  push 0
  push 42
  jmp alltraps
vector43:
  push 0
  push 43
  jmp alltraps
vector44:
  push 0
  push 44
  jmp alltraps
vector45:
  push 0
  push 45
  jmp alltraps
vector46:
  push 0
  push 46
  jmp alltraps
vector47:
  push 0
  push 47
  jmp alltraps
vector48:
  push 0
  push 48
  jmp alltraps
vector49:
  push 0
  push 49
  jmp alltraps
vector50:
  push 0
  push 50
  jmp alltraps
vector51:
  push 0
  push 51
  jmp alltraps
vector52:
  push 0
  push 52
  jmp alltraps
vector53:
  push 0
  push 53
  jmp alltraps
vector54:
  push 0
  push 54
  jmp alltraps
vector55:
  push 0
  push 55
  jmp alltraps
vector56:
  push 0
  push 56
  jmp alltraps
vector57:
  push 0
  push 57
  jmp alltraps
vector58:
  push 0
  push 58
  jmp alltraps
vector59:
  push 0
  push 59
  jmp alltraps
vector60:
  push 0
  push 60
  jmp alltraps
vector61:
  push 0
  push 61
  jmp alltraps
vector62:
  push 0
  push 62
  jmp alltraps
vector63:
  push 0
  push 63
  jmp alltraps
vector64:
  push 0
  push 64
  jmp alltraps
vector65:
  push 0
  push 65
  jmp alltraps
vector66:
  push 0
  push 66
  jmp alltraps
vector67:
  push 0
  push 67
  jmp alltraps
vector68:
  push 0
  push 68
  jmp alltraps
vector69:
  push 0
  push 69
  jmp alltraps
vector70:
  push 0
  push 70
  jmp alltraps
vector71:
  push 0
  push 71
  jmp alltraps
vector72:
  push 0
  push 72
  jmp alltraps
vector73:
  push 0
  push 73
  jmp alltraps
vector74:
  push 0
  push 74
  jmp alltraps
vector75:
  push 0
  push 75
  jmp alltraps
vector76:
  push 0
  push 76
  jmp alltraps
vector77:
  push 0
  push 77
  jmp alltraps
vector78:
  push 0
  push 78
  jmp alltraps
vector79:
  push 0
  push 79
  jmp alltraps
vector80:
  push 0
  push 80
  jmp alltraps
vector81:
  push 0
  push 81
  jmp alltraps
vector82:
  push 0
  push 82
  jmp alltraps
vector83:
  push 0
  push 83
  jmp alltraps
vector84:
  push 0
  push 84
  jmp alltraps
vector85:
  push 0
  push 85
  jmp alltraps
vector86:
  push 0
  push 86
  jmp alltraps
vector87:
  push 0
  push 87
  jmp alltraps
vector88:
  push 0
  push 88
  jmp alltraps
vector89:
  push 0
  push 89
  jmp alltraps
vector90:
  push 0
  push 90
  jmp alltraps
vector91:
  push 0
  push 91
  jmp alltraps
vector92:
  push 0
  push 92
  jmp alltraps
vector93:
  push 0
  push 93
  jmp alltraps
vector94:
  push 0
  push 94
  jmp alltraps
vector95:
  push 0
  push 95
  jmp alltraps
vector96:
  push 0
  push 96
  jmp alltraps
vector97:
  push 0
  push 97
  jmp alltraps
vector98:
  push 0
  push 98
  jmp alltraps
vector99:
  push 0
  push 99
  jmp alltraps
vector100:
  push 0
  push 100
  jmp alltraps
vector101:
  push 0
  push 101
  jmp alltraps
vector102:
  push 0
  push 102
  jmp alltraps
vector103:
  push 0
  push 103
  jmp alltraps
vector104:
  push 0
  push 104
  jmp alltraps
vector105:
  push 0
  push 105
  jmp alltraps
vector106:
  push 0
  push 106
  jmp alltraps
vector107:
  push 0
  push 107
  jmp alltraps
vector108:
  push 0
  push 108
  jmp alltraps
vector109:
  push 0
  push 109
  jmp alltraps
vector110:
  push 0
  push 110
  jmp alltraps
vector111:
  push 0
  push 111
  jmp alltraps
vector112:
  push 0
  push 112
  jmp alltraps
vector113:
  push 0
  push 113
  jmp alltraps
vector114:
  push 0
  push 114
  jmp alltraps
vector115:
  push 0
  push 115
  jmp alltraps
vector116:
  push 0
  push 116
  jmp alltraps
vector117:
  push 0
  push 117
  jmp alltraps
vector118:
  push 0
  push 118
  jmp alltraps
vector119:
  push 0
  push 119
  jmp alltraps
vector120:
  push 0
  push 120
  jmp alltraps
vector121:
  push 0
  push 121
  jmp alltraps
vector122:
  push 0
  push 122
  jmp alltraps
vector123:
  push 0
  push 123
  jmp alltraps
vector124:
  push 0
  push 124
  jmp alltraps
vector125:
  push 0
  push 125
  jmp alltraps
vector126:
  push 0
  push 126
  jmp alltraps
vector127:
  push 0
  push 127
  jmp alltraps
vector128:
  push 0
  push 128
  jmp alltraps
vector129:
  push 0
  push 129
  jmp alltraps
vector130:
  push 0
  push 130
  jmp alltraps
vector131:
  push 0
  push 131
  jmp alltraps
vector132:
  push 0
  push 132
  jmp alltraps
vector133:
  push 0
  push 133
  jmp alltraps
vector134:
  push 0
  push 134
  jmp alltraps
vector135:
  push 0
  push 135
  jmp alltraps
vector136:
  push 0
  push 136
  jmp alltraps
vector137:
  push 0
  push 137
  jmp alltraps
vector138:
  push 0
  push 138
  jmp alltraps
vector139:
  push 0
  push 139
  jmp alltraps
vector140:
  push 0
  push 140
  jmp alltraps
vector141:
  push 0
  push 141
  jmp alltraps
vector142:
  push 0
  push 142
  jmp alltraps
vector143:
  push 0
  push 143
  jmp alltraps
vector144:
  push 0
  push 144
  jmp alltraps
vector145:
  push 0
  push 145
  jmp alltraps
vector146:
  push 0
  push 146
  jmp alltraps
vector147:
  push 0
  push 147
  jmp alltraps
vector148:
  push 0
  push 148
  jmp alltraps
vector149:
  push 0
  push 149
  jmp alltraps
vector150:
  push 0
  push 150
  jmp alltraps
vector151:
  push 0
  push 151
  jmp alltraps
vector152:
  push 0
  push 152
  jmp alltraps
vector153:
  push 0
  push 153
  jmp alltraps
vector154:
  push 0
  push 154
  jmp alltraps
vector155:
  push 0
  push 155
  jmp alltraps
vector156:
  push 0
  push 156
  jmp alltraps
vector157:
  push 0
  push 157
  jmp alltraps
vector158:
  push 0
  push 158
  jmp alltraps
vector159:
  push 0
  push 159
  jmp alltraps
vector160:
  push 0
  push 160
  jmp alltraps
vector161:
  push 0
  push 161
  jmp alltraps
vector162:
  push 0
  push 162
  jmp alltraps
vector163:
  push 0
  push 163
  jmp alltraps
vector164:
  push 0
  push 164
  jmp alltraps
vector165:
  push 0
  push 165
  jmp alltraps
vector166:
  push 0
  push 166
  jmp alltraps
vector167:
  push 0
  push 167
  jmp alltraps
vector168:
  push 0
  push 168
  jmp alltraps
vector169:
  push 0
  push 169
  jmp alltraps
vector170:
  push 0
  push 170
  jmp alltraps
vector171:
  push 0
  push 171
  jmp alltraps
vector172:
  push 0
  push 172
  jmp alltraps
vector173:
  push 0
  push 173
  jmp alltraps
vector174:
  push 0
  push 174
  jmp alltraps
vector175:
  push 0
  push 175
  jmp alltraps
vector176:
  push 0
  push 176
  jmp alltraps
vector177:
  push 0
  push 177
  jmp alltraps
vector178:
  push 0
  push 178
  jmp alltraps
vector179:
  push 0
  push 179
  jmp alltraps
vector180:
  push 0
  push 180
  jmp alltraps
vector181:
  push 0
  push 181
  jmp alltraps
vector182:
  push 0
  push 182
  jmp alltraps
vector183:
  push 0
  push 183
  jmp alltraps
vector184:
  push 0
  push 184
  jmp alltraps
vector185:
  push 0
  push 185
  jmp alltraps
vector186:
  push 0
  push 186
  jmp alltraps
vector187:
  push 0
  push 187
  jmp alltraps
vector188:
  push 0
  push 188
  jmp alltraps
vector189:
  push 0
  push 189
  jmp alltraps
vector190:
  push 0
  push 190
  jmp alltraps
vector191:
  push 0
  push 191
  jmp alltraps
vector192:
  push 0
  push 192
  jmp alltraps
vector193:
  push 0
  push 193
  jmp alltraps
vector194:
  push 0
  push 194
  jmp alltraps
vector195:
  push 0
  push 195
  jmp alltraps
vector196:
  push 0
  push 196
  jmp alltraps
vector197:
  push 0
  push 197
  jmp alltraps
vector198:
  push 0
  push 198
  jmp alltraps
vector199:
  push 0
  push 199
  jmp alltraps
vector200:
  push 0
  push 200
  jmp alltraps
vector201:
  push 0
  push 201
  jmp alltraps
vector202:
  push 0
  push 202
  jmp alltraps
vector203:
  push 0
  push 203
  jmp alltraps
vector204:
  push 0
  push 204
  jmp alltraps
vector205:
  push 0
  push 205
  jmp alltraps
vector206:
  push 0
  push 206
  jmp alltraps
vector207:
  push 0
  push 207
  jmp alltraps
vector208:
  push 0
  push 208
  jmp alltraps
vector209:
  push 0
  push 209
  jmp alltraps
vector210:
  push 0
  push 210
  jmp alltraps
vector211:
  push 0
  push 211
  jmp alltraps
vector212:
  push 0
  push 212
  jmp alltraps
vector213:
  push 0
  push 213
  jmp alltraps
vector214:
  push 0
  push 214
  jmp alltraps
vector215:
  push 0
  push 215
  jmp alltraps
vector216:
  push 0
  push 216
  jmp alltraps
vector217:
  push 0
  push 217
  jmp alltraps
vector218:
  push 0
  push 218
  jmp alltraps
vector219:
  push 0
  push 219
  jmp alltraps
vector220:
  push 0
  push 220
  jmp alltraps
vector221:
  push 0
  push 221
  jmp alltraps
vector222:
  push 0
  push 222
  jmp alltraps
vector223:
  push 0
  push 223
  jmp alltraps
vector224:
  push 0
  push 224
  jmp alltraps
vector225:
  push 0
  push 225
  jmp alltraps
vector226:
  push 0
  push 226
  jmp alltraps
vector227:
  push 0
  push 227
  jmp alltraps
vector228:
  push 0
  push 228
  jmp alltraps
vector229:
  push 0
  push 229
  jmp alltraps
vector230:
  push 0
  push 230
  jmp alltraps
vector231:
  push 0
  push 231
  jmp alltraps
vector232:
  push 0
  push 232
  jmp alltraps
vector233:
  push 0
  push 233
  jmp alltraps
vector234:
  push 0
  push 234
  jmp alltraps
vector235:
  push 0
  push 235
  jmp alltraps
vector236:
  push 0
  push 236
  jmp alltraps
vector237:
  push 0
  push 237
  jmp alltraps
vector238:
  push 0
  push 238
  jmp alltraps
vector239:
  push 0
  push 239
  jmp alltraps
vector240:
  push 0
  push 240
  jmp alltraps
vector241:
  push 0
  push 241
  jmp alltraps
vector242:
  push 0
  push 242
  jmp alltraps
vector243:
  push 0
  push 243
  jmp alltraps
vector244:
  push 0
  push 244
  jmp alltraps
vector245:
  push 0
  push 245
  jmp alltraps
vector246:
  push 0
  push 246
  jmp alltraps
vector247:
  push 0
  push 247
  jmp alltraps
vector248:
  push 0
  push 248
  jmp alltraps
vector249:
  push 0
  push 249
  jmp alltraps
vector250:
  push 0
  push 250
  jmp alltraps
vector251:
  push 0
  push 251
  jmp alltraps
vector252:
  push 0
  push 252
  jmp alltraps
vector253:
  push 0
  push 253
  jmp alltraps
vector254:
  push 0
  push 254
  jmp alltraps
vector255:
  push 0
  push 255
  jmp alltraps

; vector table
SECTION .data
vectors:
  dd vector0
  dd vector1
  dd vector2
  dd vector3
  dd vector4
  dd vector5
  dd vector6
  dd vector7
  dd vector8
  dd vector9
  dd vector10
  dd vector11
  dd vector12
  dd vector13
  dd vector14
  dd vector15
  dd vector16
  dd vector17
  dd vector18
  dd vector19
  dd vector20
  dd vector21
  dd vector22
  dd vector23
  dd vector24
  dd vector25
  dd vector26
  dd vector27
  dd vector28
  dd vector29
  dd vector30
  dd vector31
  dd vector32
  dd vector33
  dd vector34
  dd vector35
  dd vector36
  dd vector37
  dd vector38
  dd vector39
  dd vector40
  dd vector41
  dd vector42
  dd vector43
  dd vector44
  dd vector45
  dd vector46
  dd vector47
  dd vector48
  dd vector49
  dd vector50
  dd vector51
  dd vector52
  dd vector53
  dd vector54
  dd vector55
  dd vector56
  dd vector57
  dd vector58
  dd vector59
  dd vector60
  dd vector61
  dd vector62
  dd vector63
  dd vector64
  dd vector65
  dd vector66
  dd vector67
  dd vector68
  dd vector69
  dd vector70
  dd vector71
  dd vector72
  dd vector73
  dd vector74
  dd vector75
  dd vector76
  dd vector77
  dd vector78
  dd vector79
  dd vector80
  dd vector81
  dd vector82
  dd vector83
  dd vector84
  dd vector85
  dd vector86
  dd vector87
  dd vector88
  dd vector89
  dd vector90
  dd vector91
  dd vector92
  dd vector93
  dd vector94
  dd vector95
  dd vector96
  dd vector97
  dd vector98
  dd vector99
  dd vector100
  dd vector101
  dd vector102
  dd vector103
  dd vector104
  dd vector105
  dd vector106
  dd vector107
  dd vector108
  dd vector109
  dd vector110
  dd vector111
  dd vector112
  dd vector113
  dd vector114
  dd vector115
  dd vector116
  dd vector117
  dd vector118
  dd vector119
  dd vector120
  dd vector121
  dd vector122
  dd vector123
  dd vector124
  dd vector125
  dd vector126
  dd vector127
  dd vector128
  dd vector129
  dd vector130
  dd vector131
  dd vector132
  dd vector133
  dd vector134
  dd vector135
  dd vector136
  dd vector137
  dd vector138
  dd vector139
  dd vector140
  dd vector141
  dd vector142
  dd vector143
  dd vector144
  dd vector145
  dd vector146
  dd vector147
  dd vector148
  dd vector149
  dd vector150
  dd vector151
  dd vector152
  dd vector153
  dd vector154
  dd vector155
  dd vector156
  dd vector157
  dd vector158
  dd vector159
  dd vector160
  dd vector161
  dd vector162
  dd vector163
  dd vector164
  dd vector165
  dd vector166
  dd vector167
  dd vector168
  dd vector169
  dd vector170
  dd vector171
  dd vector172
  dd vector173
  dd vector174
  dd vector175
  dd vector176
  dd vector177
  dd vector178
  dd vector179
  dd vector180
  dd vector181
  dd vector182
  dd vector183
  dd vector184
  dd vector185
  dd vector186
  dd vector187
  dd vector188
  dd vector189
  dd vector190
  dd vector191
  dd vector192
  dd vector193
  dd vector194
  dd vector195
  dd vector196
  dd vector197
  dd vector198
  dd vector199
  dd vector200
  dd vector201
  dd vector202
  dd vector203
  dd vector204
  dd vector205
  dd vector206
  dd vector207
  dd vector208
  dd vector209
  dd vector210
  dd vector211
  dd vector212
  dd vector213
  dd vector214
  dd vector215
  dd vector216
  dd vector217
  dd vector218
  dd vector219
  dd vector220
  dd vector221
  dd vector222
  dd vector223
  dd vector224
  dd vector225
  dd vector226
  dd vector227
  dd vector228
  dd vector229
  dd vector230
  dd vector231
  dd vector232
  dd vector233
  dd vector234
  dd vector235
  dd vector236
  dd vector237
  dd vector238
  dd vector239
  dd vector240
  dd vector241
  dd vector242
  dd vector243
  dd vector244
  dd vector245
  dd vector246
  dd vector247
  dd vector248
  dd vector249
  dd vector250
  dd vector251
  dd vector252
  dd vector253
  dd vector254
  dd vector255
