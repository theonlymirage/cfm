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

text_application_title:
;  .default db "Mirage File Manager", 0
  .cp866   db "Mirage File Manager", 0
  .utf16le db 'M', 0, 'i', 0, 'r', 0, 'a', 0, 'g', 0, 'e', 0, ' ', 0, 'F', 0, 'i', 0, 'l', 0, 'e', 0, ' ', 0, 'M', 0, 'a', 0, 'n', 0, 'a', 0, 'g', 0, 'e', 0, 'r', 0,0,0
;  .utf8    db "Mirage File Manager", 0
text_application_version:
;  .default db 'prototype version 0.0', 0
;  .cp866   db 'prototype version 0.0', 0
  .utf16le db 'p', 0,'r', 0, 'o', 0, 't', 0, 'o', 0, 't', 0, 'y', 0, 'p', 0, 'e', 0, ' ', 0, 'v', 0, 'e', 0, 'r', 0, 's', 0, 'i', 0, 'o', 0, 'n', 0, ' ', 0, '0', 0, '.', 0, '0', 0, 0, 0
;  .utf8    db 'prototype version 0.0', 0
text_test_command:
;  .default db 'ls /cd2/1', 0
;  .cp866   db 'ls /cd2/1', 0
  .utf16le db 'l', 0, 's', 0, ' ', 0, '/', 0, 'c', 0, 'd', 0, '2', 0, '/', 0, '1', 0, 0, 0
;  .utf8    db 'ls /cd2/1', 0
text_user_input:
;  .default db '#>_', 0
;  .cp866   db '#>_', 0
  .utf16le db '#', 0, '>', 0,	 0,0 ;'_', 0, 0, 0
;  .utf8    db '#>_', 0
text_unknown_error:
   .utf16le db 'U', 0, 'n', 0, 'k', 0, 'n', 0, 'o', 0, 'w', 0, 'n', 0, ' ', 0, 'e', 0, 'r', 0, 'r', 0, 'o', 0, 'r', 0, '!', 0 , 0, 0
text_true:
   ;.cp866 db 'true', 0
   .utf16le db 't', 0, 'r', 0, 'u', 0, 'e', 0, 0, 0
text_false:
   ;.cp866 db 'false', 0
   .utf16le db 'f', 0, 'a', 0, 'l', 0, 's', 0, 'e', 0, 0, 0
text_yes:
   ;.cp866 db 'yes', 0
   .utf16le db 'y', 0, 'e', 0, 's', 0, 0, 0
text_no:
   ;.cp866 db 'no', 0
   .utf16le db 'n', 0, 'o', 0, 0, 0

en: ;тексты с кодами ошибок
   .code0  db "Done, successfully completed.", 0
   .error  db "ERROR: ", 0
   .code2  db "", 0
   .code3  db "", 0
   .code5  db "File not found.", 0
   .code6  db "", 0
   .code7  db "", 0
   .code8  db "", 0
   .code9  db "", 0
   .code10 db "", 0
   .code11 db "", 0
   .code12 db "", 0
   .code_unknown db "Unknown error!", 0


;0 = успешно
;2 = функция не поддерживается для данной файловой системы
;3 = неизвестная файловая система
;5 = файл не найден
;6 = файл закончился
;7 = указатель вне памяти приложения
;8 = диск заполнен
;9 = ошибка файловой системы
;10 = доступ запрещён
;11 = ошибка устройства
;12 = файловой системе недостаточно оперативной памяти


