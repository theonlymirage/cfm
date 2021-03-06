;koZeroStringUTF16LE
;Version 0.1: Feb 8, 2018      (�� 08.02.2018)

; Copyright (c) 2017, Efremenkov Sergey aka TheOnlyMirage aka ������������ �����
; All rights reserved.
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright notice, this
;    list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright  notice,
;    this list of conditions and the following disclaimer in the documentation and/or
;    other materials provided with the distribution.
;    * Neither the name of the <organization> nor the names of its contributors may
;    be used to endorse or promote products derived from this software without
;    specific prior written permission.

; THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
; PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
; SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; --------------------------------------------------------------------------------------

;�������������� �������� �� ������ UTF16LE � ASCIIZ-������
;����: esi - ��������� �� UTF16LE ������, edi - ��������� �� ASCIIZ ������
StringUTF16LEtoASCIIZ:
   push eax
   push esi
   push edi
   xor eax, eax ;mov eax, 0
@@:
   mov ax, word[esi]
   cmp ax, 0
   je @f

   mov al, byte[esi+1]
   cmp al, 0
   jne StringUTF16LEtoASCIIZ.error

   mov al, byte[esi]
   mov byte[edi], al

   inc edi
   add esi, 2
   jmp @b
.error:
   call printError
@@:
   mov byte[edi], 0 ;����������� ����
   pop edi
   pop esi
   pop eax
   ret


;�������������� ������ N �������� �� ������ UTF16LE � ASCIIZ-������
;����: esi - ��������� �� UTF16LE ������, edi - ��������� �� ASCIIZ ������, ecx - ���-�� ��������
lengthStringUTF16LEtoASCIIZ:
   push eax
   push ecx
   push esi
   push edi
   xor eax, eax ;mov eax, 0
@@:
   cmp ecx, 0
   jbe @f

   mov ax, word[esi]
   cmp ax, 0
   je @f

   mov al, byte[esi+1]
   cmp al, 0
   jne lengthStringUTF16LEtoASCIIZ.mbKirilic ;.error

   mov al, byte[esi]
.next:
   mov byte[edi], al
   dec ecx

   inc edi
   add esi, 2
   jmp @b
.mbKirilic:
   mov al, byte[esi+1]
   cmp al, 0x04
   jne lengthStringUTF16LEtoASCIIZ.error
   mov al, byte[esi]
   call kirilicToASCII
   jmp lengthStringUTF16LEtoASCIIZ.next
.error:
   call printError
@@:
   mov byte[edi], 0 ;����������� ����
   pop edi
   pop esi
   pop ecx
   pop eax
   ret


;������ ������
;����: esi - ��������� �� ������
reverseStringUTF16LE:
   push eax
   push esi
   push edi

   mov ax, word[esi] ;���� ������ ������ �� ������� ������ ������ �� ����
   cmp ax, 0
   je reverseStringUTF16LE.end

   mov edi, esi  ;����� ����� ������� ���������� ������� ������ � ������� � � edi
.go_last_symbol:
   mov ax, word[edi+2]
   cmp ax, 0
   je @f

   add edi, 2
   jmp reverseStringUTF16LE.go_last_symbol
@@:
   push ebx
.rev:
   cmp esi, edi
   jae @f

   ;������ ������� ������� � �����
   mov ax, word[edi]
   mov bx, word[esi]
   mov word[edi], bx
   mov word[esi], ax

   add esi, 2
   sub edi, 2
   jmp reverseStringUTF16LE.rev
@@:
   pop ebx
.end:
   pop edi
   pop esi
   pop eax
   ret



;�������������� ����� � ������
;����: ecx - �����; esi - ����� ������ ������ (�������� �� ����� 22 ���� ��� ������ ����� ecx)
numberToStringUTF16LE:
   push eax
   push ecx
   push edx
   push esi

   cmp ecx, 0
   jne @f
   mov byte[esi], 48 ;0x30 ; '0'
   mov byte[esi+1], 0
   add esi, 2
   jmp numberToStringUTF16LE.end
@@:
   mov eax, ecx
   mov ecx, 10
.next:
   mov edx, 0
   div ecx
   add edx, 48 ;0x30

   mov byte[esi], dl
   mov byte[esi+1], 0

   add esi, 2
   cmp eax, 0
   je numberToStringUTF16LE.end
   jmp numberToStringUTF16LE.next
