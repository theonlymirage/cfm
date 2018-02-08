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

; Режим консоли

consoleModeSettings:   ;настройки режима консоли
   .window_background_color dd 0;0x14ffffff

   .folder_color       dd 0
   .files_color        dd 0
   .application_color  dd 0
   .partition_color    dd 0 ;цвет раздела
   .console_text_color dd 0
   .console_user_command_text_color dd 0 ;цвет пользовательского текста - команды
   .warning_text_color dd 0

   .max_string_symbols_count dd 0
   .text_offset dd 0			 ;отступ между строками текста (разных блоков)
   .block_text_offset dd 0		 ;отступ между строками текста внутри одного блока (если текст переносится на несколько строк)

   .input_style db 0   ;каретка ввода: 1=вертикальный, 0=горизонтальный (подчёркивание снизу) или в виде 3=прямоугольника

   .data	dd 0   ;указатель на строки данных
   .data_offset dd 0   ;смещение данных вверх при отображении - текущая позиция - вообще вьювер надо использовать
   .data_count	dd 0   ;текущее кол-во блоков данных
   .data_max_count dd 0 ; максимальное кол-во сохраняемых в памяти блоков данных

   .command_text db 2048 dup (0) ;текст команды вводимый пользователем (ограничение 1024 символа)
   .command_pos  dd 0 ;текущая позиция ввода
   .current_command_encoding db 2
   .current_command db 2048 dup (0)
   .current_params_encoding db 2
   .current_params db 2048 dup (0)

   .echo_mode db 1   ;0=off (no command), 1=on(all), 2=only command(no result strings)

current_string_offset dd 0 ;смещение текущей печатываемой строки

consoleDataStringStruct:   ;блок данных
   .type    db 0       ;тип данных: команда пользователя, просто текст и тд (смотри описание дальше)
   .length  dd 19;0    ;размер данных
   .text    dd 0       ;указатель на данные


;consoleDataStringStruct2:   ;для теста от 17 мая
;   .type    db 0
;   .length  dd 0
;   .text    dd 0


console_files_count dd 0




;---
clearCurrentCommand: ;esi - указатель на строку, ebx - размер очищаемых данных
   push ebx
   push esi
   ;mov ebx, 2048 ;dword[consoleModeSettings.command_pos]
   ;mov esi, consoleModeSettings.current_command
 m1:
   cmp ebx, 0
   je @f
   mov byte[esi], 0
   inc esi
   dec ebx
   jmp m1
 @@:
   pop esi
   pop ebx
   ret

trimLeftLink:  ;сдвинуть указатель на UTF16LE строку на первый не пробельый символ, входные: esi - адрес строки
   push eax
   mov eax, 0
.cicle:
   mov ax, word[esi]
   cmp ax, 0
   je trimLeftLink.exit

   cmp ax, 9 ;табуляция
   je trimLeftLink.next

   cmp ax, 32 ;пробел
   je trimLeftLink.next

   cmp ax, 10 ;возврат каретки
   je trimLeftLink.next

   cmp ax, 13 ;перенос строки
   jne trimLeftLink.exit
.next:
   add esi, 2
   jmp trimLeftLink.cicle
.exit:
   pop eax
   ret


;получить парметры следующие за командой: esi - указатель на начало строки парметров
getParams:
   push ebx
   push esi

   ; очищаю предыдущее содержимое параметров
   push ebx
   push esi
   mov ebx, 2048
   mov esi, consoleModeSettings.current_params
   pop esi
   pop ebx

   call trimLeftLink
   mov edi, consoleModeSettings.current_params
   mov ebx, 0
.cicle:
   mov bx, word[esi]
   mov word[edi], bx
   cmp bx, 0
   je @f
   add esi, 2
   add edi, 2
   jmp getParams.cicle
 @@:
   push eax
   mov eax, dword[koFont.number]
   mov byte[consoleModeSettings.current_params_encoding], al
   pop eax

   pop esi
   pop ebx
   ret

;получить текущую команду из строки
getCommand:
   mov ebx, 2048
   mov esi, consoleModeSettings.current_command
   call clearCurrentCommand;очищаем предыдущее содержимое
   push esi
   push edi
   push eax

   mov eax, 0
   mov esi, consoleModeSettings.command_text
   call trimLeftLink
   mov edi, consoleModeSettings.current_command
.c:
   mov ax, word[esi]
   cmp ax, 32  ;всё до первого пробела считается командой
   jne @f
   mov ax, 0
@@:
   mov word[edi], ax
   cmp ax, 0
   je @f
   add esi, 2
   add edi, 2
   jmp getCommand.c
@@:
   call getParams
   pop eax
   pop edi
   pop esi
   ret