command_listUTF16LE:
   .ls db 'l', 0, 's', 0, 0, 0
   .cd db 'c', 0, 'd', 0, 0, 0
   .exit db 'e', 0, 'x', 0, 'i', 0, 't', 0, 0, 0
   .run db 'r', 0, 'u', 0, 'n', 0, 0, 0
   .echo db 'e', 0, 'c', 0, 'h', 0, 'o', 0, 0, 0
   .print db 'p', 0, 'r', 0, 'i', 0, 'n', 0, 't', 0, 0, 0
   .pwd db 'p', 0, 'w', 0, 'd', 0, 0, 0
   .clear db 'c', 0, 'l', 0, 'e', 0, 'a', 0, 'r', 0, 0, 0
   .cls db 'c', 0, 'l', 0, 's', 0, 0, 0
   .set_mouse_position db 's', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, '-', 0, 'p', 0, 'o', 0, 's', 0, 'i', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0, 0, 0
   .minimize_all_windows db 'm', 0, 'i', 0, 'n', 0, 'i', 0, 'm', 0, 'i', 0, 'z', 0, 'e', 0, '-', 0, 'a', 0, 'l', 0, 'l', 0, '-', 0, 'w', 0, 'i', 0, 'n', 0, 'd', 0, 'o', 0, 'w', 0, 's', 0, 0, 0
   .call db 'c', 0, 'a', 0, 'l', 0, 'l', 0, 0, 0
   .reboot db 'r', 0, 'e', 0, 'b', 0, 'o', 0, 'o', 0, 't', 0, 0, 0
   .restart_kernel db 'r', 0, 'e', 0, 's', 0, 't', 0, 'a', 0, 'r', 0, 't', 0, '-', 0, 'k', 0, 'e', 0, 'r', 0, 'n', 0, 'e', 0, 'l', 0, 0, 0
   .shutdown db 's', 0, 'h', 0, 'u', 0, 't', 0, 'd', 0, 'o', 0, 'w', 0, 'n', 0, 0, 0
   .echo_on  db 'e', 0, 'c', 0, 'h', 0, 'o', 0, '-', 0, 'o', 0, 'n', 0, 0, 0
   .echo_off db 'e', 0, 'c', 0, 'h', 0, 'o', 0, '-', 0, 'o', 0, 'f', 0, 'f', 0, 0, 0
   .iskef db 'i', 0, 's', 0, 'k', 0, 'e', 0, 'f', 0, 0, 0
   .time db 't', 0, 'i', 0, 'm', 0, 'e', 0, 0, 0
   .kill db 'k', 0, 'i', 0, 'l', 0, 'l', 0, 0, 0
   .pause db 'p', 0, 'a', 0, 'u', 0, 's', 0, 'e', 0, 0, 0
   .set_mode db 's', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'd', 0, 'e', 0, 0, 0
   .background_redraw db 'b', 0, 'g', 0, 'i', 0, '-', 0, 'r', 0, 'e', 0, 'd', 0, 'r', 0, 'a', 0, 'w', 0, 0, 0
   .background_set_mode db 'b', 0, 'g', 0, 'i', 0, '-', 0, 's', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'd', 0, 'e', 0, 0, 0
   .terminate db 't', 0, 'e', 0, 'r', 0, 'm', 0, 'i', 0, 'n', 0, 'a', 0, 't', 0, 'e', 0, 0, 0
   .get_core_version db 'g', 0, 'e', 0, 't', 0, '-', 0, 'c', 0, 'o', 0, 'r', 0, 'e', 0, '-', 0, 'v', 0, 'e', 0, 'r', 0, 's', 0, 'i', 0, 'o', 0, 'n', 0, 0, 0
   .get_free_ram db 'g', 0, 'e', 0, 't', 0, '-', 0, 'f', 0, 'r', 0, 'e', 0, 'e', 0, '-', 0, 'r', 0, 'a', 0, 'm', 0, 0, 0
   .get_ram_size db 'g', 0, 'e', 0, 't', 0, '-', 0, 'r', 0, 'a', 0, 'm', 0, '-', 0, 's', 0, 'i', 0, 'z', 0, 'e', 0, 0, 0
   .get_mouse_speed db 'g', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, '-', 0, 's', 0, 'p', 0, 'e', 0, 'e', 0, 'd',0,0,0
   .set_mouse_speed db 's', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, '-', 0, 's', 0, 'p', 0, 'e', 0, 'e', 0, 'd',0,0,0
   .set_mouse_sensitivity db 's', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, '-', 0, 's', 0, 'e', 0, 'n', 0, 's', 0, 'i', 0, 't', 0, 'i', 0, 'v', 0, 'i', 0, 't', 0, 'y', 0, 0, 0
   .get_mouse_sensitivity db 'g', 0, 'e', 0, 't', 0, '-', 0, 'm', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, '-', 0, 's', 0, 'e', 0, 'n', 0, 's', 0, 'i', 0, 't', 0, 'i', 0, 'v', 0, 'i', 0, 't', 0, 'y', 0, 0, 0
   .pslot db 'p', 0, 's', 0, 'l', 0, 'o', 0, 't', 0, 0, 0
   .eject db 'e', 0, 'j', 0, 'e', 0, 'c', 0, 't', 0, 0, 0
   .date db 'd', 0, 'a', 0, 't', 0, 'e', 0, 0, 0
   ;добавленные новые:
   .help db 'h', 0, 'e', 0, 'l', 0, 'p', 0, 0, 0
   .mkdir db 'm', 0, 'k', 0, 'd', 0, 'i', 0, 'r', 0, 0, 0
   .md db 'm', 0, 'd', 0, 0, 0
   .rm db 'r', 0, 'm', 0, 0, 0
   .current_color_scheme db 'c', 0, 'u', 0, 'r', 0, 'r', 0, 'e', 0, 'n', 0, 't', 0, '-', 0, 'c', 0, 'o', 0, 'l', 0, 'o', 0, 'r', 0, '-', 0, 's', 0, 'c', 0, 'h', 0, 'e', 0, 'm', 0, 'e', 0, 0, 0
   .get_network_device_count db 'g', 0, 'e', 0, 't', 0, '-', 0, 'n', 0, 'e', 0, 't', 0, 'w', 0, 'o', 0, 'r', 0, 'k', 0, '-', 0, 'd', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, '-', 0, 'c', 0, 'o', 0, 'u', 0, 'n', 0, 't', 0, 0, 0
   .get_network_device_type db 'g', 0, 'e', 0, 't', 0, '-', 0, 'n', 0, 'e', 0, 't', 0, 'w', 0, 'o', 0, 'r', 0, 'k', 0, '-', 0, 'd', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, '-', 0, 't', 0, 'y', 0, 'p', 0, 'e', 0, 0, 0


