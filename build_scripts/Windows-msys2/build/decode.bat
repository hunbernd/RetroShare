if "%BuildMode%"=="vanilla" (
	set BuildExtra=
	set extrainfo=
) else if "%BuildMode%"=="extra" (
	set BuildExtra="CONFIG+=rs_gui_cmark" "CONFIG+=rs_autologin" "CONFIG+=rs_jsonapi" "CONFIG+=rs_use_native_dialogs"
	set extrainfo=-extra
)