;---
testText: ;здесь был тестовый вывод содержимого директории
   call setCurrentEncoding
   push esi
   ;call getCurrentDir
   mov esi, consoleModeSettings.current_params_encoding ;current_dir_encoding
   call setReadPath
   pop esi

   push eax
   mov eax, 0
   mov dword[console_files_count], 0
.next_block:
   call readBlock
   push edx
   mov edx, dword[read_folder_return_struct + 8]
   mov dword[console_files_count], edx
   pop edx

   push eax
   push ebx
   call addStringFileData; обработать и добавить новый блок во вьювер текущего отобрражения (не забываем обновить контент)
   pop ebx
   pop eax

   inc eax
   cmp eax, dword[console_files_count]
   jb testText.next_block
   pop eax
   ret


;---
clearCommandText:   ;очищаем поле ввода команды
   push ebx
   push esi
   mov ebx, dword[consoleModeSettings.command_pos]
   mov esi, consoleModeSettings.command_text
.m1:
   cmp ebx, 0
   je @f
   mov byte[esi], 0
   inc esi
   dec ebx
   jmp clearCommandText.m1
 @@:
   pop esi
   pop ebx
   mov dword[consoleModeSettings.command_pos], 0 ;обнуляем позицию ввода
   ret

printHeader:
   mov ecx, 40 ;2048
      mov esi, text_application_title.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 0 ;это текст
      call addInHistory

   mov ecx, 46 ;2048
      mov esi, text_application_version.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 0 ;это текст
      call addInHistory
   ret
printError:  ;распечатать код ошибки
  ;добавлю строку с выводом ошибки
   mov ecx, 30
      mov esi, text_unknown_error.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 2 ;это текст предупреждения об ошибке
      call addInHistory
   ret
printParams: ;распечатать параметры текущей команды
   mov ecx, 2048
      mov esi, consoleModeSettings.current_params
      call createStringHistory
      mov eax, 0
      mov al, 0 ;это текст
      call addInHistory
   ret
printCurrentCommand: ;распечатаьть текущую команду
mov ecx, 2048
      mov esi, consoleModeSettings.current_command
      call createStringHistory
      mov eax, 0
      mov al, 0 ;это текст
      call addInHistory
   ret


commandInterpretor:   ;интерпретатор команд
  call getCommand ;получаем текущую команду
  ;call printCurrentCommand
  ;call printParams
  ; call testText

  push eax
  mov eax, 0
  mov esi, consoleModeSettings.current_command

  ;если это команда echo или print, то просто печатаем эту строку
  mov edi, command_listUTF16LE.print
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call printParams
  jmp commandInterpretor.end
@@:
  mov esi, consoleModeSettings.current_command
  mov edi, command_listUTF16LE.echo
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call printParams
  jmp commandInterpretor.end
@@:  ;если команды выхода - завершения работы программы
  mov esi, consoleModeSettings.current_command
  mov edi, command_listUTF16LE.exit
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call consoleMode.deinit
  mov eax, -1		       ;закрыть эту программу
  mcall
  jmp commandInterpretor.end
@@:  ;отобразить текущую рабочую директорию
  mov edi, command_listUTF16LE.pwd
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_pwd
  jmp commandInterpretor.end
@@: ;смена каталога (Change Directory)
  mov edi, command_listUTF16LE.cd
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cd
  jmp commandInterpretor.end
@@: ;очистить экран консоли
  mov edi, command_listUTF16LE.cls
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cls
  jmp commandInterpretor.end
@@: ;очистить экран консоли
  mov edi, command_listUTF16LE.clear
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cls
  jmp commandInterpretor.end
@@:  ;добавить/включить вывод команд в истории
  mov edi, command_listUTF16LE.echo_on
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_echo_on
  jmp commandInterpretor.end
@@: ;убрать/отключить вывод команд в истории
  mov edi, command_listUTF16LE.echo_off
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_echo_off
  jmp commandInterpretor.end
@@:  ; пауза в секундах (разбудить нельзя)
  mov edi, command_listUTF16LE.pause
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_pause
  jmp commandInterpretor.end
@@: ;
  mov edi, command_listUTF16LE.ls
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_ls
  jmp commandInterpretor.end
@@: ;удалить пустую папку/директорию
  mov edi, command_listUTF16LE.rm
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_rm
  jmp commandInterpretor.end
@@: ;создать папку/директорию
  mov edi, command_listUTF16LE.md
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_mkdir
  jmp commandInterpretor.end
@@:  ;создать папку/директорию
  mov edi, command_listUTF16LE.mkdir
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_mkdir
  jmp commandInterpretor.end
@@: ;является ли файл исполняемым файлом системы
  mov edi, command_listUTF16LE.iskef
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_iskef
  jmp commandInterpretor.end
@@: ;минимизировать все окна
  mov edi, command_listUTF16LE.minimize_all_windows
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_minimize_all_windows
  jmp commandInterpretor.end
