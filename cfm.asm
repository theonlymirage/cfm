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

format binary as ""
use32
org 0x0    ; ������� ����� ���������� ����, ������ 0x0

; ���������
db 'MENUET01'	  ;���������� �������������
dd 0x01 	  ;������
dd START	  ;����� ����� ������ ���������
dd I_END	  ;����� �����, �� ����� ������ ����� ���������
dd 0x100000	  ;��������� ���-�� ������ ��� �������� ���������
dd 0x7fff0	  ;��������� �������� �������� esp - ����� ����� ������� ����� ��� ��� ���� ������ � ������� ������� ������� 
dd 0, 0 	  ;����� ������ ���������� � ����� ������ ���� ������������ �����
 
; ������ ������� ����, ���������� ����������� ������ ��� "����������" ������:
include './BSI/macros.inc'
include "consoleDisplayMode.inc"

START:
   mcall  68, 11	 ;������������� ����
   mov ebx, 11000000000000000000000000100111b ;������������� �� ���������� ��� �������
   mcall 40
   call consoleMode.init
   call   draw_window	 ;������ ���� ���������
 
; After the window is drawn, it's practical to have the main loop.
; Events are distributed from here.
event_wait:
	mov	eax, 10 		; function 10 : wait until event
	mcall				; event type is returned in eax
 
	cmp	eax, 1		; ��������� � ����������� ?
	je	red			; Expl.: there has been activity on screen and
					; parts of the applications has to be redrawn.
	cmp	eax, 2			; Event key in buffer ?
	je	key			; Expl.: User has pressed a key while the
					; app is at the top of the window stack.
	cmp	eax, 3			; Event button in buffer ?
	je	button			; Expl.: User has pressed one of the applications buttons.

	cmp   eax, 6		;��������� ����������� � ������� ����
	je    mouse
 
	jmp	event_wait
;  The next section reads the event and processes data.
red:					; Redraw event handler
	call	draw_window
	jmp	event_wait
key:					; Keypress event handler
	mov	eax, 2			; The key is returned in ah. The key must be
	mcall				; read and cleared from the system queue.

	cmp eax, 1
	je EmptyBuf    ;����� ����

	;��������� ������� �������
	cmp al, 0
	jne checkHotKey

	;���� ������ �� �������, �� ��������� ������� �������
	call consoleMode.keyboard

	jmp EmptyBuf
checkHotKey:
	cmp al, 2
	jne EmptyBuf
	call consoleMode.hotkey ;��� ��������� ������� �������
EmptyBuf:
	jmp	event_wait		; Just read the key, ignore it and jump to event_wait.
mouse:
	call consoleMode.mouse		  ;37-�� �������
	jmp	event_wait
button: 				; Buttonpress event handler
	mcall 17		  ; The button number defined in window_draw is returned to ah.
	cmp	ah,1			; button id=1 ?
	jne	noclose
	call consoleMode.deinit
	mov	eax,-1			; ������� -1 : ������� ��� ���������
	mcall
noclose:
	jmp	event_wait		; This is for ignored events, useful at development
 

include "koFont.inc"
include "work_with_folder.inc"
include "work_with_app.inc"
include "work_with_clipboard.inc"

setWindowTitle: ;!!! ��� ������: ���������� ����� ��������� ����
   push eax	       ;�� ����������
   push ebx
   push ecx
   push edx

   mov edx, 0
   mov dl, 2 ;dword[koFont.number]
   mcall 71, 2, text_application_title.utf16le

   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

updateWindowSizes:
   push eax
   push ebx
   push ecx
   push edx
   push esi

   mov ebx, dword[windowSettings.mode]
   cmp ebx, 0
   je updateWindowSizes.notFullScreen
   mcall 14
   mov edx, 0
   mov esi, 0
   mov si, ax
   shr eax, 16
   mov dx, ax
   jmp @f
.notFullScreen:
   mov edx, 550
   mov esi, 350
@@:
   mcall 67, 0, 0, edx, esi
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

draw_window:
   mcall 12, 1			   ;������ ����������� ����
   call getWindowParam		   ;��������� �/��� ���������� ���������� ����
