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

; ����� �������

consoleModeSettings:   ;��������� ������ �������
   .window_background_color dd 0;0x14ffffff

   .folder_color       dd 0
   .files_color        dd 0
   .application_color  dd 0
   .partition_color    dd 0 ;���� �������
   .console_text_color dd 0
   .console_user_command_text_color dd 0 ;���� ����������������� ������ - �������
   .warning_text_color dd 0

   .max_string_symbols_count dd 0
   .text_offset dd 0			 ;������ ����� �������� ������ (������ ������)
   .block_text_offset dd 0		 ;������ ����� �������� ������ ������ ������ ����� (���� ����� ����������� �� ��������� �����)

   .input_style db 0   ;������� �����: 1=������������, 0=�������������� (������������� �����) ��� � ���� 3=��������������

   .data	dd 0   ;��������� �� ������ ������
   .data_offset dd 0   ;�������� ������ ����� ��� ����������� - ������� ������� - ������ ������ ���� ������������
   .data_count	dd 0   ;������� ���-�� ������ ������
   .data_max_count dd 0 ; ������������ ���-�� ����������� � ������ ������ ������

   .command_text db 2048 dup (0) ;����� ������� �������� ������������� (����������� 1024 �������)
   .command_pos  dd 0 ;������� ������� �����
   .current_command_encoding db 2
   .current_command db 2048 dup (0)
   .current_params_encoding db 2
   .current_params db 2048 dup (0)

   .echo_mode db 1   ;0=off (no command), 1=on(all), 2=only command(no result strings)

current_string_offset dd 0 ;�������� ������� ������������ ������

consoleDataStringStruct:   ;���� ������
   .type    db 0       ;��� ������: ������� ������������, ������ ����� � �� (������ �������� ������)
   .length  dd 19;0    ;������ ������
   .text    dd 0       ;��������� �� ������


;consoleDataStringStruct2:   ;��� ����� �� 17 ���
;   .type    db 0
;   .length  dd 0
;   .text    dd 0


console_files_count dd 0




;---
clearCurrentCommand: ;esi - ��������� �� ������, ebx - ������ ��������� ������
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

trimLeftLink:  ;�������� ��������� �� UTF16LE ������ �� ������ �� ��������� ������, �������: esi - ����� ������
   push eax
   mov eax, 0
.cicle:
   mov ax, word[esi]
   cmp ax, 0
   je trimLeftLink.exit

   cmp ax, 9 ;���������
   je trimLeftLink.next

   cmp ax, 32 ;������
   je trimLeftLink.next

   cmp ax, 10 ;������� �������
   je trimLeftLink.next

   cmp ax, 13 ;������� ������
   jne trimLeftLink.exit
.next:
   add esi, 2
   jmp trimLeftLink.cicle
.exit:
   pop eax
   ret


;�������� �������� ��������� �� ��������: esi - ��������� �� ������ ������ ���������
getParams:
   push ebx
   push esi

   ; ������ ���������� ���������� ����������
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

;�������� ������� ������� �� ������
getCommand:
   mov ebx, 2048
   mov esi, consoleModeSettings.current_command
   call clearCurrentCommand;������� ���������� ����������
   push esi
   push edi
   push eax

   mov eax, 0
   mov esi, consoleModeSettings.command_text
   call trimLeftLink
   mov edi, consoleModeSettings.current_command
.c:
   mov ax, word[esi]
   cmp ax, 32  ;�� �� ������� ������� ��������� ��������
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
testText: ;����� ��� �������� ����� ����������� ����������
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
   call addStringFileData; ���������� � �������� ����� ���� �� ������ �������� ������������ (�� �������� �������� �������)
   pop ebx
   pop eax

   inc eax
   cmp eax, dword[console_files_count]
   jb testText.next_block
   pop eax
   ret


;---
clearCommandText:   ;������� ���� ����� �������
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
   mov dword[consoleModeSettings.command_pos], 0 ;�������� ������� �����
   ret

printHeader:
   mov ecx, 40 ;2048
      mov esi, text_application_title.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 0 ;��� �����
      call addInHistory

   mov ecx, 46 ;2048
      mov esi, text_application_version.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 0 ;��� �����
      call addInHistory
   ret