@@: ; установить курсор мыши в заданную позицию, по умолчанию: по центру экрана
  mov edi, command_listUTF16LE.set_mouse_position
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_position
  jmp commandInterpretor.end
@@:  ; вызов из командного файла подпрограмм или других командных файлов
  mov edi, command_listUTF16LE.call
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_run
  jmp commandInterpretor.end
@@: ;
  mov edi, command_listUTF16LE.run
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_run
  jmp commandInterpretor.end
@@:  ;
  mov edi, command_listUTF16LE.date
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_date
  jmp commandInterpretor.end
;@@: ;
;  mov edi, command_listUTF16LE.version
;  call compareStringsUTF16LE
;  cmp eax, 0
;  jne @f
;  call tool_version
;  jmp commandInterpretor.end
@@: ; распечатать список команд
  mov edi, command_listUTF16LE.help
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_help
  jmp commandInterpretor.end
;@@:  ;
;  mov edi, command_listUTF16LE.man
;  call compareStringsUTF16LE
;  cmp eax, 0
;  jne @f
;  call tool_man
;  jmp commandInterpretor.end
@@: ; перезагрузка системы
  mov edi, command_listUTF16LE.restart_kernel
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_restart_kernel
  jmp commandInterpretor.end
@@:; выключение системы
  mov edi, command_listUTF16LE.shutdown
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_shutdown
  jmp commandInterpretor.end
@@: ;извлечь лоток привода диска (не тестировалось)
  mov edi, command_listUTF16LE.eject
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_eject
  jmp commandInterpretor.end
@@: ; получить yомер слота процесса/потока по его идентификатору
  mov edi, command_listUTF16LE.pslot
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_pslot
  jmp commandInterpretor.end
@@: ; получить чувствительность мыши текстом
  mov edi, command_listUTF16LE.get_mouse_sensitivity
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_mouse_sensitivity
  jmp commandInterpretor.end
@@: ; задать новую чувствительность мыши
  mov edi, command_listUTF16LE.set_mouse_sensitivity
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_sensitivity
  jmp commandInterpretor.end
@@: ; получить скорость мыши текстом
  mov edi, command_listUTF16LE.get_mouse_speed
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_mouse_speed
  jmp commandInterpretor.end
@@: ; задать новую скорость мыши
  mov edi, command_listUTF16LE.set_mouse_speed
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_speed
  jmp commandInterpretor.end
@@: ; отобразить кол-во всей доступной ОЗУ в Кбайтах
  mov edi, command_listUTF16LE.get_ram_size
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_ram_size
  jmp commandInterpretor.end
@@: ; отобразить кол-во свободной ОЗУ в Кбайтах
  mov edi, command_listUTF16LE.get_free_ram
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_free_ram
  jmp commandInterpretor.end
@@: ; вывести версию ядра
  mov edi, command_listUTF16LE.get_core_version
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_core_version
  jmp commandInterpretor.end
@@: ; завершить процесс/поток по номеру слота
  mov edi, command_listUTF16LE.terminate
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_terminate
  jmp commandInterpretor.end
@@: ; установить режим отрисовки фона: 1=замостить, 2=растянуть
  mov edi, command_listUTF16LE.background_set_mode
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_background_set_mode
  jmp commandInterpretor.end
@@: ; перерисовать фон рабочего стола
  mov edi, command_listUTF16LE.background_redraw
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_background_redraw
  jmp commandInterpretor.end
@@: ; установить режим отрисовки окна ФМ
  mov edi, command_listUTF16LE.set_mode
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mode
  jmp commandInterpretor.end
@@: ; отобразить системную дату
  mov edi, command_listUTF16LE.date
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_date
  jmp commandInterpretor.end
@@: ; отобразить системное время
  mov edi, command_listUTF16LE.time
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_time
  jmp commandInterpretor.end
@@: ; перезагрузка системы
  mov edi, command_listUTF16LE.reboot
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_reboot
  jmp commandInterpretor.end
@@: ;завершить процесс/поток по uid
  mov edi, command_listUTF16LE.kill
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_kill
  jmp commandInterpretor.end
@@: ;отобразить текущую цветовую схему
  mov edi, command_listUTF16LE.current_color_scheme
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_current_color_scheme
  jmp commandInterpretor.end
@@: ;отобразить количество активных сетевых устройств
  mov edi, command_listUTF16LE.get_network_device_count
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_network_device_count
  jmp commandInterpretor.end
@@: ;получить тип сетевого устройства
  mov edi, command_listUTF16LE.get_network_device_type
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_network_device_type
  jmp commandInterpretor.end
;@@: ;получить IP
;  mov edi, command_listUTF16LE.ipconfig
;  call compareStringsUTF16LE
;  cmp eax, 0
;  jne @f
;  call tool_get_ip
;  jmp commandInterpretor.end
@@:
  ;если ни с чем не совпало то пробуем выполнить
  call tool_execfile
.end:
  pop eax
  ret
