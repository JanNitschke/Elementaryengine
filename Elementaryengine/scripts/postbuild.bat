XCOPY "$(SolutionDir)"\lib\*.DLL "$(TargetDir)" /D /K /Y
XCOPY "$(SolutionDir)"\"$(ProjectDir)"\*.frag "$(TargetDir)" /D /K /Y
XCOPY "$(SolutionDir)"\"$(ProjectDir)"\*.vert "$(TargetDir)" /D /K /Y