;   .ipconfig db 'i', 0, 'p', 0, 'c', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0, 0, 0

;   .man  db 'm', 0, 'a', 0, 'n', 0, 0, 0
;   .version db 'v', 0, 'e', 0, 'r', 0, 's', 0, 'i', 0, 'o', 0, 'n', 0, 0, 0
;   .history db 'h', 0, 'i', 0, 's', 0, 't', 0, 'o', 0, 'r', 0, 'y', 0, 0, 0
;   .ver db 'v', 0, 'e', 0, 'r', 0, 0, 0
;   .tail db 't', 0, 'a', 0, 'i', 0, 'l', 0, 0, 0
;   .ps db 'p', 0, 's', 0, 0, 0
;   .about db 'a', 0, 'b', 0, 'o', 0, 'u', 0, 't', 0, 0, 0
;   .setColorFile db 'set-color-file', 0, 0, 0
;   .more db 'm', 0, 'o', 0, 'r', 0, 'e', 0, 0, 0
;   .uptime db 'u', 0, 'p', 0, 't', 0, 'i', 0, 'm', 0, 'e', 0, 0, 0
;   .free db 'f', 0, 'r', 0, 'e', 0, 'e', 0, 0, 0
;   .exec db 'e', 0, 'x', 0, 'e', 0, 'c', 0, 0, 0
;   .sleep db 's', 0, 'l', 0, 'e', 0, 'e', 0, 'p', 0, 0, 0
;   .halt db 'h', 0, 'a', 0, 'l', 0, 't', 0,0,0
;   .get_language db 'g', 0, 'e', 0, 't', 0, '-', 0, 'l', 0, 'a', 0, 'n', 0, 'g', 0, 0, 0