;---
deleteLastSymbol:
   push ebx
   push esi
   mov ebx, dword[consoleModeSettings.command_pos] ;получаем текущую позицию ввода
   cmp ebx, 0  ; все символы удалены (больше нечего удалять)
   jbe @f
   sub ebx, 2
   mov dword[consoleModeSettings.command_pos], ebx
   mov esi, consoleModeSettings.command_text
   add esi, ebx
   mov word[esi], 0 ;byte[esi], 0
 @@:
   pop esi
   pop ebx
   ret




kirilicToASCII: ;al преобразование кода
   cmp al, 0x51 ;ё
   jne @f
   mov al, 241
   jmp kirilicToASCII.end
@@:
   cmp al, 0x01 ;Ё
   jne @f
   mov al, 240
   jmp kirilicToASCII.end
@@:
   cmp al, 0x10 ;от А
   jb @f
   cmp al, 0x2F ;до Я
   ja @f
   sub al, 0x10
   add al, 128
   jmp kirilicToASCII.end
@@:
   cmp al, 0x30 ;от а
   jb @f
   cmp al, 0x3F ;до п
   ja @f
   sub al, 0x30
   add al, 160
   jmp kirilicToASCII.end
@@:
   cmp al, 0x40 ;от р
   jb @f
   cmp al, 0x4F ;до я
   ja @f
   sub al, 0x40
   add al, 224
   jmp kirilicToASCII.end
@@:

.end:
   ret

addRusSymbol: ;добавить кириллистический символ в строку utf16le : ah преобразование кода
   cmp ah, 241 ;ё
   jne @f
   mov ah, 0x51
   jmp addRusSymbol.end
@@:
   cmp ah, 240 ;Ё
   jne @f
   mov ah, 0x01
   jmp addRusSymbol.end
@@:
   cmp ah, 128	  ;от А
   jb @f
   cmp ah, 159	  ;до Я
   ja @f
   sub ah, 128
   add ah, 0x10
   jmp addRusSymbol.end
@@:
   cmp ah, 160	  ;от а
   jb @f
   cmp ah, 175	  ;до п
   ja @f
   sub ah, 160
   add ah, 0x30
   jmp addRusSymbol.end
@@:
   cmp ah, 224	  ;от р
   jb @f
   cmp ah, 239	  ;до я
   ja @f
   sub ah, 224
   add ah, 0x40
   jmp addRusSymbol.end
@@:
.end:
   ret

insertLastSymbol:
   push eax
   mov al, 0
   cmp ah, 128	;проверяем симол кириллистический или нет
   jb @f       ;если нет то переходим на добавление обычного символа
   call addRusSymbol	;иначе преобразуем код русского символа
   mov al, 0x04
@@:
   push ebx
   push esi
   mov ebx, dword[consoleModeSettings.command_pos] ;получаем текущую позицию ввода
   cmp ebx, 2048-2    ;лимит заполнение превышен(больше символов ввести нельзя)
   jae @f
   mov esi, consoleModeSettings.command_text
   add esi, ebx
   mov byte[esi], ah
   inc esi
   mov byte[esi], al ;0
   inc esi
   mov word[esi], 0
   add ebx, 2
   mov dword[consoleModeSettings.command_pos], ebx
 @@:
   pop esi
   pop ebx
   pop eax
   ret

;---