printError:  ;����������� ��� ������
  ;������� ������ � ������� ������
   mov ecx, 30
      mov esi, text_unknown_error.utf16le
      call createStringHistory
      mov eax, 0
      mov al, 2 ;��� ����� �������������� �� ������
      call addInHistory
   ret
printParams: ;����������� ��������� ������� �������
   mov ecx, 2048
      mov esi, consoleModeSettings.current_params
      call createStringHistory
      mov eax, 0
      mov al, 0 ;��� �����
      call addInHistory
   ret
printCurrentCommand: ;������������ ������� �������
mov ecx, 2048
      mov esi, consoleModeSettings.current_command
      call createStringHistory
      mov eax, 0
      mov al, 0 ;��� �����
      call addInHistory
   ret


commandInterpretor:   ;������������� ������
  call getCommand ;�������� ������� �������
  ;call printCurrentCommand
  ;call printParams
  ; call testText

  push eax
  mov eax, 0
  mov esi, consoleModeSettings.current_command

  ;���� ��� ������� echo ��� print, �� ������ �������� ��� ������
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
@@:  ;���� ������� ������ - ���������� ������ ���������
  mov esi, consoleModeSettings.current_command
  mov edi, command_listUTF16LE.exit
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call consoleMode.deinit
  mov eax, -1		       ;������� ��� ���������
  mcall
  jmp commandInterpretor.end
@@:  ;���������� ������� ������� ����������
  mov edi, command_listUTF16LE.pwd
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_pwd
  jmp commandInterpretor.end
@@: ;����� �������� (Change Directory)
  mov edi, command_listUTF16LE.cd
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cd
  jmp commandInterpretor.end
@@: ;�������� ����� �������
  mov edi, command_listUTF16LE.cls
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cls
  jmp commandInterpretor.end
@@: ;�������� ����� �������
  mov edi, command_listUTF16LE.clear
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_cls
  jmp commandInterpretor.end
@@:  ;��������/�������� ����� ������ � �������
  mov edi, command_listUTF16LE.echo_on
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_echo_on
  jmp commandInterpretor.end
@@: ;������/��������� ����� ������ � �������
  mov edi, command_listUTF16LE.echo_off
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_echo_off
  jmp commandInterpretor.end
@@:  ; ����� � �������� (��������� ������)
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
@@: ;������� ������ �����/����������
  mov edi, command_listUTF16LE.rm
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_rm
  jmp commandInterpretor.end
@@: ;������� �����/����������
  mov edi, command_listUTF16LE.md
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_mkdir
  jmp commandInterpretor.end
@@:  ;������� �����/����������
  mov edi, command_listUTF16LE.mkdir
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_mkdir
  jmp commandInterpretor.end
@@: ;�������� �� ���� ����������� ������ �������
  mov edi, command_listUTF16LE.iskef
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_iskef
  jmp commandInterpretor.end
@@: ;�������������� ��� ����
  mov edi, command_listUTF16LE.minimize_all_windows
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_minimize_all_windows
  jmp commandInterpretor.end
@@: ; ���������� ������ ���� � �������� �������, �� ���������: �� ������ ������
  mov edi, command_listUTF16LE.set_mouse_position
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_position
  jmp commandInterpretor.end
@@:  ; ����� �� ���������� ����� ����������� ��� ������ ��������� ������
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
@@: ; ����������� ������ ������
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
@@: ; ������������ �������
  mov edi, command_listUTF16LE.restart_kernel
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_restart_kernel
  jmp commandInterpretor.end
@@:; ���������� �������
  mov edi, command_listUTF16LE.shutdown
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_shutdown
  jmp commandInterpretor.end
@@: ;������� ����� ������� ����� (�� �������������)
  mov edi, command_listUTF16LE.eject
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_eject
  jmp commandInterpretor.end
@@: ; �������� y���� ����� ��������/������ �� ��� ��������������
  mov edi, command_listUTF16LE.pslot
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_pslot
  jmp commandInterpretor.end
@@: ; �������� ���������������� ���� �������
  mov edi, command_listUTF16LE.get_mouse_sensitivity
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_mouse_sensitivity
  jmp commandInterpretor.end
@@: ; ������ ����� ���������������� ����
  mov edi, command_listUTF16LE.set_mouse_sensitivity
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_sensitivity
  jmp commandInterpretor.end