command_listUTF16LE_end: ;метка адреса для использования командой help
   .folder db 'f', 0, 'o', 0, 'l', 0, 'd', 0, 'e', 0, 'r', 0, 0, 0
   .file_name db 'f', 0, 'i', 0, 'l', 0, 'e', 0, ' ', 0, 'n', 0, 'a', 0, 'm', 0, 'e', 0, 0, 0
   .application db 'e', 0, 'x', 0, 'e', 0, 'c', 0, 'u', 0, 't', 0, 'a', 0, 'b', 0, 'l', 0, 'e', 0, ' ', 0, 'f', 0, 'i', 0, 'l', 0, 'e', 0, 0, 0
   .command db 'u', 0, 's', 0, 'e', 0, 'r', 0, ' ', 0, 'c', 0, 'o', 0, 'm', 0, 'm', 0, 'a', 0, 'n', 0, 'd', 0, 0, 0
   .error db 'w', 0, 'a', 0, 'r', 0, 'n', 0, 'i', 0, 'n', 0, 'g', 0, ' ', 0, '&', 0, ' ', 0, 'e', 0, 'r', 0, 'r', 0, 'o', 0, 'r', 0, 0, 0
   .partition db 'p', 0, 'a', 0, 'r', 0, 't', 0, 'i', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0, ' ', 0, 'c', 0, 'o', 0, 'l', 0, 'o', 0, 'r', 0, 0, 0
   .text db 't', 0, 'e', 0, 'x', 0, 't', 0, ' ', 0, 'c', 0, 'o', 0, 'l', 0, 'o', 0, 'r', 0, 0, 0
   .other db 'o', 0, 't', 0, 'h', 0, 'e', 0, 'r', 0, 's', 0, 0, 0


include 'network.inc'

;показать цветовую схему: вывод цветных строк с описанием обозначения цвета
tool_current_color_scheme:
  push eax
  push ecx
  push esi

  ;отображаем цвет партиции/раздела
  mov ecx, 32
  mov esi, command_listUTF16LE_end.partition
  call createStringHistory
  mov eax, 0
  mov al, 6
  call addInHistory

  ;отображаем цвет папки
  mov ecx, 14
  mov esi, command_listUTF16LE_end.folder
  call createStringHistory
  mov eax, 0
  mov al, 3
  call addInHistory

  ;отображаем цвет обычного файла
  mov ecx, 20
  mov esi, command_listUTF16LE_end.file_name
  call createStringHistory
  mov eax, 0
  mov al, 4
  call addInHistory

  ;отображаем цвет обычного текста
  mov ecx, 22
  mov esi, command_listUTF16LE_end.text
  call createStringHistory
  mov eax, 0
  mov al, 0 ;7
  call addInHistory

  ;отображаем цвет с текстом предупреждения об ошибке
  mov ecx, 32
  mov esi, command_listUTF16LE_end.error
  call createStringHistory
  mov eax, 0
  mov al, 2
  call addInHistory

  ;отображаем цвет пользовательского запроса
  mov ecx, 26
  mov esi, command_listUTF16LE_end.command
  call createStringHistory
  mov eax, 0
  mov al, 7  ;он 4, но 4 рисуется с начальными символами а они нам сейчас не надо
  call addInHistory

  ;отображаем цвет исполняемого файла
  mov ecx, 32
  mov esi, command_listUTF16LE_end.application
  call createStringHistory
  mov eax, 0
  mov al, 5
  call addInHistory

  ;иначе всё другое
  mov ecx, 14
  mov esi, command_listUTF16LE_end.other
  call createStringHistory
  mov eax, 0
  mov al, 8
  call addInHistory

  pop esi
  pop ecx
  pop eax
  ret

tool_mkdir:
  push esi
  mov esi, consoleModeSettings.current_params_encoding
  call trimLeftLink
  call function_folder_mkdir
  pop esi
  ret

tool_rm:
  push esi
  mov esi, consoleModeSettings.current_params_encoding
  call trimLeftLink
  call function_folder_rm
  pop esi
  ret


tool_help:
   push esi
   push edi
   mov esi, command_listUTF16LE
   mov edi, command_listUTF16LE_end
   call printBlockStringUTF16LE
   pop edi
   pop esi
   ret