consoleMode:
   .update_history_string_count:
      push eax
      mov eax, dword[windowSettings.height]
      call koFont.getStringCount
      ;call changeHistorySize
      ;mov dword[consoleModeSettings.data_max_count], eax ;14	;максимальное кол-во строк данных в истории
      pop eax
      ret
   .init:
      push eax
      mov eax, 2	;используем UTF 16 LE
      call koFont.init	;инициализируем шрифт
      pop eax				   ;A0 вместо 20 из-за косяка в новой версии
      mov dword[consoleModeSettings.console_text_color], 0xA0b3b3b3 ;0x14FFFFFF
      mov dword[consoleModeSettings.folder_color], 0xA000FF00
      mov dword[consoleModeSettings.files_color], 0xA0FFFFFF
      mov dword[consoleModeSettings.application_color], 0xA00000FF
      mov dword[consoleModeSettings.warning_text_color], 0xA0ff8090
      mov dword[consoleModeSettings.console_user_command_text_color], 0xA027a327 ;004d00 ;00FF00
      mov dword[consoleModeSettings.partition_color], 0xA07019c2

      mov dword[applicationSettings.window_background_color], 0x14000000
      mov dword[consoleModeSettings.text_offset], 5
      mov dword[current_string_offset], 30

      push eax
      mov eax, dword[windowSettings.height]
      call koFont.getStringCount
      mov dword[consoleModeSettings.data_max_count], 14   ;максимальное кол-во строк данных в истории
      pop eax
      call initHistory ;инициализируем блоки истории и выделяем под них память

      mov byte[consoleModeSettings.echo_mode], 1 ;отображать всё
      call initCurrentDir
      call printHeader
      ret
   .mouse:
      ret

   .keyboard_interpretator:
      cmp ah, 13    ;enter
      jne @f

      ;добавляю строку запроса в историю
      push ecx
      push esi
      push eax

      mov ecx, 0
      mov cl, byte[consoleModeSettings.echo_mode]
      cmp cl, 0   ;если отключено echo команд
      je .keyboard_interpretator_1

      mov ecx, dword[consoleModeSettings.command_pos]
      add ecx, 2
      mov esi, consoleModeSettings.command_text
      call createStringHistory
      mov eax, 0
      mov al, 1 ;это СКРИПТОВЫЙ текст запроса
      call addInHistory
   .keyboard_interpretator_1:
      pop eax
      pop esi
      pop ecx

      call commandInterpretor
      call clearCommandText
      call draw_window ;call red ;вызываем перерисовку
      ret
   @@:
      cmp ah, 10    ;игнорирую перенос картеки
      je @f
      call insertLastSymbol;добавляем нажатый символ в строку
      ;call draw_window ;call red ;вызываем перерисовку
   @@:
      ret

   .keyboard:
      cmp     ah, 27  ;escape
      jne     @f
	call consoleMode.deinit
	mov	eax,-1			; Функция -1 : закрыть эту программу
	mcall
    @@:
      cmp ah, 8   ;delete=8
      jne @f
      call deleteLastSymbol  ;удаляем из строки последний введённый символ
      call red ;вызываем перерисовку
      ret
    @@:
      cmp ah, 182   ;backspace=182
      jne @f
      call deleteLastSymbol  ;удаляем из строки последний введённый символ
      call red ;вызываем перерисовку
      ret
    @@:
      cmp ah, 13    ;enter
      jne @f

      ;добавляю строку запроса в историю
      push ecx
      push esi
      push eax

      mov ecx, 0
      mov cl, byte[consoleModeSettings.echo_mode]
      cmp cl, 0   ;если отключено echo команд
      je .keyboard_interpretator_2

      mov ecx, dword[consoleModeSettings.command_pos]
      add ecx, 2
      mov esi, consoleModeSettings.command_text
      call createStringHistory
      mov eax, 0
      mov al, 1 ;это пользовательский текст запроса
      call addInHistory
   .keyboard_interpretator_2:
      pop eax
      pop esi
      pop ecx

      ;call clearCommandText
      call commandInterpretor
      call clearCommandText
      call red ;вызываем перерисовку
      ret
    @@:
      call insertLastSymbol;добавляем нажатый символ в строку:
      call red ;вызываем перерисовку
      ret
   .hotkey:
      ret
   .draw:
      call updateConsoleStringSymbolsCount ;обновить кол-во символов вмещаемых в консоле
      mov dword[current_string_offset], 30 ;смещение вывода текста относительно верха окна

;      mov dword[consoleDataStringStruct.text], text_application_title.utf16le
;      mov eax, 0
;      mov byte[consoleDataStringStruct.type], al
;      mov esi, consoleDataStringStruct
;      call consoleMode_printData

;      mov dword[consoleDataStringStruct.text], text_application_version.utf16le
;      mov eax, 0
;      mov al, 0
;      mov byte[consoleDataStringStruct.type], al
;      mov esi, consoleDataStringStruct
;      call consoleMode_printData

      call printHistoryContent ;распечатать историю предыдущих запросов

      ;в самом конце печатаем текущую строку ввода пользователя
      mov dword[consoleDataStringStruct.text], consoleModeSettings.command_text
      mov eax, dword[consoleModeSettings.command_pos]
      mov dword[consoleDataStringStruct.length], eax
      mov eax, 0
      mov al, 1
      mov byte[consoleDataStringStruct.type], al
      mov esi, consoleDataStringStruct
      call consoleMode_printData

      ret
   .deinit:
      call deinitAllSlotHistory ;освобождаем дополнительную память - строки истории
      call deinitHistory ;удаляем блоки истории
      ;mov dword[applicationSettings.window_background_color], 0x14FFFFFF
      ret



;временный вывод ls
addStringFileData:
   mov eax, read_folder_return_struct.block + 40
   mov dword[consoleDataStringStruct.text], eax

;раздел
mov ebx, dword[read_folder_return_struct.block]
;shr ebx, 3
;shl ebx, 31
;shr ebx, 31
;cmp al, 1
;jne @f
bt ebx, 3
jnc @f
 mov al, 6
 jmp .ress

@@:;папка
mov ebx, dword[read_folder_return_struct.block]
;shr ebx, 4
;shl ebx, 31
;shr ebx, 31
;cmp al, 1
;jne @f
bt ebx, 4
jnc @f
 mov al, 3
 jmp .ress