@@: ; �������� �������� ���� �������
  mov edi, command_listUTF16LE.get_mouse_speed
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_mouse_speed
  jmp commandInterpretor.end
@@: ; ������ ����� �������� ����
  mov edi, command_listUTF16LE.set_mouse_speed
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mouse_speed
  jmp commandInterpretor.end
@@: ; ���������� ���-�� ���� ��������� ��� � �������
  mov edi, command_listUTF16LE.get_ram_size
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_ram_size
  jmp commandInterpretor.end
@@: ; ���������� ���-�� ��������� ��� � �������
  mov edi, command_listUTF16LE.get_free_ram
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_free_ram
  jmp commandInterpretor.end
@@: ; ������� ������ ����
  mov edi, command_listUTF16LE.get_core_version
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_core_version
  jmp commandInterpretor.end
@@: ; ��������� �������/����� �� ������ �����
  mov edi, command_listUTF16LE.terminate
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_terminate
  jmp commandInterpretor.end
@@: ; ���������� ����� ��������� ����: 1=���������, 2=���������
  mov edi, command_listUTF16LE.background_set_mode
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_background_set_mode
  jmp commandInterpretor.end
@@: ; ������������ ��� �������� �����
  mov edi, command_listUTF16LE.background_redraw
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_background_redraw
  jmp commandInterpretor.end
@@: ; ���������� ����� ��������� ���� ��
  mov edi, command_listUTF16LE.set_mode
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_set_mode
  jmp commandInterpretor.end
@@: ; ���������� ��������� ����
  mov edi, command_listUTF16LE.date
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_date
  jmp commandInterpretor.end
@@: ; ���������� ��������� �����
  mov edi, command_listUTF16LE.time
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_time
  jmp commandInterpretor.end
@@: ; ������������ �������
  mov edi, command_listUTF16LE.reboot
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_reboot
  jmp commandInterpretor.end
@@: ;��������� �������/����� �� uid
  mov edi, command_listUTF16LE.kill
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_kill
  jmp commandInterpretor.end
@@: ;���������� ������� �������� �����
  mov edi, command_listUTF16LE.current_color_scheme
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_current_color_scheme
  jmp commandInterpretor.end
@@: ;���������� ���������� �������� ������� ���������
  mov edi, command_listUTF16LE.get_network_device_count
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_network_device_count
  jmp commandInterpretor.end
@@: ;�������� ��� �������� ����������
  mov edi, command_listUTF16LE.get_network_device_type
  call compareStringsUTF16LE
  cmp eax, 0
  jne @f
  call tool_get_network_device_type
  jmp commandInterpretor.end
;@@: ;�������� IP
;  mov edi, command_listUTF16LE.ipconfig
;  call compareStringsUTF16LE
;  cmp eax, 0
;  jne @f
;  call tool_get_ip
;  jmp commandInterpretor.end
@@:
  ;���� �� � ��� �� ������� �� ������� ���������
  call tool_execfile
.end:
  pop eax
  ret
;---
deleteLastSymbol:
   push ebx
   push esi
   mov ebx, dword[consoleModeSettings.command_pos] ;�������� ������� ������� �����
   cmp ebx, 0  ; ��� ������� ������� (������ ������ �������)
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




kirilicToASCII: ;al �������������� ����
   cmp al, 0x51 ;�
   jne @f
   mov al, 241
   jmp kirilicToASCII.end
@@:
   cmp al, 0x01 ;�
   jne @f
   mov al, 240
   jmp kirilicToASCII.end
@@:
   cmp al, 0x10 ;�� �
   jb @f
   cmp al, 0x2F ;�� �
   ja @f
   sub al, 0x10
   add al, 128
   jmp kirilicToASCII.end
@@:
   cmp al, 0x30 ;�� �
   jb @f
   cmp al, 0x3F ;�� �
   ja @f
   sub al, 0x30
   add al, 160
   jmp kirilicToASCII.end
@@:
   cmp al, 0x40 ;�� �
   jb @f
   cmp al, 0x4F ;�� �
   ja @f
   sub al, 0x40
   add al, 224
   jmp kirilicToASCII.end
@@:

.end:
   ret