date_data db 22 dup(0)
tool_date:
   push eax
   push ecx
   push esi

   mov	   eax, 29
   int	   0x40

   mov esi, date_data

   mov ecx, eax 	;получаем день
   and ecx, 0x00F00000
   shr ecx, 20
   add ecx, '0'
   mov byte[esi], cl
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, eax
   and ecx, 0x000F0000
   shr ecx, 16
   add ecx, '0'
  ; mov word[esi], cx
   mov byte[esi], cl
   mov byte[esi+1], 0
   mov byte[esi+2], '.'
   mov byte[esi+3], 0
   add esi, 4


   mov ecx, eax 	;получаем месяц
   and ecx, 0x0000F000
   shr ecx, 12
   add ecx, '0'
   mov byte[esi], cl
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, eax
   and ecx, 0x00000F00
   shr ecx, 8
   add ecx, '0'
   mov word[esi], cx
   mov byte[esi+2], '.'
   mov byte[esi+3], 0
   add esi, 4


   mov ecx, eax 	;получаем год
   and ecx, 0x000000F0
   shr ecx, 4
   add ecx, '0'
   mov word[esi], cx
   add esi, 2

   mov ecx, eax
   and ecx, 0x0000000F
   add ecx, '0'
   mov word[esi], cx
   add esi, 2

   mov word[esi], 0

   mov esi, date_data
   mov ecx, 22
   call addStringInHistory
   pop esi
   pop ecx
   pop eax
   ret
;---- ЭТОТ БЛОК НЕ ДОДЕЛАН И НЕ РАБОТАЕТ
lang_list:
.en db 'E', 0, 'n', 0, 'g', 0, 'l', 0, 'i', 0, 's', 0, 'h', 0, 0, 0
.fi db 'F', 0, 'i', 0, 'n', 0, 'n', 0, 'i', 0, 's', 0, 'h', 0, 0, 0
.ger db 'D', 0, 'e', 0, 'u', 0, 't', 0, 's', 0, 'c', 0, 'h', 0, 0, 0
.ru db 'Р', 0, 'у', 0, 'с', 0, 'с', 0, 'к', 0, 'и', 0, 'й', 0, 0, 0
tool_get_language:
   push eax
   push ebx

   mcall 26, 5
   cmp eax, 1
   jne @f
	;english
@@:
   cmp eax, 2
   jne @f
   ;fi
@@:
   cmp eax, 3
   jne @f
   ;ger
@@:
   cmp eax, 4
   jne @f
   ;russian
@@:
   ;number eax
.end:
   pop ebx
   pop eax
   ret
;-------
tool_eject: ;извлечь лоток привода диска
   push eax	  ;не тестировалась
   push ebx
   push ecx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   mcall 24, 4, ecx
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

;----------
pslot_value db 22 dup(0)
tool_pslot:  ;получить номер слота процесса/потока по идентификатору
   push eax
   push ebx
   push ecx
   push edx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   mcall 18, 21, ecx
   cmp eax, 0
   je tool_pslot.error
   mov ecx, eax
   mov esi, pslot_value
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory
   jmp @f
.error:
   call printError
 @@:
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret
;----------
mouse_sensitivity db 22 dup(0)
mouse_speed_value db 22 dup(0) ;оптимизировать и переделать потом
tool_get_mouse_sensitivity:  ;получить чувствительность мыши
   push eax
   push ebx
   push ecx
   push esi
   mcall 18, 19, 2
   mov ecx, eax
   mov esi, mouse_sensitivity
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

tool_set_mouse_sensitivity:  ;задать чувствительность мыши
   push eax
   push ebx
   push ecx
   push edx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   mov edx, ecx
   mcall 18, 19, 3
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

tool_get_mouse_speed:	     ;получить скорость мыши
   push eax
   push ebx
   push ecx
   push esi
   mcall 18, 19, 0
   mov ecx, eax
   mov esi, mouse_speed_value
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

