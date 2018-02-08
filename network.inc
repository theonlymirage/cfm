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


;network
device_count_text db 22 dup(0)
device_name_buff db 64 dup (0)

;Получить количество активных сетевых устройств
tool_get_network_device_count:
   push eax
   push ebx
   mcall 74, -1
   mov ecx, eax
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory
   pop ebx
   pop eax
   ret


;Получить тип сетевого устройства
tool_get_network_device_type:
   push eax
   push ebx
   mov esi, consoleModeSettings.current_params
   call trimLeftLink
   call strUTF16LEtoInteger

;тут нужно проверить cmp ecx, dword[device_count]
;если больше чем кол-во устройств, то вывести ошибку

   mov ebx, 0
   mov bh, cl
   mov bl, 0
   mcall 74
   mov ecx, eax
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory
   pop ebx
   pop eax
   ret


;Получить имя сетевого устройства НЕ СДЕЛАНО
tool_get_network_device_name:
   mov bl, 1
   mcall 74
   ret


;прочитать IP-адрес
tool_get_ip:
   push eax
   push ebx
   mov ebx, 0
   mov bh, 1
   mov bl, 2
   mcall 76

   mov ecx, ebx
   and ecx, 0x000000FF
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory

   mov ecx, ebx
   and ecx, 0x0000FF00
   shr ecx, 8
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory

   mov ecx, ebx
   and ecx, 0x00FF0000
   shr ecx, 16
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory

   mov ecx, ebx
   and ecx, 0xFF000000
   shr ecx, 24
   mov esi, device_count_text
   call numberToStringUTF16LE
   mov ecx, 22
   call addStringInHistory

   pop ebx
   pop eax
   ret







;--------------------------------------------
;(с) Ефременков Сергей В., 2018