addRusSymbol: ;�������� ���������������� ������ � ������ utf16le : ah �������������� ����
   cmp ah, 241 ;�
   jne @f
   mov ah, 0x51
   jmp addRusSymbol.end
@@:
   cmp ah, 240 ;�
   jne @f
   mov ah, 0x01
   jmp addRusSymbol.end
@@:
   cmp ah, 128	  ;�� �
   jb @f
   cmp ah, 159	  ;�� �
   ja @f
   sub ah, 128
   add ah, 0x10
   jmp addRusSymbol.end
@@:
   cmp ah, 160	  ;�� �
   jb @f
   cmp ah, 175	  ;�� �
   ja @f
   sub ah, 160
   add ah, 0x30
   jmp addRusSymbol.end
@@:
   cmp ah, 224	  ;�� �
   jb @f
   cmp ah, 239	  ;�� �
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
   cmp ah, 128	;��������� ����� ���������������� ��� ���
   jb @f       ;���� ��� �� ��������� �� ���������� �������� �������
   call addRusSymbol	;����� ����������� ��� �������� �������
   mov al, 0x04
@@:
   push ebx
   push esi
   mov ebx, dword[consoleModeSettings.command_pos] ;�������� ������� ������� �����
   cmp ebx, 2048-2    ;����� ���������� ��������(������ �������� ������ ������)
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
      ;mov dword[consoleModeSettings.data_max_count], eax ;14	;������������ ���-�� ����� ������ � �������
      pop eax
      ret
   .init:
      push eax
      mov eax, 2	;���������� UTF 16 LE
      call koFont.init	;�������������� �����
      pop eax				   ;A0 ������ 20 ��-�� ������ � ����� ������
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
      mov dword[consoleModeSettings.data_max_count], 14   ;������������ ���-�� ����� ������ � �������
      pop eax
      call initHistory ;�������������� ����� ������� � �������� ��� ��� ������

      mov byte[consoleModeSettings.echo_mode], 1 ;���������� ��
      call initCurrentDir
      call printHeader
      ret
   .mouse:
      ret

   .keyboard_interpretator:
      cmp ah, 13    ;enter
      jne @f

      ;�������� ������ ������� � �������
      push ecx
      push esi
      push eax

      mov ecx, 0
      mov cl, byte[consoleModeSettings.echo_mode]
      cmp cl, 0   ;���� ��������� echo ������
      je .keyboard_interpretator_1

      mov ecx, dword[consoleModeSettings.command_pos]
      add ecx, 2
      mov esi, consoleModeSettings.command_text
      call createStringHistory
      mov eax, 0
      mov al, 1 ;��� ���������� ����� �������
      call addInHistory
   .keyboard_interpretator_1:
      pop eax
      pop esi
      pop ecx

      call commandInterpretor
      call clearCommandText
      call draw_window ;call red ;�������� �����������
      ret
   @@:
      cmp ah, 10    ;��������� ������� �������
      je @f
      call insertLastSymbol;��������� ������� ������ � ������
      ;call draw_window ;call red ;�������� �����������
   @@:
      ret

   .keyboard:
      cmp     ah, 27  ;escape
      jne     @f
	call consoleMode.deinit
	mov	eax,-1			; ������� -1 : ������� ��� ���������
	mcall
    @@:
      cmp ah, 8   ;delete=8
      jne @f
      call deleteLastSymbol  ;������� �� ������ ��������� �������� ������
      call red ;�������� �����������
      ret
    @@:
      cmp ah, 182   ;backspace=182
      jne @f
      call deleteLastSymbol  ;������� �� ������ ��������� �������� ������
      call red ;�������� �����������
      ret
    @@:
      cmp ah, 13    ;enter
      jne @f

      ;�������� ������ ������� � �������
      push ecx
      push esi
      push eax

      mov ecx, 0
      mov cl, byte[consoleModeSettings.echo_mode]
      cmp cl, 0   ;���� ��������� echo ������
      je .keyboard_interpretator_2

      mov ecx, dword[consoleModeSettings.command_pos]
      add ecx, 2
      mov esi, consoleModeSettings.command_text
      call createStringHistory
      mov eax, 0
      mov al, 1 ;��� ���������������� ����� �������
      call addInHistory
   .keyboard_interpretator_2:
      pop eax
      pop esi
      pop ecx

      ;call clearCommandText
      call commandInterpretor
      call clearCommandText
      call red ;�������� �����������
      ret
    @@:
      call insertLastSymbol;��������� ������� ������ � ������:
      call red ;�������� �����������
      ret
   .hotkey:
      ret
   .draw:
      call updateConsoleStringSymbolsCount ;�������� ���-�� �������� ��������� � �������
      mov dword[current_string_offset], 30 ;�������� ������ ������ ������������ ����� ����

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

      call printHistoryContent ;����������� ������� ���������� ��������

      ;� ����� ����� �������� ������� ������ ����� ������������
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
      call deinitAllSlotHistory ;����������� �������������� ������ - ������ �������
      call deinitHistory ;������� ����� �������
      ;mov dword[applicationSettings.window_background_color], 0x14FFFFFF
      ret



