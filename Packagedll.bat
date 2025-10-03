"C:\Program Files\MRE_SDK\tools\DllPackage.exe" "D:\MyGitHub\cell_info\cell_info.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy cell_info.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\cell_info.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