;файл
@@:
mov ebx, dword[read_folder_return_struct.block]
shr ebx, 5
shl ebx, 31
shr ebx, 31
cmp al, 1
jne @f
 mov al, 4
 jmp .ress

@@:
   mov al, 8 ;7
.ress:
;;   mov byte[consoleDataStringStruct.type], al
;;   mov dword[consoleDataStringStruct.length], 40
;;   mov esi, consoleDataStringStruct

;из-за косяка в новых версиях везде исправил 20 на A0
      mov dword[consoleModeSettings.console_text_color], 0xA0b3b3b3 ;0x14FFFFFF
      mov dword[consoleModeSettings.folder_color], 0xA000FF00
      mov dword[consoleModeSettings.files_color], 0xA0FFFFFF
      mov dword[consoleModeSettings.application_color],  0xA00000FF
      mov dword[consoleModeSettings.warning_text_color], 0xA0ff8090
      mov dword[consoleModeSettings.console_user_command_text_color], 0xA027a327 ;004d00 ;00FF00
      mov dword[consoleModeSettings.partition_color], 0xA07019c2

   push ecx
   push esi
      mov ecx, 520 ;40	 ;cp866 = 264
      mov esi, read_folder_return_struct.block + 40 ;consoleDataStringStruct.text
      call createStringHistory
      call addInHistory
   pop esi
   pop ecx

;;   call consoleMode_printData
   ret




;обновить кол-во символов влазиющих в строку вывода консоли при текущем шрифте
updateConsoleStringSymbolsCount:
   push eax
   mov eax, [windowSettings.cwidth]   ;размер вьювера консоли
   sub eax, 15 ;отступ для скрола
   sub eax, dword[koFont.symbol_width] ;минус два символа отступа слева, может быть для других языков это справа (но я пока их не учитываю)
   sub eax, dword[koFont.symbol_width]
   call koFont.getSymbolsCount
   mov dword[consoleModeSettings.max_string_symbols_count], eax
   pop eax
   ret


;печать данных
consoleMode_printData:	;в esi - указатель на блок данных
      push eax
      push ebx
      push ecx
      push edx
      push esi
      mov  ebx, 28

;в зависимости от типа данных - цвет текста и смещение
      mov eax, 0
      mov al, byte [esi]

      cmp al, 0    ;если 0 - это обычный текст
      jne @f
      mov     ecx, dword[consoleModeSettings.console_text_color]
      jmp .finish
   @@:
      cmp al, 1   ;если 1 - это пользовательский текст
      jne @f
      mov  ecx, dword[consoleModeSettings.console_user_command_text_color]
;      sub ebx, dword[fonts.symbol_width]
;;	call printPreString
      jmp .finish
   @@:
      cmp al, 2   ;если 2 то это предупреждение
      jne @f
      mov     ecx, dword[consoleModeSettings.warning_text_color]
      jmp .finish
   @@:
      cmp al, 3   ;если 3 то это папка
      jne @f
      mov     ecx, dword[consoleModeSettings.folder_color]
      jmp .finish

   @@:
      cmp al, 4   ;если 4 то это файл
      jne @f
      mov     ecx, dword[consoleModeSettings.files_color]
      jmp .finish

   @@:
      cmp al, 5   ;если 5 то это приложение
      jne @f
      mov     ecx, dword[consoleModeSettings.application_color]
      jmp .finish

   @@:
      cmp al, 6   ;если 6 то это раздел
      jne @f
      mov     ecx, dword[consoleModeSettings.partition_color]
      jmp .finish

   @@:
      cmp al, 7   ;если 7 - то это команда пользователя без начальных символов строки
      jne @f
      mov     ecx, dword[consoleModeSettings.console_user_command_text_color]
      jmp .finish

   @@: ;иначе просто белым рисую - ибо непонятное что-то
      mov     ecx, dword[consoleModeSettings.console_text_color]
.finish:
   push eax


   shl ebx, 16
   sub ebx, dword[consoleModeSettings.data_offset] ;10 = 01, 20 = 10, 30 = 11
   mov eax, dword[current_string_offset]
   add ebx, eax
   add eax, dword[koFont.symbol_height]
   add eax, dword[consoleModeSettings.text_offset]
   mov dword[current_string_offset], eax		      ;dword[koFont.number]
.end:
      mov     edx, dword[esi+5]  ;указатель на текст
      mov     esi, dword[esi+1]  ;кол-вл символов
      mov     eax, 4
      mcall
pop eax