;��������� ����� ls
addStringFileData:
   mov eax, read_folder_return_struct.block + 40
   mov dword[consoleDataStringStruct.text], eax

;������
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

@@:;�����
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

;����
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

;��-�� ������ � ����� ������� ����� �������� 20 �� A0
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




;�������� ���-�� �������� ��������� � ������ ������ ������� ��� ������� ������
updateConsoleStringSymbolsCount:
   push eax
   mov eax, [windowSettings.cwidth]   ;������ ������� �������
   sub eax, 15 ;������ ��� ������
   sub eax, dword[koFont.symbol_width] ;����� ��� ������� ������� �����, ����� ���� ��� ������ ������ ��� ������ (�� � ���� �� �� ��������)
   sub eax, dword[koFont.symbol_width]
   call koFont.getSymbolsCount
   mov dword[consoleModeSettings.max_string_symbols_count], eax
   pop eax
   ret


;������ ������
consoleMode_printData:	;� esi - ��������� �� ���� ������
      push eax
      push ebx
      push ecx
      push edx
      push esi
      mov  ebx, 28

;� ����������� �� ���� ������ - ���� ������ � ��������
      mov eax, 0
      mov al, byte [esi]

      cmp al, 0    ;���� 0 - ��� ������� �����
      jne @f
      mov     ecx, dword[consoleModeSettings.console_text_color]
      jmp .finish
   @@:
      cmp al, 1   ;���� 1 - ��� ���������������� �����
      jne @f
      mov  ecx, dword[consoleModeSettings.console_user_command_text_color]
;      sub ebx, dword[fonts.symbol_width]
;;	call printPreString
      jmp .finish
   @@:
      cmp al, 2   ;���� 2 �� ��� ��������������
      jne @f
      mov     ecx, dword[consoleModeSettings.warning_text_color]
      jmp .finish
   @@:
      cmp al, 3   ;���� 3 �� ��� �����
      jne @f
      mov     ecx, dword[consoleModeSettings.folder_color]
      jmp .finish

   @@:
      cmp al, 4   ;���� 4 �� ��� ����
      jne @f
      mov     ecx, dword[consoleModeSettings.files_color]
      jmp .finish

   @@:
      cmp al, 5   ;���� 5 �� ��� ����������
      jne @f
      mov     ecx, dword[consoleModeSettings.application_color]
      jmp .finish

   @@:
      cmp al, 6   ;���� 6 �� ��� ������
      jne @f
      mov     ecx, dword[consoleModeSettings.partition_color]
      jmp .finish

   @@:
      cmp al, 7   ;���� 7 - �� ��� ������� ������������ ��� ��������� �������� ������
      jne @f
      mov     ecx, dword[consoleModeSettings.console_user_command_text_color]
      jmp .finish

   @@: ;����� ������ ����� ����� - ��� ���������� ���-��
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
      mov     edx, dword[esi+5]  ;��������� �� �����
      mov     esi, dword[esi+1]  ;���-�� ��������
      mov     eax, 4
      mcall
pop eax

cmp al, 1   ;���������� - ��� �������� ��� ������� ����� ��������
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
      mov     esi, 2  ;���-�� ��������
      mov     eax, 4
      mcall
@@:
      pop esi
      pop edx
      pop ecx
      pop ebx
      pop eax
      ret


;�������� ������ ������������� ����� ����� �������
;����: eax - ����� ����� �����
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

