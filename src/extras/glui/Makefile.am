AUTOMAKE_OPTIONS = foreign no-dependencies

GLUI_SRCS = glui_add_controls.cpp glui.cpp glui_bitmap_img_data.cpp glui_bitmaps.cpp glui_button.cpp glui_edittext.cpp glui_checkbox.cpp glui_node.cpp glui_radio.cpp glui_statictext.cpp glui_panel.cpp glui_separator.cpp glui_spinner.cpp glui_control.cpp glui_column.cpp glui_translation.cpp glui_rotation.cpp glui_mouse_iaction.cpp glui_listbox.cpp glui_rollout.cpp arcball.cpp algebra3.cpp quaternion.cpp \
glui_img_checkbox_0.c glui_img_checkbox_0_dis.c glui_img_checkbox_1.c glui_img_checkbox_1_dis.c glui_img_downarrow.c glui_img_leftarrow.c glui_img_listbox_down.c glui_img_listbox_up.c glui_img_listbox_up_dis.c glui_img_radiobutton_0.c glui_img_radiobutton_0_dis.c glui_img_radiobutton_1.c glui_img_radiobutton_1_dis.c glui_img_rightarrow.c glui_img_spindown_0.c glui_img_spindown_1.c glui_img_spindown_dis.c glui_img_spinup_0.c glui_img_spinup_1.c glui_img_spinup_dis.c glui_img_uparrow.c

GLUI_HDRS = algebra3.h arcball.h glui.h quaternion.h stdinc.h # viewmodel.h


EXTRA_DIST = $(GLUI_HDRS) readme

if GLUI
noinst_LIBRARIES = libglui.a
libglui_a_SOURCES = $(GLUI_SRCS) $(GLUI_HDRS)
endif
