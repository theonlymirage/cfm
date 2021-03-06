;������ ������ � ������� � ������� (�� ������� koLibrary)
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

koFont:  ;������� ������ ������� ������
   .symbol_width  dd 6; 16   ;db ��������� ��������
   .symbol_height dd 9; 8
   .number	  dd 0 ;��������� ����� �������� ������ (������������� � ����������)
		       ;��������� ������ 1 = cp866   2 = UTF-16LE   3 = UTF-8
		       ;0 = �� ��������� (������������ ���� ��������� � ������ ������)

   ;������������� ������, ��������: eax - ����� ������
   .init:
      mov dword[koFont.number], eax
      call koFont.setSizes
      ret

   ;���������� ������ �������� ������� �������: ������ �� ���������, ������ �� �����
   .setSizes:
      push eax
      mov eax, dword[koFont.number]
      cmp eax, 0
      jne @f
      mov dword[koFont.symbol_width], 6
      mov dword[koFont.symbol_height], 9
      jmp koFont.setSizesFinal
   @@:
      mov dword[koFont.symbol_width], 8
      mov dword[koFont.symbol_height], 16
   .setSizesFinal:
      pop eax
      ret

   ;�������� ������� ������ ������ ������� �������
   ;��������� �� ����: eax - ���-�� �������� � ������
   ;����������: eax - ����� ������ � ��������
   .getTextWidth:
      push edx
      push ebx
      mov ebx, dword[koFont.symbol_width]
      mul ebx
      pop ebx
      pop edx
      ret


   ;�������� ������������ ���-�� ��������, ������� ������ � �������� ������ �������
   ;���������: eax - ������ ������� � ��������
   ;���������: eax - ���-�� �������� (���� ��� �������� ����� ������ �������, �� �� ������� ���-�� �� ���)
   .getSymbolsCount:
      push edx
      push ebx
      mov edx, 0
      mov ebx, dword[koFont.symbol_width]
      cmp ebx, 0
      jbe @f
      div ebx
   @@:
      pop ebx
      pop edx
      ret

    ;�������� ������������ ���-�� ����� ������, ������� ������ �� ������
    ;����: eax - ������ ������� � ��������
    ;�����: eax -  rjkd-dj cnhjr
    .getStringCount:
      push edx
      push ebx
      mov edx, 0
      mov ebx, dword[koFont.symbol_height]
      cmp ebx, 0
      jbe @f
      div ebx
   @@:
      pop ebx
      pop edx
      ret




;���������� ���������� ������ �� ������ ������� ��� ����� �������� ������ ����������
;���������: ebx - ���-�� �������� � ������, edi - ��������� �� koView (����� ������ ��� �������)
;����������: ebx - ����� ���������� ������
setCoordInCenterForView:
   push eax
   push ecx
   push edi
   ;������� X ����������
   mov eax, ebx
   call koFont.getTextWidth		       ;������ � eax - ����� ������ � ��������
   mov ebx, dword[edi+4]		;ebx = ������ koView
   cmp eax, ebx
   jb .coord_x				;���� ������ ������ ������ ������� ������� koView, �� ��������� �������� �� �
   mov eax, 0				;����� ���������� � = 0
   jmp .coord_y
.coord_x:
   sub ebx, eax
   shr ebx, 1
   mov eax, ebx
   shl eax, 16				;������ � eax - ���������� X
.coord_y:
   ;������ ������� Y ����������
   mov ebx, dword[edi+8]		;ebx = ������ koView
   mov ecx, dword[koFont.symbol_height]  ;������� ������ ������
   cmp ebx, ecx 			;������� ������ ������� � ������ ������
   jbe .zero_y				;���� ������ ������ ������ ������ ������� ��� ����� ��, �� ���������� Y = 0
   sub ebx, ecx 			;������ ������� ������ ������
   shr ebx, 1				;�������� �������, ������ � ebx - ���������� Y ������
   jmp .coord_y_end
.zero_y:
   mov ebx, 0
.coord_y_end:				;������ ebx = ���������� ������ Y
   add eax, ebx
   mov ebx, eax 			;ebx = X*65536+Y
   pop edi
   pop ecx
   pop eax
   ret


;--------------------------------------------
;(�) ���������� ������ �., 2018