mov ebx, dword[consoleModeSettings.data_count]	  ;���� ������� ����� ����� ������ ��� �����, �� ��������� ��������
mov ecx, ebx ;�������� � ecx - ���-�� ������ � ������ �������
mov dword[old_history_blocks_count], ecx
cmp ebx, eax
jbe @f
mov ebx, eax
@@:
push ebx

    ;�������� ��������� �� ������ �������
    mov edi, dword[consoleModeSettings.data]
    mov dword[old_adress], edi
    push edi
    push ecx

    ;�������� ����� ��������� �������
    mov dword[consoleModeSettings.data_max_count], eax
    call initHistory
    mov esi, eax

    ;������������ ����� ������� ������� � �����
    pop ecx
    pop edi
    ;������ ������� ������ � ������ ������ ������� ����� ���������� � ������� � edx
    mov edx, 0
    cmp ecx, dword[consoleModeSettings.data_max_count]
    jbe @f
    mov edx, ecx
    sub edx, dword[consoleModeSettings.data_max_count]
@@:
    mov dword[skip_count], edx
    push edi
    ;��� �������� �������� ������
    push eax
    push edx
    push ebx
    mov ebx, edx
    mov edx, 0
    mov eax, 9 ;��� ������ 1 ���������
    mul ebx
    mov dword[offset_data_byte], eax
    pop ebx
    pop edx
    pop eax

    add edi, dword[offset_data_byte] ;���� ��������� �� ������ ���������� ������ �� ������ ���������
    mov edx, dword[old_history_blocks_count]
.copy: ;��������
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
    ;������� ������ �������� ����� ������ �������
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


    ;��������� ������ ������ �������
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

;�������� ������ ��� ������ ����� ������ ������
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
   mul ebx ; 9 ��� ������ ������ ����� ������
   mov ecx, eax
   mcall 68, 12
   mov dword[consoleModeSettings.data], eax
@@:
   pop edx
   pop ecx
   pop ebx
   pop eax
   ret

;����������� ������ ������ ������ (�� �� �� �����������)
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

;����������� ������ ������� �����
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

;����������� ������ ���� ������ �������
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

;�������� ��� ������ �� 1 ����� � �������� ���������
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

;�������� ������ � �������
;�������: al -��� ������, ecx - ������ ������ � ������, esi - ��������� �� ������
addInHistory:
   push eax
   push ecx
   push esi
   push edi
   push edx

   ;��������� ���� �� ��������� �����
   mov edx, dword[consoleModeSettings.data_count]
   cmp edx, dword[consoleModeSettings.data_max_count]
   jl @f ;���� ���� �� ��������� �� ���
   push ecx
   push esi
   call deinitFirstSlotHistory; ����� ������� ������ ����
   call swapHistory;� ������ ����� � ����������
   ;���� ��� �������� ��� ���� � �����
   ;dec eax;��������� ������� ������� ������ �� 1
   ;mov dword[consoleModeSettings.data_count], eax
   pop esi
   pop ecx
@@:
   ;������� ������ ��������� ����
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

   ;���������� ���� ������
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

; ����������� ��� ������� �������� ������������
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


;�������� ������ ��� ������ � ����������� � ������ ����
;�������: ecx - ������ ������ � ������, esi - ����� ������
;��������: esi - ����� ����� ����� ������
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
;��������� ���� utf16le �����
;�������: esi - ��������� �� ������ ������, edi - ��������� �� ������ ������
;��������: eax - 0 ������ ��������� ��� ����� ������� ��������� �������
compareStringsUTF16LE:
   mov eax, 0
   push ebx
   push edi
   push esi
.start:
   mov bx, word[esi]
   inc eax
   cmp bx, word[edi]   ;���� ������� �� �������, �� �� ����� ������� �� ����������
   jne compareStringsUTF16LE.end
   cmp bx, 0  ;����� ���� ������ ���� ����� � ������ � ����� 0, �� ��� ����� ����� �����
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


;��������/������������ ���� utf16le �����
;�������: esi - ��������� �� ������ ������, edi - ��������� �� ������ ������
;eax - ��������� �� ����� ������, ����������� ���������
concatStringsUTF16LE:
   ret

;--------------------------------------------
;(�) ���������� ������ �., 2018