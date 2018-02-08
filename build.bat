@echo lang fix ru >lang.inc
@fasm.exe -m 16384 cfm.asm cfm
@erase lang.inc
@kpack cfm
@pause