cmp al, 1   ;предстрока - она занимает два символа перед строками
jne @f
      mov ebx, 28
      sub ebx, dword[koFont.symbol_width]
      sub ebx, dword[koFont.symbol_width]
      sub ebx, 2
      shl ebx, 16
      sub ebx, dword[consoleModeSettings.data_offset]
      mov eax, dword[current_string_offset]
      sub eax, dword[koFont.symbol_height]
   sub eax, dword[consoleModeSettings.text_offset]
      add ebx, eax
      mov     ecx, dword[consoleModeSettings.console_user_command_text_color]
      mov dword[consoleDataStringStruct.text], text_user_input.utf16le
      mov edx, dword[consoleDataStringStruct.text] ;user_input.utf
      mov     esi, 2  ;кол-во символов
      mov     eax, 4
      mcall
@@:
      pop esi
      pop edx
      pop ecx
      pop ebx
      pop eax
      ret


;изменить размер максимального числа строк истории
;вход: eax - новое число строк
offset_data_byte dd 0
old_history_blocks_count dd 0
skip_count dd 0
old_adress dd 0
changeHistorySize:
   push eax
   push ebx
   push ecx
   push edx
   push esi
   push edi

mov ebx, dword[consoleModeSettings.data_count]	  ;если текущее число строк больше чем будет, то установим максимум
mov ecx, ebx ;сохраним в ecx - кол-во блоков в старой истории
mov dword[old_history_blocks_count], ecx
cmp ebx, eax
jbe @f
mov ebx, eax
@@:
push ebx

    ;сохраним указатель на старую историю
    mov edi, dword[consoleModeSettings.data]
    mov dword[old_adress], edi
    push edi
    push ecx

    ;создадим новую структуру истории
    mov dword[consoleModeSettings.data_max_count], eax
    call initHistory
    mov esi, eax

    ;перекопируем конец прошлой истории в новую
    pop ecx
    pop edi
    ;узнаем сколько блоков с начала старой истории нужно пропустить и положим в edx
    mov edx, 0
    cmp ecx, dword[consoleModeSettings.data_max_count]
    jbe @f
    mov edx, ecx
    sub edx, dword[consoleModeSettings.data_max_count]
@@:
    mov dword[skip_count], edx
    push edi
    ;тут вычислим смещение адреса
    push eax
    push edx
    push ebx
    mov ebx, edx
    mov edx, 0
    mov eax, 9 ;это размер 1 структуры
    mul ebx
    mov dword[offset_data_byte], eax
    pop ebx
    pop edx
    pop eax

    add edi, dword[offset_data_byte] ;берём указатель на начало копируемых данных из старой структуры
    mov edx, dword[old_history_blocks_count]
.copy: ;копируем
    cmp edx, 0
    je @f
    push eax
    mov eax, 0
    mov al, byte[edi]
    mov byte[esi], al

    mov eax, dword[edi+1]
    mov dword[esi+1], eax

    mov eax, dword[edi+5]
    mov dword[esi+5], eax
    pop eax
    add edi, 9
    add esi, 9
    dec edx
    jmp changeHistorySize.copy
@@:
    ;очистим память ненужных строк старой истории
    push eax
   push ebx
   push ecx
   push esi
   push edx
   mov edx, dword[skip_count]
   mov esi, dword[old_adress]
   add esi, 5
.cicle:
   cmp edx, 0
   je @f
   mov ecx, dword[esi]
   mcall 68, 13
   dec edx
   add esi, 9
   jmp changeHistorySize.cicle
@@:
   pop edx
   pop esi
   pop ecx
   pop ebx
   pop eax


    ;освободим память старой истории
    pop edi

    push eax
    push ebx
    push ecx
    mov ecx, edi
    mcall 68, 13
    pop ecx
    pop ebx
    pop eax

pop ebx
mov dword[consoleModeSettings.data_count], ebx
   pop edi
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

;выделяем память под нужное число блоков данных
initHistory:
   mov dword[consoleModeSettings.data_count], 0
   push eax
   push ebx
   push ecx
   push edx
   mov eax, dword[consoleModeSettings.data_max_count] ;dword[consoleModeSettings.data_count]
   cmp eax, 0
   je @f
   mov ebx, 9
   mul ebx ; 9 это размер одного блока данных
   mov ecx, eax
   mcall 68, 12
   mov dword[consoleModeSettings.data], eax
@@:
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

;освобождаем память блоков данных (но не их содержимого)
deinitHistory:
   push eax
   push ebx
   push ecx
   mov ecx, dword[consoleModeSettings.data]
   mcall 68, 13
   pop ecx
   pop ebx
   pop eax
   ret

;освобождаем память первого слота
deinitFirstSlotHistory:
   push eax
   push ebx
   push ecx
   push esi
   mov esi, dword[consoleModeSettings.data]
   add esi, 5
   mov ecx, dword[esi]
   mcall 68, 13
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

;освобождаем память всех слотов истории
deinitAllSlotHistory:
   push eax
   push ebx
   push ecx
   push esi
   push edx
   mov edx, dword[consoleModeSettings.data_count]
   mov esi, dword[consoleModeSettings.data]
   add esi, 5
