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

format binary as ""
use32
org 0x0    ; базовый адрес размещения кода, всегда 0x0

; заголовок
db 'MENUET01'	  ;магический идентификатор
dd 0x01 	  ;версия
dd START	  ;адрес точки старта программы
dd I_END	  ;адрес конца, по факту размер файла программы
dd 0x100000	  ;требуемое кол-во памяти для загрузки программы
dd 0x7fff0	  ;начальное значение регистра esp - адрес конца области стэка так как стэк растет в сторону меньших адресов 
dd 0, 0 	  ;адрес строки параметров и адрес строки пути исполняемого файла
 
; начало области кода, подключаем стандартный модуль для "облегчения" работы:
include './BSI/macros.inc'
include "consoleDisplayMode.inc"

START:
   mcall  68, 11	 ;инициализация кучи
   mov ebx, 11000000000000000000000000100111b ;подписываемся на интересные нам события
   mcall 40
   call consoleMode.init
   call   draw_window	 ;рисуем окно программы
 
; After the window is drawn, it's practical to have the main loop.
; Events are distributed from here.
event_wait:
	mov	eax, 10 		; function 10 : wait until event
	mcall				; event type is returned in eax
 
	cmp	eax, 1		; сообщение о перерисовке ?
	je	red			; Expl.: there has been activity on screen and
					; parts of the applications has to be redrawn.
	cmp	eax, 2			; Event key in buffer ?
	je	key			; Expl.: User has pressed a key while the
					; app is at the top of the window stack.
	cmp	eax, 3			; Event button in buffer ?
	je	button			; Expl.: User has pressed one of the applications buttons.

	cmp   eax, 6		;обработка перемещений и нажатия мыши
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
	je EmptyBuf    ;буфер пуст

	;проверяем горячие клавиши
	cmp al, 0
	jne checkHotKey

	;если ничего не совпало, то проверяем обычные клавиши
	call consoleMode.keyboard

	jmp EmptyBuf
checkHotKey:
	cmp al, 2
	jne EmptyBuf
	call consoleMode.hotkey ;тут проверяем горячии клавиши
EmptyBuf:
	jmp	event_wait		; Just read the key, ignore it and jump to event_wait.
mouse:
	call consoleMode.mouse		  ;37-ая функция
	jmp	event_wait
button: 				; Buttonpress event handler
	mcall 17		  ; The button number defined in window_draw is returned to ah.
	cmp	ah,1			; button id=1 ?
	jne	noclose
	call consoleMode.deinit
	mov	eax,-1			; Функция -1 : закрыть эту программу
	mcall
noclose:
	jmp	event_wait		; This is for ignored events, useful at development
 

include "koFont.inc"
include "work_with_folder.inc"
include "work_with_app.inc"
include "work_with_clipboard.inc"

setWindowTitle: ;!!! для замены: установить новый заголовок окна
   push eax	       ;не тестировал
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
   mcall 12, 1			   ;начало перерисовки окна
   call getWindowParam		   ;получение и/или обновление параметров окна
;не забыть потом применить полученные параметры окна

    ;рисуем само окно приложения в зависимости от режима работы
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
   mov	   edx, dword[applicationSettings.window_background_color]    ; цвет рабочей (клиентской) области окна RRGGBB
				   ; 0x02000000 = window type 4 (fixed size, skinned window)
   mov	   esi, 0x808899ff	   ; color of grab bar	RRGGBB: 0x80000000 = color glide
   mov	   edi, text_application_title.cp866
   mcall 0			   ;функция 0: определяет и рисует окно

   ;call updateWindowSizes  ;обновляем размеры окна

   call consoleMode.draw     ;отрисовка текущего режима работы внутри окна
   mcall 12, 2			   ;завершение перерисовки окна
   ret

applicationSettings: ;настройки файлового менеджера
   .window_background_color dd 0x13ffffff  ;0x14ffffff ;по умолчанию белый  (перенести в параметры окна)


windowSettings:      ;настройки главного окна файлового менеджера
   .x	    dd 0     ;обычные настройки вьювера: координаты и размеры (вынести во вьювер потом)
   .width   dd 0
   .y	    dd 0
   .height  dd 0

   .cx	    dd 0     ;аналогично для содержимого окна - клиентская область
   .cy	    dd 0
   .cwidth  dd 0
   .cheight dd 0

   .background_color	dd 0x13000000  ;0x14ffffff ;по умолчанию белый
   .title_skin_height	dd 0	      ;высота заголовка в текущей теме окна
   .status  dd 1     ;статус-состояние окна
   ; бит 0 (маска 1): окно максимизировано
   ; бит 1 (маска 2): окно минимизировано в панель задач
   ; бит 2 (маска 4): окно свёрнуто в заголовок
   .mode dd 0 ; 0 -обычное окно; 1 - полноэкранный режим

getWindowParam:  ;получаем/обновляем параметры главного окна
   push eax
   push ebx
   push ecx

   mcall 9, temp_buf_1024, -1  ;воспользуемся временным буфером, чтобы снизить размер занимаемой памяти
			       ; -1 означает текущий поток и текущее окно (заменить потом при работе с окнами)
   mov	 eax,[ebx+34]	  ;параметры самого окна
   mov	 [windowSettings.x],eax
   mov	 eax,[ebx+38]
   mov	 [windowSettings.y],eax
   mov	 eax,[ebx+42]
   mov	 [windowSettings.width],eax
   mov	 eax,[ebx+46]
   mov	 [windowSettings.height],eax

   mov	 eax,[ebx+54]	   ;клиентская область окна
   mov	 [windowSettings.cx],eax
   mov	 eax,[ebx+58]
   mov	 [windowSettings.cy],eax
   mov	 eax,[ebx+62]
   mov	 [windowSettings.cwidth],eax
   mov	 eax,[ebx+66]
   mov	 [windowSettings.cheight],eax

   mov	 eax,[ebx+70]	   ;состояние окна
   mov	 [windowSettings.status],eax

   mcall 48, 4		   ;размер скина - заголовок
   mov dword[windowSettings.title_skin_height], eax

   pop ecx
   pop ebx
   pop eax
   ret

;  *************   ОБЛАСТЬ ДАННЫХ   *****************
elements_count_string db 12 dup (0)  ; для записи 256^4 состояний хватит и 10 + нуль на конце - ещё один ноль для страховки
   include  'text.inc'
I_END:
   ;буфер на 1024 байта - используется временно разными участками кода, чтобы сократить общий объём приложения
   temp_buf_1024:  rb 1024

;--------------------------------------------
;(с) Ефременков Сергей В., 2018