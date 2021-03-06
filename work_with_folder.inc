;������ � ������
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

;�������� ������� ������:
; - ������ ���������
;  readBlock	      - ������ ���� ����: eax - ����� �����, ����������: � ����������
;������ �������
;  setCurrentEncoding - ���������� ������� ��������� �� ������ ������ ������ koFont
;  setRootPath	      - ���������� �������� ����


;���������� ������� ��������� (� ������� ����� �������� �����)
setCurrentEncoding:
   push eax
   mov eax, dword[koFont.number]
   mov dword[read_folder_struct.encoding], eax
   mov byte[current_dir_encoding], al
   pop eax
   ret


;���������� �������� ����
setRootPath:
   push eax
   mov eax, dword[read_folder_struct.encoding]
   cmp eax, 1
   jne @f
.zero:
   mov byte[read_folder_struct.name], '/'      ;��� cp866
   mov byte[read_folder_struct.name + 1], 0
   jmp .finish
@@:
   cmp eax, 2
   jne .zero
   mov byte[read_folder_struct.name], 0      ;��� utf
   mov dword[read_folder_struct.name + 1], root_path.utf16le
.finish:
   pop eax
   ret


;���������� ����: esi - ��������� �� ������ ����
setReadPath:
   mov dword[read_folder_struct.path_adr], esi
   ret


;��������� ����: � eax - ����� ������� ����� ;, esi - ������ ������ � ����
readBlock:
   push eax
   push ebx
   mov dword[read_folder_struct.count], 1 ;������������� ���-�� �������� ������ � �������
   mov dword[read_folder_struct.start], eax ;������������� ����� �����
;{ ! 17 ���
;mov dword[read_folder_struct.path_adr], root_path_cp_17052017 ;esi
;}
   mcall 70, read_folder_struct  ;������ ����

   test  eax,eax
   jz	 @f

   ;����� ������ - ��� ��������� �� ����� ����������� �� ������ ��
   jmp .finish

@@:
  cmp ebx, 1	 ;�� ��������� ���� ����, ���� �� ���������� �� ����
  jne .finish

  ;��������� ����� ����� ������ � �����
  ;call updateElementsCountInFolder

  mov ebx, read_folder_return_struct.header ;ebx
  add ebx, 8
  mov eax, dword[ebx]
  ;!�� 17 ��� 2017
;!  mov edi, elements_count_string
;!  call dword_to_STR

;!   mov     ebx, (190+8*10) * 65536 + 5
;!   mov     ecx, 0xFF0000
;!   mov     edx, read_folder_return_struct.block + 40;elements_count_string
;!   mov     esi, 19
;!   mov     eax, 4
;!   mcall

.finish:
   pop ebx
   pop eax
   ret


align 4
root_path:
   .cp866   db "/", 0
   .utf16le db 2, '/', 0, 0,0 ;'c', 0, 'd', 0,	'2', 0,  '/', 0,  '1', 0,  '/', 0, 0, 0 ;2, '/cd2/1/', 0, 0, 0 ;'/', 0, 0, 0 ;
read_folder_struct:
   .subfunction dd  1
   .start	dd  0 ;� ������ ����� �������� ������, ��������� � 0
   .encoding	dd  0 ;1-cp866, 2-UTF-16LE, 3-utf8
   .count	dd  1 ;���-�� �������� ������
   .return	dd  read_folder_return_struct ;����� ������ ��� ����������� ����� ���� � ����������
   .name	db  0
   .path_adr	dd  path ;0
   ;.name db   1000 dup(0)
   ;��� db 0
   ;	.path dd 0
path db 1024 dup(0)

read_folder_return_struct:
   .header db 32 dup (0)
   .block  db 560 dup (0)   ;���� ���� �������� ��� � cp866, �� ����� ���� ���������� 304 �����, ����� - 560 ����.



;������ � ������� ������ ����������
current_dir_encoding db 2
current_dir db 2048 dup(0)
initCurrentDir:
  call getCurrentDir
  ret
setCurrentDir: ;ecx - ��������� �� ������ � ����������� � ������� ���������
   push eax
   push ebx
   push ecx
   push edx
   cmp ecx, 0
   jne @f
   mov ecx, current_dir
@@:
   mcall 30, 4, ecx, dword[koFont.number]  ;mcall 30, 1, current_dir
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret
getCurrentDir:
   push eax
   push ebx
   push ecx
   push edx
   push esi
   mov esi, dword[koFont.number]
   mcall 30, 5, current_dir, 2048 ;mcall 30, 2, current_dir, 2048
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret


;-----------------------------------
;������� ��������� ���������
;���������

;�����
FileParser:
   .getSizeInByte: ;������ ����� � ������  �������� � eax
      ;push eax
      mov eax, dword[read_folder_return_struct.block + 32]
      ;pop eax
      ret





;-----------------------------------
;�������� � �������� �����:

;��������� ��� ������ � �������
folder_work:
  .func_number dd 0 ;8=��������, 9=��������
  dq 0
  dq 0
  db 0
  .name: dd 0 ;folder_name
;folder_name db 2048 dup(0)


;������� ������ ����� (�� ������ ������� ������� ������ - ����� ������)
;����: esi - ����� ������ � ����, eax - ����� �������
function_folder:
   push eax
   push ebx
   push esi
   mov dword[folder_work], eax
   mov dword[folder_work.name], esi
   mcall 70, folder_work
   cmp eax, 0
   je @f
   call printError
 @@:
   pop esi
   pop ebx
   pop eax
   ret

function_folder_mkdir: ;������� �����: esi - ����� ������ � ����
   push eax
   mov eax, 9
   call function_folder
   pop eax
   ret

function_folder_rm: ;������� �����: esi - ����� ������ � ����
   push eax
   mov eax, 8
   call function_folder
   pop eax
   ret













;--------------------------------------------
;(�) ���������� ������ �., 2018