.end:
   mov word[esi], 0   ;����� ����������� ����
   pop esi
   pop edx
   pop ecx
   pop eax
   call reverseStringUTF16LE ;������ ������ ������
   ret



;---------------------------------------------------------------
;�������������� ������ � ����� �����
strUTF16LEtoInteger: ;����: esi - ��������� �� ������ � ���������� ������� �����, �����: e�x - �����
   push eax
   push ebx
   push edx
   push edi
   xor eax, eax  ;�������� eax
   mov ecx, 0 ;xor ecx, ecx

   mov edi, esi
.get_last_symbol:
   ;������ ��������� ������
   mov ax, word[edi]

   ;���� ��� ����, �� ������ �����������
   cmp ax, 0
   je @f

   ;����� ��������� �� ��������� ������
   add edi, 2
   jmp strUTF16LEtoInteger.get_last_symbol
@@:   ;����� �� ���������� ������� � ����� ����� �� ���� � edi
   sub edi, 2

   cmp edi, esi    ;����� ���������� ������� ������ ���� ������ ������ ������� �������
   jae @f
   jmp strUTF16LEtoInteger.error  ;����� ������
@@:
   ;������ � edi ��������� ������, � esi ��������� �� ������ ������ (�� ����� ������ ������)

   mov cl, byte[edi]
   sub cl, 48 ;0x30 ;'0'
   ;mov cx, word[edi]  ;����� ��������� ��������� �����
   ;sub cx, 0x3000     ;�������� ��� ����
   ;shr cx, 8
   sub edi, 2	      ;��������� �� ���� ������ �� �����
   mov ebx, 10	      ;��������� �������
.tranform_cicle:
   cmp edi, esi        ;���� ������ ��� �������� ������, �� ����� ������������� - �������
   jb strUTF16LEtoInteger.end

   mov eax, 0
   mov al, byte[edi]
   sub al, 48 ;0x30 ;'0'
   ;mov ax, word[edi]	   ; �������� ��� ������ ����� ����� 0 � 9
   ;sub ax, 0x3000
   ;shr ax, 8
   cmp al, 9 ;0x39	     ;���� ����� ������ 9, �� ������ � ������ ���� ���-�� �� ���
   ja strUTF16LEtoInteger.error
   cmp al, 0 ;0x30	     ;���� ����� ������ 0, �� ������ � ������ ���� ���-�� �� ���
   jb strUTF16LEtoInteger.error
   mov edx, 0
   mul ebx

   add ecx, eax

   sub edi, 2
   mov edx, 0
   mov eax, 10
   mul ebx
   mov ebx, eax
   jmp strUTF16LEtoInteger.tranform_cicle
.error:
   mov ecx, 0
   call printError
.end:

; ��� �������� ������������ ������:
;   push ecx
;   push esi
;   test0_data db 100 dup(0)
;   mov esi, test0_data
;   call numberToStringUTF16LE
;   mov ecx, 100
;   call addStringInHistory
;   pop esi
;   pop ecx

   pop edi
   pop edx
   pop ebx
   pop eax
   ret







;������ ����������������� ����� �� ����� UTF16LE
;����: esi - ������ ����� �����, edi - ����� ����� �����
printBlockStringUTF16LE:    ;������������ help'��
   push eax
   push ecx
   push esi
   push edi
@@:
   cmp esi, edi
   jae @f	  ;���� ������ ����������� �������

   mov ecx, 0
   push esi
.find_end:   ;������� ����� ������ � ����������� � �����
   mov cx, word[esi]
   cmp cx, 0
   je printBlockStringUTF16LE.print
   add esi, 2
   jmp printBlockStringUTF16LE.find_end
.print:
   add esi, 2
   mov ecx, esi
   pop esi
   sub ecx, esi

   push edi
   push esi
   push ecx
   call createStringHistory   ;������� ������
   mov eax, 0
   mov al, 0 ;��� ������� �����
   call addInHistory
   pop ecx
   pop esi
   pop edi

   add esi, ecx
   jmp @b
@@:
   pop edi
   pop esi
   pop ecx
   pop eax
   ret




;--------------------------------------------
;(�) ���������� ������ �., 2018