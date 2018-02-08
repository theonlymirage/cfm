;работа с папкой
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

;Основные функции модуля:
; - читаем заголовок
;  readBlock	      - читаем один блок: eax - номер блока, Результаты: в структурах
;ДРУГИЕ ФУНКЦИИ
;  setCurrentEncoding - установить текущую кодировку на основе знаний модуля koFont
;  setRootPath	      - установить корневой путь


;установить текущую кодировку (в которой будем получать имена)
setCurrentEncoding:
   push eax
   mov eax, dword[koFont.number]
   mov dword[read_folder_struct.encoding], eax
   mov byte[current_dir_encoding], al
   pop eax
   ret


;установить корневой путь
setRootPath:
   push eax
   mov eax, dword[read_folder_struct.encoding]
   cmp eax, 1
   jne @f
.zero:
   mov byte[read_folder_struct.name], '/'      ;для cp866
   mov byte[read_folder_struct.name + 1], 0
   jmp .finish
@@:
   cmp eax, 2
   jne .zero
   mov byte[read_folder_struct.name], 0      ;для utf
   mov dword[read_folder_struct.name + 1], root_path.utf16le
.finish:
   pop eax
   ret


;установить путь: esi - указатель на строку пути
setReadPath:
   mov dword[read_folder_struct.path_adr], esi
   ret


;Прочитать блок: в eax - номер нужного блока ;, esi - адресс строки с путём
readBlock:
   push eax
   push ebx
   mov dword[read_folder_struct.count], 1 ;устанавливаем кол-во читаемых блоков в единицу
   mov dword[read_folder_struct.start], eax ;устанавливаем номер блока
;{ ! 17 мая
;mov dword[read_folder_struct.path_adr], root_path_cp_17052017 ;esi
;}
   mcall 70, read_folder_struct  ;читаем блок

   test  eax,eax
   jz	 @f

   ;иначе ошибка - тут переходим на вывод уведомления об ошибка ФС
   jmp .finish

@@:
  cmp ebx, 1	 ;мы считывали один блок, если не получилось то беда
  jne .finish

  ;обновляем общее число файлов в папке
  ;call updateElementsCountInFolder

  mov ebx, read_folder_return_struct.header ;ebx
  add ebx, 8
  mov eax, dword[ebx]
  ;!от 17 мая 2017
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
   .start	dd  0 ;с какого блока начинать читать, нумерация с 0
   .encoding	dd  0 ;1-cp866, 2-UTF-16LE, 3-utf8
   .count	dd  1 ;кол-во читаемых блоков
   .return	dd  read_folder_return_struct ;адрес памяти для получаемого блока БДВК с заголовком
   .name	db  0
   .path_adr	dd  path ;0
   ;.name db   1000 dup(0)
   ;или db 0
   ;	.path dd 0
path db 1024 dup(0)

read_folder_return_struct:
   .header db 32 dup (0)
   .block  db 560 dup (0)   ;Если БДВК содержит имя в cp866, то длина БДВК составляет 304 байта, иначе - 560 байт.



;работа с текущей папкой приложения
current_dir_encoding db 2
current_dir db 2048 dup(0)
initCurrentDir:
  call getCurrentDir
  ret
setCurrentDir: ;ecx - указатель на строку с директорией в текущей кодировке
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
;ФУНКЦИИ ОБРАБОТКИ СТРУКТУРЫ
;заголовка

;файла
FileParser:
   .getSizeInByte: ;размер файла в байтах  положить в eax
      ;push eax
      mov eax, dword[read_folder_return_struct.block + 32]
      ;pop eax
      ret





;-----------------------------------
;удаление и создание папок:

;структура для работы с папками
folder_work:
  .func_number dd 0 ;8=удаление, 9=создание
  dq 0
  dq 0
  db 0
  .name: dd 0 ;folder_name
;folder_name db 2048 dup(0)


;удалить пустую папку (не пустой каталог удалить нельзя - будет ошибка)
;вход: esi - адрес строки с путём, eax - номер функции
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

function_folder_mkdir: ;создать папку: esi - адрес строки с путём
   push eax
   mov eax, 9
   call function_folder
   pop eax
   ret

function_folder_rm: ;удалить папку: esi - адрес строки с путём
   push eax
   mov eax, 8
   call function_folder
   pop eax
   ret













;--------------------------------------------
;(с) Ефременков Сергей В., 2018