tool_set_mouse_speed:	     ;задать скорость мыши
   push eax
   push ebx
   push ecx
   push edx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   mov edx, ecx
   mcall 18, 19, 1
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret
;----------
ram_info:
 .headtotal db 'R', 0, 'A', 0, 'M', 0, ' ', 0, '[', 0, 'K', 0, 'b', 0, ']', 0, ':', 0, ' ', 0
 .total db 26 dup (0)
 .headfree db 'F', 0, 'r', 0, 'e', 0, 'e', 0, ' ', 0, 'R', 0, 'A', 0, 'M', 0, ' ', 0, '[', 0, 'K', 0, 'b', 0, ']', 0, ':', 0, ' ', 0
 .free db 26 dup(0)
tool_get_ram_size: ;получить весь объём памяти ОЗУ в Кб
   push eax
   push ebx
   push ecx
   push esi
   mcall 18, 17
   mov ecx, eax
   mov esi, ram_info.total
   call numberToStringUTF16LE

   mov ecx, 28+26
   mov esi,ram_info.headtotal
   call addStringInHistory
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret
tool_get_free_ram:  ;получить размер свободной памяти ОЗУ в Кб
   push eax
   push ebx
   push ecx
   push esi
   mcall 18, 16
   mov ecx, eax
   mov esi, ram_info.free
   call numberToStringUTF16LE

   mov ecx, 28+26
   mov esi,ram_info.headfree
   call addStringInHistory
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret
;--------------------------
core_version_utf16le:
 .head db 'K', 0, 'o', 0, 'l', 0, 'i', 0, 'b', 0, 'r', 0, 'i', 0, ' ', 0, 'O', 0, 'S', 0, ' ', 0, ' ', 0, 'K', 0, 'e', 0, 'r', 0, 'n', 0, 'e', 0, 'l', 0, ' ', 0, 'v', 0, 'e', 0, 'r', 0, 's', 0, 'i', 0, 'o', 0, 'n', 0, ' ', 0
 .code: db 400 dup (0)
core_version_info db 16 dup(0) ;16 dup(0)
;dd 0
;reserv db 0
;rev dd 0

tool_get_core_version:
   push eax
   push ebx
   push ecx
   push esi
   push edi
   mcall 18, 13, core_version_info

   mov edi, core_version_info
   mov esi, core_version_utf16le.code

   mov ecx, 0
   mov cl, byte[edi]
   call numberToStringUTF16LE
   add esi, 2
   inc edi

   mov byte[esi], '.'
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, 0
   mov cl, byte[edi]
   call numberToStringUTF16LE
   add esi, 2
   inc edi

   mov byte[esi], '.'
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, 0
   mov cl, byte[edi]
   call numberToStringUTF16LE
   add esi, 2
   inc edi

   mov byte[esi], '.'
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, 0
   mov cl, byte[edi]
   call numberToStringUTF16LE
   add esi, 2
   add edi, 2

   mov byte[esi], ' '
   mov byte[esi+1], 0
   add esi, 2

   mov byte[esi], 's'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'v'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'n'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], '-'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'r'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'e'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'v'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'i'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 's'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'i'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'o'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], 'n'
   mov byte[esi+1], 0
   add esi, 2
   mov byte[esi], ' '
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, 0
   mov ecx, dword[edi]
   call numberToStringUTF16LE
;   add esi, 2

;   mov byte[esi], '.'
;   mov byte[esi+1], 0
;   add esi, 2

;   mov word[esi], 0	;завершающий ноль

   mov ecx, 400+54
   mov esi, core_version_utf16le
   call addStringInHistory
   pop edi
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

tool_terminate: ;завершить процесс/поток по слоту
   push eax
   push ebx
   push ecx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   mcall 18, 2, ecx
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

tool_background_set_mode: ;установить режим отрисовки фона: 1=замостить, 2=растянуть
   push eax		  ;работоспособность не протестирована
   push ebx
   push ecx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   cmp ecx, 2	;пока только два параметра 1 и 2, если задано другое то игнорируем
   ja tool_background_set_mode.error
   cmp ecx, 0
   jbe tool_background_set_mode.error
   mcall 15, 4, ecx
   jmp @f
