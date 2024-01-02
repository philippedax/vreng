AUTOMAKE_OPTIONS = foreign no-dependencies

GLUI_CPP = glui_add_controls.cpp glui.cpp glui_bitmap_img_data.cpp glui_bitmaps.cpp glui_button.cpp glui_commandline.cpp glui_edittext.cpp glui_checkbox.cpp glui_filebrowser.cpp glui_node.cpp glui_radio.cpp glui_statictext.cpp glui_panel.cpp glui_separator.cpp glui_spinner.cpp glui_control.cpp glui_column.cpp glui_translation.cpp glui_rotation.cpp glui_mouse_iaction.cpp glui_list.cpp glui_listbox.cpp glui_rollout.cpp glui_scrollbar.cpp glui_window.cpp glui_string.cpp glui_textbox.cpp glui_tree.cpp glui_treepanel.cpp arcball.cpp algebra3.cpp quaternion.cpp viewmodel.cpp
GLUI_C = glui_img_checkbox_0.c glui_img_checkbox_0_dis.c glui_img_checkbox_1.c glui_img_checkbox_1_dis.c glui_img_downarrow.c glui_img_leftarrow.c glui_img_listbox_down.c glui_img_listbox_up.c glui_img_listbox_up_dis.c glui_img_radiobutton_0.c glui_img_radiobutton_0_dis.c glui_img_radiobutton_1.c glui_img_radiobutton_1_dis.c glui_img_rightarrow.c glui_img_spindown_0.c glui_img_spindown_1.c glui_img_spindown_dis.c glui_img_spinup_0.c glui_img_spinup_1.c glui_img_spinup_dis.c glui_img_uparrow.c

GLUI_SRCS = $(GLUI_CPP) $(GLUI_C)
GLUI_HDRS = glui.h algebra3.h arcball.h glui_internal.h glui_internal_control.h quaternion.h viewmodel.h

AM_CPPFLAGS = @GL_CFLAGS@

EXTRA_DIST = $(GLUI_HDRS)

#if ENABLE_GLUI
noinst_LIBRARIES = libglui.a
libglui_a_SOURCES = $(GLUI_SRCS) $(GLUI_HDRS)
#endif
