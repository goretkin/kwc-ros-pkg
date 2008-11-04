// generated by Fast Light User Interface Designer (fluid) version 1.0108

#ifndef stereogui_h
#define stereogui_h

#ifdef WIN32
#pragma warning (disable: 4312 4311)
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Box.H>
#include "calwin.h"
#include <FL/Fl_Output.H>
#include <FL/Fl_Menu_Bar.H>
extern void load_images_cb(Fl_Menu_*, void*);
extern void save_images_cb(Fl_Menu_*, void*);
extern void load_params_cb(Fl_Menu_*, void*);
extern void save_params_cb(Fl_Menu_*, void*);
extern void stereo_window_cb(Fl_Menu_*, void*);
extern void cal_window_cb(Fl_Menu_*, void*);
#include <FL/Fl_Button.H>
extern void cal_capture_cb(Fl_Button*, void*);
extern void cal_load_cb(Fl_Button*, void*);
extern void cal_save_image_cb(Fl_Button*, void*);
extern void cal_save_all_cb(Fl_Button*, void*);
extern void cal_delete_image(Fl_Button*, void*);
extern void cal_load_left_cb(Fl_Button*, void*);
extern void cal_load_right_cb(Fl_Button*, void*);
extern void cal_load_seq_cb(Fl_Button*, void*);
#include <FL/Fl_Value_Input.H>
extern void cal_check_size_cb(Fl_Value_Input*, void*);
extern void cal_check_x_cb(Fl_Value_Input*, void*);
extern void cal_check_y_cb(Fl_Value_Input*, void*);
extern void cal_calibrate_cb(Fl_Button*, void*);
extern void cal_save_params_cb(Fl_Button*, void*);
extern void cal_ok_cb(Fl_Button*, void*);
extern void cal_epipolar_cb(Fl_Button*, void*);
#include <FL/Fl_Check_Button.H>
extern void cal_fixed_aspect_cb(Fl_Check_Button*, void*);
extern void cal_fixed_kappa2_cb(Fl_Check_Button*, void*);
extern void cal_fixed_kappa3_cb(Fl_Check_Button*, void*);
extern void cal_fixed_tau_cb(Fl_Check_Button*, void*);
extern void cal_zero_disparity_cb(Fl_Check_Button*, void*);
#include <FL/Fl_Counter.H>
extern void unique_cb(Fl_Counter*, void*);
extern void texture_cb(Fl_Counter*, void*);
extern void speckle_cb(Fl_Counter*, void*);
extern void disparity_cb(Fl_Counter*, void*);
extern void corrsize_cb(Fl_Counter*, void*);
extern void xoff_cb(Fl_Counter*, void*);
extern void do_stereo_cb(Fl_Button*, void*);

class stereogui {
public:
  stereogui();
  Fl_Window *stereo_calibration;
  Fl_Tabs *window_tab;
  Fl_Group *tab0;
  calImageWindow *calLeft0;
  calImageWindow *calRight0;
  Fl_Group *tab1;
  calImageWindow *calLeft1;
  calImageWindow *calRight1;
  Fl_Group *tab2;
  calImageWindow *calLeft2;
  calImageWindow *calRight2;
  Fl_Group *tab3;
  calImageWindow *calLeft3;
  calImageWindow *calRight3;
  Fl_Group *tab4;
  calImageWindow *calLeft4;
  calImageWindow *calRight4;
  Fl_Group *tab5;
  calImageWindow *calLeft5;
  calImageWindow *calRight5;
  Fl_Group *tab6;
  calImageWindow *calLeft6;
  calImageWindow *calRight6;
  Fl_Group *tab7;
  calImageWindow *calLeft7;
  calImageWindow *calRight7;
  Fl_Group *tab8;
  calImageWindow *calLeft8;
  calImageWindow *calRight8;
  Fl_Group *tab9;
  calImageWindow *calLeft9;
  calImageWindow *calRight9;
  Fl_Group *tab10;
  calImageWindow *calLeft10;
  calImageWindow *calRight10;
  Fl_Group *tab11;
  calImageWindow *calLeft11;
  calImageWindow *calRight11;
  Fl_Group *tab12;
  calImageWindow *calLeft12;
  calImageWindow *calRight12;
  Fl_Group *tab13;
  calImageWindow *calLeft13;
  calImageWindow *calRight13;
  Fl_Group *tab14;
  calImageWindow *calLeft14;
  calImageWindow *calRight14;
  Fl_Group *tab15;
  calImageWindow *calLeft15;
  calImageWindow *calRight15;
  Fl_Group *tab16;
  calImageWindow *calLeft16;
  calImageWindow *calRight16;
  Fl_Group *tab17;
  calImageWindow *calLeft17;
  calImageWindow *calRight17;
  Fl_Group *tab18;
  calImageWindow *calLeft18;
  calImageWindow *calRight18;
  Fl_Group *tab19;
  calImageWindow *calLeft19;
  calImageWindow *calRight19;
  Fl_Group *tab20;
  calImageWindow *calLeft20;
  calImageWindow *calRight20;
  Fl_Output *info_message;
  static Fl_Menu_Item menu_[];
  Fl_Window *cal_window;
  Fl_Button *capture_button;
  Fl_Button *load_button;
  Fl_Button *save_button;
  Fl_Button *save_all_button;
  Fl_Button *delete_button;
  Fl_Button *load_left_button;
  Fl_Button *load_right_button;
  Fl_Button *load_seq_button;
  Fl_Value_Input *user_check_size;
  Fl_Value_Input *user_check_x;
  Fl_Value_Input *user_check_y;
  Fl_Button *calibrate_button;
  Fl_Button *save_params_button;
  Fl_Button *done_button;
  Fl_Button *epi_button;
  Fl_Check_Button *fixed_aspect_button;
  Fl_Check_Button *fixed_kappa2_button;
  Fl_Check_Button *fixed_kappa3_button;
  Fl_Check_Button *fixed_tau_button;
  Fl_Check_Button *zero_disparity_button;
  Fl_Window *stereo_window;
  Fl_Button *stereo_button;
  ~stereogui(); 
};
#endif
