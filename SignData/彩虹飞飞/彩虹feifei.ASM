所有对象修改开关
005DB798 - 89 0D 384DAB00  - mov [00AB4D38],ecx



68 62 02 00 00                                      push    262h            ; ArgList
.text:00555792 8B CE                                               mov     ecx, esi
.text:00555794 E8 4E FD EB FF                                      call    sub_4154E7
.text:00555799 FF 35 D4 88 85 00                                   push    collsior_8588D4
.text:0055579F 8B C8                                               mov     ecx, eax
.text:005557A1 E8 B1 58 F6 FF                                      call    sub_4BB057
.text:005557A6 68 65 02 00 00                                      push    265h            ; ArgList
.text:005557AB 8B CE                                               mov     ecx, esi
.text:005557AD E8 35 FD EB FF                                      call    sub_4154E7
.text:005557B2 8B 0D 94 2E 87 00                                   mov     ecx, dword_872E94
.text:005557B8 FF B1 C4 00 00 00                                   push    dword ptr [ecx+0C4h]
.text:005557BE 8B C8                                               mov     ecx, eax
.text:005557C0 E8 92 58 F6 FF                                      call    sub_4BB057
.text:005557C5 68 66 02 00 00                                      push    266h            ; ArgList
 
 
68 62 02 00 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 68 65 02 00 00