.error:
   call printError
@@:
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

tool_background_redraw: ;перерисовать фон рабочего стола
   push eax
   push ebx
   mcall 15, 3
   pop ebx
   pop eax
   ret

tool_set_mode: ;установить режим отрисовки окна файлового менеджера: полноэкранный или нет
   push ecx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   cmp ecx, 1	;пока только два режима 0 и 1, если задано другое то игнорируем
   ja tool_set_mode.error
   cmp ecx, dword[windowSettings.mode]	  ;если повторно пытаемся поставить уже выставленный режим, то ничего не надо делать
   je @f
   mov dword[windowSettings.mode], ecx
   call updateWindowSizes  ;обновляем размеры окна
   call consoleMode.update_history_string_count  ;обновляем число строк
   jmp @f
.error:
   call printError
@@:
   pop esi
   pop ecx
   ret

tool_pause: ;ничего не делает в течении заданных секунд (в отличие от sleep нельзя разбудить)
   push eax
   push ebx
   push ecx
   push edx
   push esi
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger
   cmp ecx, 0
   je @f
   mov edx, 0
   mov eax, 100
   mul ecx
   mov ebx, eax
   mcall 5
@@:
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

time_data db 22 dup(0) ; создаём строку 22 байта для хранения времени
tool_time:  ;получить системное время
   push eax
   push ecx
   push esi

   ;mcall 3
   mov	   eax, 3
   int	   0x40

   mov esi, time_data

   mov ecx, eax 	;получаем часы
   and ecx, 0x000000F0
   shr ecx, 4
   add ecx, '0'
   mov word[esi], cx
   add esi, 2

   mov ecx, eax
   and ecx, 0x0000000F
   add ecx, '0'
   mov word[esi], cx
   mov byte[esi+2], ':'
   mov byte[esi+3], 0
   add esi, 4


   mov ecx, eax 	;получаем минуты
   and ecx, 0x0000F000
   shr ecx, 12
   add ecx, '0'
   mov byte[esi], cl
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, eax
   and ecx, 0x00000F00
   shr ecx, 8
   add ecx, '0'
   mov word[esi], cx
   mov byte[esi+2], ':'
   mov byte[esi+3], 0
   add esi, 4


   mov ecx, eax 	;получаем секунды
   and ecx, 0x00F00000
   shr ecx, 20
   add ecx, '0'
   mov byte[esi], cl
   mov byte[esi+1], 0
   add esi, 2

   mov ecx, eax
   and ecx, 0x000F0000
   shr ecx, 16
   add ecx, '0'
  ; mov word[esi], cx
   mov byte[esi], cl
   mov byte[esi+1], 0
   add esi, 2

   mov word[esi], 0

   mov esi, time_data
   mov ecx, 22
   call addStringInHistory
   pop esi
   pop ecx
   pop eax
   ret

include 'koZeroStringUTF16LE.inc'


tool_kill: ;Завершить процесс/поток по идентификатору
   push eax
   push ebx
   push ecx

   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger  ;преобразовываем текст-число из параметров в число в регистре ecx

   mcall 18, 18, ecx
   cmp eax, 0
   je @f
   call printError
   ;call printParams
 @@:
   pop ecx
   pop ebx
   pop eax
   ret

tool_minimize_all_windows: ;минимизировать все окна
   push eax
   push ebx
   mcall 18, 23
   pop ebx
   pop eax
   ret

tool_set_mouse_position:  ;поместить курсор мыши в центр экрана.
   push eax
   push ebx
   mcall 18, 15
   cmp eax, 0
   je @f
   call printError ;иначе ошибка
@@:
   pop ebx
   pop eax
   ret

tool_pwd:
   call getCurrentDir
   mov ecx, 2048
      mov esi, current_dir
      call createStringHistory
      mov eax, 0
      mov al, 0
      call addInHistory
   ret

tool_cd:
   push ecx
   mov ecx, consoleModeSettings.current_params
   call setCurrentDir
   pop ecx
   ret