.cicle:
   cmp edx, 0
   je @f
   mov ecx, dword[esi]
   mcall 68, 13
   dec edx
   add esi, 9
   jmp deinitAllSlotHistory.cicle
@@:
   pop edx
   pop esi
   pop ecx
   pop ebx
   pop eax
   ret

;сдвинуть все ячейки на 1 вверх и обнулить последнюю
swapHistory:
   push eax
   push ecx
   push edi
   push esi
   mov ecx, dword[consoleModeSettings.data_max_count]
   dec ecx
   mov esi, dword[consoleModeSettings.data]
   mov edi, esi
   add esi, 9
 c1:
   cmp ecx, 0
   je @f
   ;mov eax, 0

   mov al, byte[esi]
   mov byte[edi], al

   mov eax, dword[esi+1]
   mov dword[edi+1], eax

   mov eax, dword[esi+5]
   mov dword[edi+5], eax

   add esi, 9
   add edi, 9
   dec ecx
   jmp c1
@@:
   mov eax, dword[consoleModeSettings.data_count]
   cmp eax, 0
   je @f
   dec eax
   mov dword[consoleModeSettings.data_count], eax
@@:
;   mov byte[esi], 0
;   mov dword[esi+1], 0
;   mov dword[esi+5], 0
   pop esi
   pop edi
   pop ecx
   pop eax
   ret

;добавить строку в историю
;входные: al -тип строки, ecx - размер строки в байтах, esi - указатель на строку
addInHistory:
   push eax
   push ecx
   push esi
   push edi
   push edx

   ;проверяем есть ли свободные слоты
   mov edx, dword[consoleModeSettings.data_count]
   cmp edx, dword[consoleModeSettings.data_max_count]
   jl @f ;если есть то переходим на них
   push ecx
   push esi
   call deinitFirstSlotHistory; иначе удаляем первый блок
   call swapHistory;и делаем сдвиг с затиранием
   ;след два действия уже есть в свапе
   ;dec eax;уменьшаем счётчик занятых блоков на 1
   ;mov dword[consoleModeSettings.data_count], eax
   pop esi
   pop ecx
@@:
   ;находим первый свободный слот
   mov edi, dword[consoleModeSettings.data]
   push eax
   push edx
   mov eax, dword[consoleModeSettings.data_count]
   push ebx
   mov ebx, 9
   mul ebx
   pop ebx
   add edi, eax
   pop edx
   pop eax

   ;записываем туда данные
   mov byte[edi], al
   mov dword[edi+1], ecx
   mov dword[edi+5], esi

   mov eax, dword[consoleModeSettings.data_count]
   inc eax
   mov dword[consoleModeSettings.data_count], eax

   pop edx
   pop edi
   pop esi
   pop ecx
   pop eax
   ret

; распечатать всю историю запросов пользователя
printHistoryContent:
   push eax
   push esi
   mov eax, dword[consoleModeSettings.data_count]
   mov esi, dword[consoleModeSettings.data]
 m12:
   cmp eax, 0
   je @f
   call consoleMode_printData
   dec eax
   add esi, 9
   jmp m12
@@:
   pop esi
   pop eax
   ret


;выделить память под строку и скопировать её данные туда
;входные: ecx - размер строки в байтах, esi - адрес строки
;выходные: esi - новый адрес копии строки
createStringHistory:
   push eax
   push ebx
   push ecx
   push edi
   mov edi, 0
   cmp ecx, 0
   je @f
   mcall 68, 12
   mov edi, eax
   mov eax, 0
   push edi
.c:
   cmp ecx, 0
   je @f
   mov al, byte[esi]
   mov byte[edi], al
   dec ecx
   inc esi
   inc edi
   jmp createStringHistory.c
@@:
   pop edi
   mov esi, edi
   pop edi
   pop ecx
   pop ebx
   pop eax
   ret





;--
;сравнение двух utf16le строк
;входные: esi - указатель на первую строку, edi - указатель на вторую строку
;выходные: eax - 0 строки одинаковы или номер первого отличного символа
compareStringsUTF16LE:
   mov eax, 0
   push ebx
   push edi
   push esi
.start:
   mov bx, word[esi]
   inc eax
   cmp bx, word[edi]   ;если символы не совпали, то мы нашли позицию не совпадения
   jne compareStringsUTF16LE.end
   cmp bx, 0  ;иначе если строки были равны и символ с кодом 0, то это конец обеих строк
   je @f
   add esi, 2
   add edi, 2
   jmp compareStringsUTF16LE.start
@@:
   mov eax, 0
.end:
   pop esi
   pop edi
   pop ebx
   ret


;сложение/конкатенация двух utf16le строк
;входные: esi - указатель на первую строку, edi - указатель на вторую строку
;eax - указатель на новую строку, содержающую результат
concatStringsUTF16LE:
   ret

;--------------------------------------------
;(с) Ефременков Сергей В., 2018