;�� ������ ����� ��������� ���������� ��������� ����

    ;������ ���� ���� ���������� � ����������� �� ������ ������
   mov ebx, dword[windowSettings.mode]
   cmp ebx, 0
   je @f
   push eax
   mcall 14
   mov ebx, 0
   mov ecx, 0
   mov cx, ax
   mov dword[windowSettings.height], ecx
   shr eax, 16
   mov bx, ax
   pop eax
   jmp draw_window.nn
@@:
   mov	   ebx, 100 * 65536 + 550  ; [x start] *65536 + [x size]
   mov	   ecx, 100 * 65536 + 350  ; [y start] *65536 + [y size]
   mov dword[windowSettings.height], 350
.nn:
   mov	   edx, dword[applicationSettings.window_background_color]    ; ���� ������� (����������) ������� ���� RRGGBB
				   ; 0x02000000 = window type 4 (fixed size, skinned window)
   mov	   esi, 0x808899ff	   ; color of grab bar	RRGGBB: 0x80000000 = color glide
   mov	   edi, text_application_title.cp866
   mcall 0			   ;������� 0: ���������� � ������ ����

   ;call updateWindowSizes  ;��������� ������� ����

   call consoleMode.draw     ;��������� �������� ������ ������ ������ ����
   mcall 12, 2			   ;���������� ����������� ����
   ret

applicationSettings: ;��������� ��������� ���������
   .window_background_color dd 0x13ffffff  ;0x14ffffff ;�� ��������� �����  (��������� � ��������� ����)


windowSettings:      ;��������� �������� ���� ��������� ���������
   .x	    dd 0     ;������� ��������� �������: ���������� � ������� (������� �� ������ �����)
   .width   dd 0
   .y	    dd 0
   .height  dd 0

   .cx	    dd 0     ;���������� ��� ����������� ���� - ���������� �������
   .cy	    dd 0
   .cwidth  dd 0
   .cheight dd 0

   .background_color	dd 0x13000000  ;0x14ffffff ;�� ��������� �����
   .title_skin_height	dd 0	      ;������ ��������� � ������� ���� ����
   .status  dd 1     ;������-��������� ����
   ; ��� 0 (����� 1): ���� ���������������
   ; ��� 1 (����� 2): ���� �������������� � ������ �����
   ; ��� 2 (����� 4): ���� ������� � ���������
   .mode dd 0 ; 0 -������� ����; 1 - ������������� �����

getWindowParam:  ;��������/��������� ��������� �������� ����
   push eax
   push ebx
   push ecx

   mcall 9, temp_buf_1024, -1  ;������������� ��������� �������, ����� ������� ������ ���������� ������
			       ; -1 �������� ������� ����� � ������� ���� (�������� ����� ��� ������ � ������)
   mov	 eax,[ebx+34]	  ;��������� ������ ����
   mov	 [windowSettings.x],eax
   mov	 eax,[ebx+38]
   mov	 [windowSettings.y],eax
   mov	 eax,[ebx+42]
   mov	 [windowSettings.width],eax
   mov	 eax,[ebx+46]
   mov	 [windowSettings.height],eax

   mov	 eax,[ebx+54]	   ;���������� ������� ����
   mov	 [windowSettings.cx],eax
   mov	 eax,[ebx+58]
   mov	 [windowSettings.cy],eax
   mov	 eax,[ebx+62]
   mov	 [windowSettings.cwidth],eax
   mov	 eax,[ebx+66]
   mov	 [windowSettings.cheight],eax

   mov	 eax,[ebx+70]	   ;��������� ����
   mov	 [windowSettings.status],eax

   mcall 48, 4		   ;������ ����� - ���������
   mov dword[windowSettings.title_skin_height], eax

   pop ecx
   pop ebx
   pop eax
   ret

;  *************   ������� ������   *****************
elements_count_string db 12 dup (0)  ; ��� ������ 256^4 ��������� ������ � 10 + ���� �� ����� - ��� ���� ���� ��� ���������
   include  'text.inc'
I_END:
   ;����� �� 1024 ����� - ������������ �������� ������� ��������� ����, ����� ��������� ����� ����� ����������
   temp_buf_1024:  rb 1024

;--------------------------------------------
;(�) ���������� ������ �., 2018