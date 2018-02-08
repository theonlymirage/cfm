;Модуль работы с текстом и шрифтом (из состава koLibrary)
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

koFont:  ;средний размер символа шрифта
   .symbol_width  dd 6; 16   ;db оказалось неудобно
   .symbol_height dd 9; 8
   .number	  dd 0 ;системный номер текущего шрифта (используемого в приложении)
		       ;кодировка строки 1 = cp866   2 = UTF-16LE   3 = UTF-8
		       ;0 = по умолчанию (поддерживает байт кодировки в начале строки)

   ;инициализация работы, входящие: eax - номер шрифта
   .init:
      mov dword[koFont.number], eax
      call koFont.setSizes
      ret

   ;установить размер символов текущим шрифтом: ничего не принимает, ничего не отдаёт
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

   ;получить средний размер текста текущим шрифтом
   ;принимает на вход: eax - кол-во символов в строке
   ;возвращает: eax - длина текста в пикселях
   .getTextWidth:
      push edx
      push ebx
      mov ebx, dword[koFont.symbol_width]
      mul ebx
      pop ebx
      pop edx
      ret


   ;получить максимальное кол-во символов, которое влезет в заданную ширину области
   ;принимает: eax - ширина области в пикселях
   ;результат: eax - кол-во символов (если это значение равно ширине области, то со шрифтом что-то не так)
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

    ;получить максимальное кол-во строк текста, которые влезут на экране
    ;вход: eax - высота области в пикселях
    ;выход: eax -  rjkd-dj cnhjr
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




;Установить координаты текста по центру вьювера так чтобы максимум текста вместилось
;принимает: ebx - кол-во символов в строке, edi - указатель на koView (чтобы узнать его размеры)
;возвращает: ebx - новые координаты текста
setCoordInCenterForView:
   push eax
   push ecx
   push edi
   ;получим X координату
   mov eax, ebx
   call koFont.getTextWidth		       ;теперь в eax - длина текста в пикселях
   mov ebx, dword[edi+4]		;ebx = ширина koView
   cmp eax, ebx
   jb .coord_x				;если размер текста меньше размера области koView, то вычисляем смещение по Х
   mov eax, 0				;иначе координата Х = 0
   jmp .coord_y
.coord_x:
   sub ebx, eax
   shr ebx, 1
   mov eax, ebx
   shl eax, 16				;теперь в eax - координата X
.coord_y:
   ;теперь получим Y координату
   mov ebx, dword[edi+8]		;ebx = высота koView
   mov ecx, dword[koFont.symbol_height]  ;текущая высота текста
   cmp ebx, ecx 			;сравним высоту вьювера и высоту текста
   jbe .zero_y				;если высота текста больше высоты вьювера или равна ей, то координата Y = 0
   sub ebx, ecx 			;вычтем текущую высоту текста
   shr ebx, 1				;разделим пополам, теперь в ebx - координата Y текста
   jmp .coord_y_end
.zero_y:
   mov ebx, 0
.coord_y_end:				;теперь ebx = координата текста Y
   add eax, ebx
   mov ebx, eax 			;ebx = X*65536+Y
   pop edi
   pop ecx
   pop eax
   ret


;--------------------------------------------
;(с) Ефременков Сергей В., 2018