tool_cls: ;очистить экран с историей
   call deinitAllSlotHistory
   mov dword[consoleModeSettings.data_count], 0
   ret

tool_run:
   call clearCommandText

   ;загружаем файл переданный в параметрах
   push eax
   push ebx
   push ecx
   push edx
   mcall 68, 28, consoleModeSettings.current_params, dword[koFont.number]
   cmp eax, 0  ;если не удалось загрузить - выходим с ошибкой
   jne tool_run.n
   call printError
   jmp @f
.n:
   cmp edx, 0  ;выходим, если файл пустой (его развер ноль и нет содержимого)
   je @f

   ;если же всё хорошо то интерпретируем его как ввод символом пользователем
   ;по одному символу переносим в строку ввода, пока не закончится весь файл
   mov bh, byte[eax]

   push edx
   push eax
   mov eax, 0
   mov ah, bh
   call consoleMode.keyboard_interpretator
   pop eax
   pop edx

   dec edx
   inc eax
   jmp tool_run.n
@@:
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

tool_shutdown: ;выключить систему, компьютер
   push eax
   push ebx
   push ecx
   mcall 18, 9, 2
   cmp eax, 0
   je @f
   call printError
@@:
   pop ecx
   pop ebx
   pop eax
   ret

tool_reboot: ;перезагрузка системы
   push eax
   push ebx
   push ecx
   mcall 18, 9, 3
   cmp eax, 0
   je @f
   call printError
@@:
   pop ecx
   pop ebx
   pop eax
   ret

tool_restart_kernel: ;перезагрузка ядра ОС
   push eax
   push ebx
   push ecx
   mcall 18, 9, 4
   cmp eax, 0
   je @f
   call printError
@@:
   pop ecx
   pop ebx
   pop eax
   ret

tool_echo_on:
   mov byte[consoleModeSettings.echo_mode], 1
   ;добавляю строку запроса в историю
      push ecx
      push esi
      push eax
      mov ecx, dword[consoleModeSettings.command_pos]
      add ecx, 2
      mov esi, consoleModeSettings.command_text
      call createStringHistory
      mov eax, 0
      mov al, 1 ;это пользовательский текст запроса
      call addInHistory
      pop eax
      pop esi
      pop ecx
   ret

tool_echo_off:
   mov byte[consoleModeSettings.echo_mode], 0
   ret



addStringInHistory: ;ecx - размер строки вместе с завершающим нулём в байтах; esi - указатель на строку
   push ecx
      push esi
      push eax
      call createStringHistory
      mov eax, 0
      mov al, 0 ;это пользовательский текст запроса
      call addInHistory
      pop eax
      pop esi
      pop ecx
      ret

tool_iskef: ;проверяет (по наличию сигнатуры 'MENUET01') является файл исполнимым файлом колибри ос
   push eax
   push ebx
   push ecx
   push edx

   ;загружаем файл переданный в параметрах
   mcall 68, 28, consoleModeSettings.current_params, dword[koFont.number]
   cmp eax, 0  ;если не удалось загрузить - выходим с ошибкой
   jne tool_iskef.n
   call printError
   jmp @f
.n:
   cmp edx, 0  ;выводим false, если файл пустой (его развер ноль и нет содержимого)
   je tool_iskef.false

   ;если же всё хорошо то проверяем заголовок
   mov bh, byte[eax]
   cmp bh, 'M'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, 'E'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, 'N'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, 'U'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, 'E'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, 'T'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, '0'
   jne tool_iskef.false

   inc eax
   mov bh, byte[eax]
   cmp bh, '1'
   jne tool_iskef.false

.true:
   mov ecx, 10
   mov esi, text_true.utf16le
   call addStringInHistory
   jmp @f
.false:
   mov ecx, 12
   mov esi, text_false.utf16le
   call addStringInHistory
@@:
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

tool_ls:
   call testText
   ret

tool_version:
   call printError
   ret
tool_man:
   call printError
   ret

;--------------------------------------------
;(с) Ефременков Сергей В., 2018