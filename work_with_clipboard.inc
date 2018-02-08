;Буфер обмена
;Version 0.1: Feb 8, 2018      (от 08.02.2018)

; Copyright (c) 2017, Efremenkov Sergey aka TheOnlyMirage aka Единственный Мираж
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

; THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
; PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
; SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; --------------------------------------------------------------------------------------

slots_count dd 0

Clipboard:
   ;получить кол-во слотов в буфере обмена
   .getSlotsCount:
      mcall 54, 0
;      cmp eax, 0xFFFFFFFFFFFFFFFF   ;-1 это ошибка - отсутствует буфер
;      je @f
;      mov eax, 0
   @@:
      mov dword[slots_count], eax
;;	call testPrintEAX
      ret


   ;отправить в буфер обмена
   .createNewSlot:
      mcall 54, 2, dword[test_current_path2.length], test_current_path2
      ret


   ;удалить последний слот буфера обмена
   .deleteLastSlot:
      mcall 54, 3
      cmp eax, 0
      je @f
      ;eax = 0 - успех, eax = 1 - ошибка, -1 - недоступен буфер
   @@:
      ret


   ;аварийная разблокировка буфера     - НЕ ТЕСТИРОВАЛОСЬ
   .unlockClipboard:
      mcall 54, 4
      cmp eax, 0
      je @f
      ; eax = 0 - успешно, иначе или нет блокировки или буфер не доступен
   @@:
      ret


   .readSlot:
      ret
   .readLastSlot:
       mov     dword[current_slot], 1
       mcall   54, 1, [current_slot]
       ret
;      call Clipboard.getSlotsCount
;      mov ecx, dword[slots_count]
;      cmp ecx, 0
;      je @f
;      dec ecx
;      mov ebx, 1
;      mov eax, 54
;      mcall

	mcall	54,0
	test	eax,eax
	jz	@f   ; no slots of clipboard
	inc	eax
	jz	@f   ; main list area not found
	sub	eax,2
	mov	ecx,eax

	mov ecx, 0
	mov edx, 0
	mov esi, 0
	mov edi, 0

;;	  mov ebx, 1
;;	  mov eax, 54
;;	  int 0x40
       ; mcall	54,1

;      mcall 54, 1, 1
   ;   mcall 54, 4

;      cmp eax, 1	;ошибка
;      je @f

;      cmp eax, -1	;ошибка - отсутствует буфер
;      je @f

;	call testPrintEAX
;      mov eax, dword[eax+4]
   @@:
      ret


clipboard_buffer_struct:
   .length dd 0     ;содержит общую длину данных в контейнере
   .type   dd 0     ;0-текст, 1-блочный текст, остальные нас не интересуют
   .code   dd 0     ;0 = UTF, 1 = 0866,  2 = 1251
   .data   dd 0     ;содержимое

test_current_path:
   .length dd 5;0
   .path   db "/rd/1", 0 ;100 dup(0)


test_current_path2:
   .length dd 18
   .type   dd 0
   .code   dd 1
   .data   db "/rd/1", 0, "=              1          ="

set_test_current_path:

   ret



current_slot: rd 1



;--------------------------------------------
;(с) Ефременков Сергей В., 2018