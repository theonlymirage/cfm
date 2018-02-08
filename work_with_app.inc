;Запуск приложения
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

start_application: ;esi - указатель на строку с именем проограммы
   push eax
   push ebx
   push esi
   mov dword[start_file_data.name], esi
   mcall 70, start_file_data
   cmp eax, 0
   ja @f       ;если eax>0, то успешно запущено
   call printError ;testPrintEAX  ; иначе печатаем ошибку - её код
@@:
   call printPID
   pop esi
   pop ebx
   pop eax
   ret

start_file_data:
    .subfunction dd   7
    .flags	 dd   0 ;если бит 0 установлен, то запустить процесс как отлаживаемый
    .param	 dd   file_params ; 0 или указатель на ASCIIZ-строку с параметрами (длина до 256 символов, оканчивается нулём)
    .rezerv	 dd   0  ;зарезервировано
    .rezerv1	 dd   0  ;зарезервировано
   ; .file_name   db   "/rd/1/TINYPAD",0
    db 0
    .name: dd file_name

align 4
file_params db 257 dup(0)   ;256, ещё один ноль на всякий случай
align 4
file_name: db "/rd/1/shell",0 ;tinypad",0 ;rb 4096


;получаем параметры из строки параметров текущей команды и устанавливаем/заполняем поле параметры в структуре запуска файла
setParamsFromCommandParam:
   push ecx
   push edi
   push esi
   ;проверим текущую кодировку параметров, если уже аски то ничего не надо делать
   mov cl, byte[consoleModeSettings.current_params_encoding]
   cmp cl, 1
   jne @f
   mov dword[start_file_data.param], consoleModeSettings.current_params
   jmp setParamsFromCommandParam.end
@@:
   mov esi, consoleModeSettings.current_params
   mov edi, file_params
   mov dword[start_file_data.param], edi
   mov ecx, 256   ;максимальное число переносимых символов
   call lengthStringUTF16LEtoASCIIZ
.end:
   pop esi
   pop edi
   pop ecx
   ret



;распечатать PID из eax
pid_start db 'P', 0, 'I', 0, 'D', 0, ':', 0, ' ', 0
pid_string db 22 dup(0)
printPID:
   cmp eax, 0
   jbe @f
   cmp eax, 10000000000000000000000000000000b ;429496
   jae printPID.error ; printPID.error	;jae @f

   push eax
   push ecx
   push esi

   mov ecx, eax
   mov esi, pid_string
   call numberToStringUTF16LE
   mov esi, pid_start
   mov ecx, 22
   call addStringInHistory

   pop esi
   pop ecx
   pop eax
   jmp @f
.error:
   call printError
@@:
   ret


;если не распознана команда, то нужно пробовать запускать файл
tool_execfile:
   push esi
   ;mov dword[start_file_data.param], 0 ;
   call setParamsFromCommandParam
   mov esi, consoleModeSettings.current_command_encoding ;file_name ;
   call start_application
   pop esi
   ret





;--------------------------------------------
;(с) Ефременков Сергей В., 2018