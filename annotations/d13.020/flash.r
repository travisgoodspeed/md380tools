
f md380_menu_id @ 0x2001e915
f md380_menu_mem_base @ 0x2001b274
f md380_menu_memory @ 0x2001d5cc
f mn_editbuffer_poi @ 0x200049fc
f md380_menu_edit_buf @ 0x2001cb9a
f md380_menu_0x2001d3f0 @ 0x2001e946
f md380_menu_depth @ 0x20004acc
f md380_menu_0x2001d3ef @ 0x2001e945
f md380_menu_0x2001d3f1 @ 0x2001e947

f md380_menu_0x2001d3ee @ 0x2001e944
f md380_menu_0x2001d3f4 @ 0x2001e94a
f md380_menu_0x2001d3ed @ 0x2001e943

af+ 0x800c188 1446 md380_create_main_meny_entry
af+ 0x800c72e 86 md380_create_menu_entry
af+ 0x800fc84 18 md380_menu_entry_back



af+ 0x80134a0 408 Create_Menu_Utilies
CCa 0x801351e R.a.d.i.o...S.e
CCa 0x801351e R.a.d.i.o...S.e
CCa 0x801354e R.a.d.i.o...I.n.
CCa 0x801359a P.r.o.g.r.a.m..
CCa 0x80135ce G.P.S./.B.e.i.D
CCa 0x8013602 R.X...G.P.S.I.n.
af+ 0x80136c0 930 md380_menu_entry_programradio
CCa 0x8013710 E.d.i.t...C.h.a.
CCa 0x8013762 R.x...F.r.e.q.u
CCa 0x80137ba C.h.a.n.n.e.l.
CCa 0x80137e8 T.i.m.e...O.u.t
CCa 0x8013824 C.T.C./.D.C.S
CCa 0x8013890 C.o.l.o.r...C.o
CCa 0x80138be R.e.p.e.a.t.e.r.
CCa 0x80138ec T.x.C.o.n.t.a.
CCa 0x801391a G.r.o.u.p.L.i.s
CCa 0x801394a C.o.l.o.r...C.o
CCa 0x8013978 R.e.p.e.a.t.e.r


CCa 0x8013a1e E.n.t.e.r...P.
af+ 0x80156a4 104 Create_Menu_Entry_RX_QRG_shown
af+ 0x8015720 220 Create_Menu_Entry_RX_QRG_1


CCa 0x80157e2 Create_Menu_Entry_RX_QRG_2
CCa 0x80157e6 Create_Menu_Entry_RX_QRG_3

af+ 0x80157fc 126 Create_Menu_Entry_RX_QRG_2


af+ 0x801587a 102 Create_Menu_Entry_RX_QRG_3
CCa 0x80158c4 Create_Menu_Entry_RX_QRG_4
af+ 0x8015900 628 Create_Menu_Entry_RX_QRG_4
f Create_Menu_Entry_RX_QRG_4 @ 0x8015900


af+ 0x8017cbc 1404 Create_Menu_Entry_RadioSettings

CCa 0x8017f48 I.n.t.r.o...S.c.
CCa 0x8017f94 K.e.y.p.a.d...L.
CCa 0x8017fcc L.a.n.g.u.a.g.e
CCa 0x8018018 L.E.D...I.n.d.i.
af+ 0x8018b28 236 md380_itow


af+ 0x801ad56 246 Create_Menu_Entry_LEDIndicator


af+ 0x801b042 26 md380_menu_numerical_input

CCa 0x801b8e8 md380_menu_0x2001d3f1

f gfx_blockfill 30 0x801d88c
af+ 0x801d88c 30 gfx_blockfill

f gfx_linefill 0 0x0801d81a
af+ 0x0801d81a 104 gfx_linefill

f screen_unknown1 0 0x0800e728
f menu_6_15_1 0 0x0800e7a8
f menu_6_1_1 0 0x0800e7cc


af+ 0x801fe5c 5454 md380_f_4225
af+ 0x802256a 324 aes_startup_check
af+ 0x80226c0 18 Get_Welcome_Line1_from_spi_flash
af+ 0x80226d2 18 Get_Welcome_Line2_from_spi_flash

CCa 0x8022f1a md380_menu_edit_buf
CCa 0x8022f46 md380_menu_edit_buf
CCa 0x8022fde md380_menu_edit_buf


af+ 0x80237fe 86 gfx_drawbmp
f gfx_drawbmp 0 0x80237fe


af+ 0x8023ee4 394 Edit_Message_Menu_Entry


af+ 0x8025ae4 888 F_4315


af+ 0x802b3f6 80 md380_RTC_GetTime
af+ 0x802b50c 76 md380_RTC_GetDate


af+ 0x802dfbc 1908 md380_f_4137


af+ 0x802f9dc 4056 Beep_Process

CCa 0x802fa36 re issue 227
CCa 0x802fc1e beep 9
CCa 0x802fa54 no dmr sync tone
CCa 0x802fa66 dmr sync
CCa 0x802fad8 roger beep
CCa 0x802fbe4 beginn roger beep
CCa 0x802fd54 beginn dmr sync
af+ 0x8030aa4 52 F_293
af+ 0x8030ad8 16 F_294

f md380_OSMboxPend @ 0x8031084
af+ 0x8031084 258 md380_OSMboxPend

af+ 0x803119c 86 md380_OSMboxPost

af+ 0x8031276 52 md380_spiflash_sektor_erase4k
af+ 0x80312aa 52 md380_spiflash_block_erase64k
af+ 0x80312de 76 spiflash_program_page
af+ 0x803132a 332 F_1069_spiflash_multiple_spiflash_program_page
af+ 0x8031476 70 md380_spiflash_read
af+ 0x80314bc 58 md380_spi_sendrecv
af+ 0x80314f6 18 spiflash_write_enable
af+ 0x8031508 34 md380_spiflash_wait
af+ 0x803152a 28 md380_spiflash_enable   
af+ 0x8031546 24 md380_spiflash_disable
af+ 0x803155e 704 md380_spiflash_write
af+ 0x8031830 52 spiflash_Erase_Security_Registers_44h
af+ 0x8031864 76 spiflash_Program_Security_Registers_42h
af+ 0x80318b0 78 md380_spiflash_security_registers_read
af+ 0x8033eb4 104 OSTimeDly

af+ 0x803b39a 144 main_menu
af+ 0x803f708 76 OSSemCreate
af+ 0x803f754 218 OSSemPend
af+ 0x803f844 92 OSSemPost

f F_414 @ 0x8040670
af+ 0x8040670 880 F_414
af+ 0x8040a02 740 dmr_call_start
af+ 0x8040ce6 92 dmr_before_squelch
af+ 0x8040d44 94 F_858

af+ 0x8040de0 1540 dmr_sms_arrive
af+ 0x8041430 864 dmr_call_end


f dmr_CSBK_handler @ 0x080417e0
af+ 0x080417e0 0 dmr_CSBK_handler

af+ 0x8043de4 8 OS_ENTER_CRITICAL
af+ 0x8043dec 6 OS_EXIT_CRITICAL
af+ 0x80462bc 314 Start
af+ 0x8046520 684 Start_multiple_tasks
af+ 0x8049e14 798 Start_2_more_tasks__init_vocoder_tasks__Q
CCa 0x804c74c md380_menu_edit_buf
CCa 0x804c76e md380_menu_edit_buf
af+ 0x804dd70 430 dmr_handle_data
af+ 0x804e580 204 OSTaskCreateExt
af+ 0x804e64c 90 OSTaskNameSet
af+ 0x804eb64 152 md380_f_4098
af+ 0x804ec66 298 md380_f_4102
CCa 0x804ee2c md380_menu_0x200011e4
af+ 0x80531d8 124 ambe_encode_thing__size_not_correct
af+ 0x8053680 140 ambe_decode_wav
af+ 0x8055100 68 usb_setcallbacks
af+ 0x8059b02 40 usb_send_packet
af+ 0x808eb30 190 usb_do_setup
af+ 0x808ebee 1444 usb_dnld_handle
af+ 0x808f308 3036 usb_upld_handle
af+ 0x8090370 80 usb_dfu_write
af+ 0x80903c0 54 usb_dfu_read
af+ 0x809662e 34 usb_serialnumber


f menu_entry_back_1 @ 0x800fc85

f md380_radio_config @ 0x2001dadc

f rc_write_radio_config_to_flash @ 0x080226f6 
af+ 0x080226f6 18 rc_write_radio_config_to_flash

Cd 4 @ 0x0800c784
Cd 4 @ 0x0800c788
Cd 4 @ 0x0800c7ac

f menugreen.Contacts.800fcbc 0 0x800fcbc
f menugreen.Scan.8037e34 0 0x8037e34
f menugreen.Zone.80131ac 0 0x80131ac
f menugreen.Messages.8023858 0 0x8023858
f menugreen.Call_Log.8034274 0 0x8034274
f menugreen.Utilities.80134a0 0 0x80134a0
f menugreen.Radio_Set.8017cbc 0 0x8017cbc
f menugreen.Radio_Inf.80165b8 0 0x80165b8
f menugreen.MD380Tool.809c3b0 0 0x809c3b0
f menugreen.GPS_BeiDo.8016688 0 0x8016688
f menugreen.RX_GPSInf.8013638 0 0x8013638
f menugreen.Talkaroun.8019170 0 0x8019170
f menugreen.Tone_Ale.8019798 0 0x8019798
f menugreen.Power.8019c7c 0 0x8019c7c
f menugreen.Squelch.801a1e0 0 0x801a1e0
f menugreen.Intro_Scr.801a4c6 0 0x801a4c6
f menugreen.Keypad_Lo.801a6c8 0 0x801a6c8
f menugreen.Language.801ab84 0 0x801ab84
f menugreen.LED_Indic.801ad56 0 0x801ad56
f menugreen.VOX.80185d8 0 0x80185d8
f menugreen.SiteRoam1.801ae5c 0 0x801ae5c
f menugreen.Passwd_Lo.801af7c 0 0x801af7c
f menugreen.SiteRoam2.800fc84 0 0x800fc84
f menugreen.Record.8018f04 0 0x8018f04
f menugreen.Clock.8018868 0 0x8018868
f menugreen.GPS.8018248 0 0x8018248

f menugreen.prog.80136c0 0 0x80136c0
f menugreen.prog.rxf.80156a4 0 0x80156a4
f menugreen.prog.rxf.show.8015720 0 0x8015720
f menugreen.prog.rxf.edit.801587a 0 0x801587a

f menugreen.prog.txf.8015b74 0 0x8015b74
f menugreen.prog.txf.show.8015be8 0 0x8015be8
f menugreen.prog.txf.edit.8015d58 0 0x8015d58


f menugreen.prog.Channel_N.8016024 0 0x8016024
f menugreen.prog.Time_Out_.8016320 0 0x8016320
f menugreen.prog.CTC_DCS.800fc84 0 0x800fc84
f menugreen.prog.Color_Cod.8015540 0 0x8015540
f menugreen.prog.Repeater_.80153dc 0 0x80153dc
f menugreen.prog.TxContact.8013e78 0 0x8013e78
f menugreen.prog.GroupList.8013fbc 0 0x8013fbc

f menugreen.Talk_Perm.8019998 0 0x8019998
f menugreen.Keypad_To.8019b40 0 0x8019b40

f menugreen.All_Tones.80198b0 0 0x80198b0
f menugreen.All_Tones.TurnOn.8019290 0 0x8019290
f menugreen.All_Tones.TurnOff.80194f4 0 0x80194f4

f menugreen.talkaround.TurnOn.8019290 0 0x8019290
f menugreen.talkaround.TurnOff.80194f4 0 0x80194f4

f menugreen.Mode.801840c 0 0x801840c
f menugreen.CH_Mode.80184e8 0 0x80184e8
f menugreen.MR_Mode.801855c 0 0x801855c

f menugreen.Backlight.8019eb0 0 0x8019eb0
f menugreen.backlight.Always.801a440 0 0x801a440
f menugreen.backlight.5S.801a03e 0 0x801a03e
f menugreen.backlight.10S.801a0c8 0 0x801a0c8
f menugreen.backlight.15S.801a154 0 0x801a154

f menugreen.Repeater_Slot.80153dc 0 0x80153dc
f menugreen.Repeater_Slot_1_2.80154a4 0 0x80154a4

f menugreen.msg.Inbox.80264fc 0 0x80264fc
f menugreen.msg.Write.8023cfe 0 0x8023cfe
f menugreen.msg.Quick_Tex.80239ec 0 0x80239ec
f menugreen.msg.Sent_Item.803a024 0 0x803a024
f menugreen.msg.Drafts.8024a80 0 0x8024a80
f menugreen.msg.unk.8023c34 0 0x8023c34
f menugreen.msg.quicktext.8023de2 0 0x8023de2
f menugreen.msg.Send.802409c 0 0x802409c
f menugreen.msg.Save.80249a8 0 0x80249a8
f menugreen.msg.Clear.8023e24 0 0x8023e24
f menugreen.msg.Contacts.802417e 0 0x802417e
f menugreen.msg.Manual_Di.8024220 0 0x8024220
f menugreen.msg.enter_id.802430c 0 0x802430c
f menugreen.msg.enter_id_ok.8024740 0 0x8024740

af+ 0x08024740 608 menugreen.msg.enter_id_ok

af+ 0x80154a4 152 menu_timeslot

f menucall.Contacts 0 0x800c278
f menucall.Scan 0 0x800c2f4
f menucall.Zone 0 0x800c326
f menucall.Messages 0 0x800c358
f menucall.Call_Log 0 0x800c38a
f menucall.Utilities 0 0x800c3bc

f menucall.Radio_Set 0 0x8013528
f menucall.Radio_Inf 0 0x801355c
f menucall.Program_R 0 0x080135a8
f menucall.Program_R_patched 0 0x809d818
f menucall.MD380Tool 0 0x809d834
f menucall.GPS_BeiDo 0 0x80135dc
f menucall.RX_GPSInf 0 0x8013610

f menu.dispatcher.unkn1 0 0x0800eb98
f menu.dispatch.greenkey 0 0x0800ed3a
f menu.dispatch.redkey_maybe 0 0x0800f2a0
af+ 0x0800eb98 1810 menu_dispatcher

f draw_datetime_row 0 0x0800df1a
af+ 0x0800df1a 250 draw_datetime_row
f md380_f_4225 0 0x0801fe5c

f call_F_4225_1 0 0x0802db42
f call_F_4225_2 0 0x080468e6

f update_scr_16 0 0x0802015e
f update_scr_17 0 0x08020236
f update_scr_18 0 0x08020376
f update_scr_19 0 0x0802046c
f update_scr_20 0 0x08020052
f update_scr_21 0 0x0801fee0
f update_scr_22 0 0x0801ff3e

f update_scr_27 0 0x08020612
f update_scr_28 0 0x080206b2
f update_scr_29 0 0x080206ee
f update_scr_30 0 0x0801ff8e
f update_scr_31 0 0x08020018
f update_scr_32 0 0x080207b4
f update_scr_33 0 0x0802080e

f update_scr_35 0 0x080201bc
f update_scr_36 0 0x0802020e
f update_scr_other 0 0x0802082c

f after_update_scr 0 0x080213aa

f scr_1 0 0x0802daf8

f draw_botline_text 0 0x08046810
f draw_topline_text_maybe 0 0x080467fa

f mainloop_entry 0 0x080468ae
f jmp_to_mainloop 0 0x080468f6
f md380_f_4520 0 0x802c83c
af+ 0x802c83c 2370 md380_f_4520

af+ 0x0804fdf4 10 set_AAAA
af+ 0x0804fdfe 10 set_CCCC

f disp_something 0 0x0800d69c
af+ 0x0800d69c 0 disp_something
 
f draw_statusline @ 0x08033dac
af+ 0x08033dac 244 draw_statusline

f draw_statusline_more @ 0x08021694
af+ 0x08021694 294 draw_statusline_more


af+ 0x08036fba 2 do_nothing_1
af+ 0x08036fbc 2 do_nothing_2


# gfx_

f gfx_info @ 0x2001da1c

f gfx_rc @ 0x080249be

f gfx_newline @ 0x08033c04
af+ 0x08033c04 26 gfx_newline

f gfx_get_xpos @ 0x08021888
af+ 0x08021888 8 gfx_get_xpos

f gfx_get_ypos @ 0x08021890
af+ 0x08021890 8 gfx_get_ypos

f gfx_set_fg_color 0 0x801d370
af+ 0x801d370 8 gfx_set_fg_color

f gfx_set_fg_color2 0 0x080331e0
af+ 0x080331e0 24 gfx_set_fg_color2

f gfx_set_bg_color @ 0x801d368
af+ 0x801d368 8 gfx_set_bg_color

f gfx_set_bg_color2 @ 0x080331c8
af+ 0x080331c8 24 gfx_set_bg_color2

af+ 0x8021874 16 gfx_select_font
f gfx_select_font 0 0x8021874

f gfx_drawchar_pos @ 0x08021940
af+ 0x08021940 18 gfx_drawchar_pos

f gfx_drawchar @ 0x0802189c
af+ 0x0802189c 154 gfx_drawchar

af+ 0x800def6 36 gfx_drawtext
f gfx_drawtext 0 0x800def6

af+ 0x801dd08 18 gfx_drawtext2
f gfx_drawtext2 0 0x801dd08

f call_gfx_drawtext2_1 0 0x0801ea2e
f call_gfx_drawtext2_2 0 0x0801f02c
f call_gfx_drawtext2_3 0 0x0801f044
f call_gfx_drawtext2_4 0 0x0801f07a
f call_gfx_drawtext2_5 0 0x0801f092
f call_gfx_drawtext2_6 0 0x0802d660
f call_gfx_drawtext2_7 0 0x0802d70e
f call_gfx_drawtext2_8 0 0x0802d8e2
f call_gfx_drawtext2_9 0 0x0802d9e4

f gfx_drawtext3 0 0x0802b142
af+ 0x0802b142 148 gfx_drawtext3

f gfx_clear3 0 0x0801dcc0
af+ 0x0801dcc0 40 gfx_clear3

f gfx_drawtext4 0 0x0801dd1a
af+ 0x0801dd1a 18 gfx_drawtext4

f gfx_drawtext5 0 0x0801dd2c
af+ 0x0801dd2c 16 gfx_drawtext5

f gfx_drawtext6 0 0x08027728
af+ 0x08027728 154 gfx_drawtext6

f gfx_drawtext7 0 0x080277c2
af+ 0x080277c2 16 gfx_drawtext7

f gfx_drawtext8 0 0x08036fc0
af+ 0x08036fc0 378 gfx_drawtext8

f gfx_drawchar_unk @ 0x0801d960

CCa 0x08036ff4 check_for_0_term 
CCa 0x08037118 check_for_0_term_and_loop

CCa 0x08033c96 mult_off21_off23_font

f gfx_drawtext9 0 0x0802b0d4
af+ 0x0802b0d4 110 gfx_drawtext9

f F_4039_something_write_to_screen 0 0x0800ded8
af+ 0x0800ded8 30 F_4039_something_write_to_screen


f draw_channel_label 0 0x0800e5a6
f draw_zone_label 0 0x0800e682
f draw_zone_channel 0 0x0800e538
af+ 0x0800e538 98 draw_zone_channel

f scr_mode_stable 0 0x08020830

f return_to_mode_1_from10 0 0x0800fc96
af+ 0x0800fc96 32 return_to_mode_1_from10

f F_4315 0 0x08025ae4

f promisc_audio_frame @ 0x08040cce 
f normal_audio_frame @ 0x08040cc4
f audio_for_me_or_not @ 0x08040c7a
f re_create_event_8 @ 0x08040c02
f event_36 @ 0x08041616
f event_4 @ 0x08041e44


f struct_channel_info2 @ 0x2001de78


af+ 0x0804edd0 2 dummy_0x0804edd0
af+ 0x0804f688 2 dummy_0x0804f688

f This_function_called_Read_Channel_Switch @ 0x0804fd04
af+ 0x0804fd04 136 This_function_called_Read_Channel_Switch

# keyborked

# struct keyboard_data
f keypressed_struct @ 0x2001e5f8
f keylocked_flags @ 0x2001e5f9

f kb_row_col_pressed @ 0x2001e7ba
f kb_row_col_pressed_last @ 0x2001e7bc
f kb_keydown_debounce @ 0x2001e889
f kb_keycode @ 0x2001e890

f keyup_keydown @ 0x2001e889
f keycode_old @ 0x2001e891
f keypressed_duringmenu @ 0x2001e5f3
f keypress_max_time @ 0x2001e7b8
f keypress_time_some_button @ 0x2001e7ac
f keypress_time_lower_button @ 0x2001e7b0
f keypress_time_upper_button @ 0x2001e7b2
f keypress_time_all @ 0x2001e7be

f keypress_flag @ 0x2001e5f8

f keypress_max_time_reached @ 0x0804face

f init_radioconfig_2_from_spi @ 0x08022714
f read_40_from_2100_spi @ 0x08022716
f menu_pointer_maybe_channel_data @ 0x2001def8

f base_for_longpress_struct @ 0x2001e5ec

f check_for_ptt_switch @ 0x0804ebfc
af+ 0x0804ebfc 106 check_for_ptt_switch

f kb_enter_alpha @ 0x0802e0b8

f get_keycode_from_table @ 0x804f8e4
f get_keycode_from_table_2 @ 0x804f8ea

f big_switch @ 0x0802d1b2

f store_keycode @ 0x0804fb24
f kb_handler @ 0x0804f94c
af+ 0x0804f94c 384 kb_handler
CCa 0x0804fa32 definite keydown
CCa 0x0804fa1e jump if b0 not set, reset debounce
CCa 0x0804fa30 not debounced yet, jump
CCa 0x0804fa4e jump if long keypress count is reached

f some_radio_status @ 0x2001e5f0

f dispatch_keyboard_2 @ 0x0802c83c

af+ 0x0801dd5c 2182 often_called_something_keycode_menu

f biglist_pollsubsys_maybe @ 0x0804eb64
af+ 0x0804eb64 152 biglist_pollsubsys_maybe
af+ 0x0804fc32 2 dummy_0x0804fc32
af+ 0x0804fc2e 2 dummy_0x0804fc2e
af+ 0x0804fc2e 2 dummy_0x0804fc2e
af+ 0x0804fc26 2 dummy_0x0804fc26
af+ 0x0804fc28 2 dummy_0x0804fc28
af+ 0x0804fc2a 2 dummy_0x0804fc2a
af+ 0x0804fc2c 2 dummy_0x0804fc2c

# modes
f gui_opmode1 @ 0x2001e94d
f gui_opmode2 @ 0x2001e94b
f gui_opmode3 @ 0x2001e892

af+ 0x801eb00 1436 handle_keycode_F_4171

f dispatch_event @ 0x0803c39c

# events

# 0x20017468

f event4_mbox_poi @ 0x2001e660
f event2_mbox_poi_beep @ 0x2001e67c
# 0x20017468
f event1_mbox_poi_radio @ 0x2001e65c 
# 0x20017348
f event3_mbox_poi @ 0x2001e664
# 20017438
f event5_mbox_poi @ 0x2001e658

f event1_buffer @ 0x2001e8aa

# event5 events
f ev5_1 @ 0x803b958
f ev5_2 @ 0x803b980
f ev5_3 @ 0x803b986
f ev5_4_8 @ 0x803b9a4
f ev5_12 @ 0x803ba14
f ev5_14 @ 0x803bf2a
f ev5_15 @ 0x803ba5e
f ev5_16 @ 0x803bad8
f ev5_17 @ 0x803baec
f ev5_18 @ 0x803bc86
f ev5_19 @ 0x803bd46
f ev5_20 @ 0x803be8e
f ev5_21 @ 0x803b9c2
f ev5_22 @ 0x803b9fa
f ev5_rest @ 0x803bf5a

# radioevents

f handle_inter_request_deny @ 0x08040690
f handle_inter_sendstart @ 0x080406a2
f handle_inter_sendstop @ 0x080406b4
f handle_inter_lateentry @ 0x080406c6
f handle_inter_recvdata @ 0x0804077a
f handle_inter_recvmessage @ 0x080409ac
f handle_inter_quit @ 0x080409be
f handle_inter_phy @ 0x080409d0

f jump_if_b7_0 @ 0x080406ce

f some_radio_state @ 0x2001e8b0 
f some_radio_state_prev @ 0x2001e8af

f re_test_for_04 @ 0x08041da8

f re_wait_for_event @ 0x0803c7fc

f re_handle_1 @ 0x0803c85c
f re_handle_2 @ 0x0803ca2c
f re_handle_3 @ 0x0803c8cc
f re_handle_4 @ 0x0803c956
f re_handle_5 @ 0x0803c984
f re_handle_7 @ 0x0803c9fc
f re_handle_8 @ 0x0803ca08
f re_handle_9 @ 0x0803ca14
f re_handle_a @ 0x0803ca20
f re_handle_e @ 0x0803ca3a

f re_last_radio_event @ 0x2001e8a1
    
f phone_ringing @ 0x0804df0e

# tasks

f create_Sys_Inter @ 0x08046548
f create_RTC_Timer @ 0x0804657c
f create_State_Change @ 0x080467b8
f create_ChAccess_Pr @ 0x0804678a

f task_state_change @ 0x0803c330
f task_rtc_timer @ 0x080467cc

# memory
f write_current_channel_info_to_spi @ 0x08022cac
af+ 0x08022cac 26 write_current_channel_info_to_spi
f write_current_channel_info_to_spi_long @ 0x080231a8
af+ 0x080231a8 26 write_current_channel_info_to_spi_long

f selected_channel @ 0x2001e850
#CCa 0x2001e850 selected_channel

f current_channel_info @ 0x2001deb8

# c5000
f c5000_pll_init @ 0x0803f95c
f c5000_strange_init @ 0x0803f982
f c5000_iffreq_init @ 0x0803f9c6

f c5000_handle_0 @ 0x080408a4
f c5000_handle_1 @ 0x080408ba
f c5000_handle_2 @ 0x080408d0
f c5000_handle_3 @ 0x080408e6
f c5000_handle_4 @ 0x08040912
f c5000_handle_5 @ 0x08040914
f c5000_handle_6 @ 0x08040916
f c5000_handle_7 @ 0x0804092c
f c5000_handle_8 @ 0x08040944
f c5000_handle_9 @ 0x0804095c
f c5000_handle_A @ 0x0804098a
f c5000_handle_B_F @ 0x080409a2

f c5000_handle_Voice_LC @ 0x080408ba
f c5000_handle_Terminator_LC @ 0x080408d0
f c5000_handle_CSBK @ 0x080408e6
f c5000_handle_DataHeader @ 0x08040916
f c5000_handle_DataRate1_2 @ 0x0804092c
f c5000_handle_DataRate3_4 @ 0x08040944
f c5000_handle_DataRate1 @ 0x0804098a
f c5000_handle_PI @ 0x080408a4
f c5000_handle_idle @ 0x0804095c

f c5000_init_lowregs @ 0x0803fed4
f c5000_enable_audio @ 0x08040226

f c5000_wr_60_1 @ 0x0803ff16
f c5000_wr_60_2 @ 0x08040230
f c5000_wr_60_3 @ 0x08040382
f c5000_wr_60_4 @ 0x08046b32
f c5000_wr_60_5 @ 0x0805055a


af+ 0x0804dd36 24 some_func_pend
af+ 0x0804dd4e 20 some_func_post
af+ 0x0804dc84 2 some_bitband_io
af+ 0x0804dcf6 28 some_bitband_io_range

af+ 0x080402f8 218 c5000_some2
af+ 0x08040290 86 c5000_some3

af+ 0x0803ff84 38 c5000_spi0_writereg
af+ 0x0803ffd0 42 c5000_spi0_readreg
af+ 0x0803ffaa 38 c5000_spi0_writereg_1

f c5000_spi0_readreg_maybe @ 0x0803fffa
af+ 0x0803fffa 46 c5000_spi0_readreg_maybe

f c5000_maybe_read_packet @ 0x08040028
af+ 0x08040028 52 c5000_maybe_read_packet

f c5000_read_inter @ 0x08040680
f c5000_read_dll_cc @ 0x080406ec
f c5000_read_dll_datatype @ 0x080407a8

f c5000_dispatch_dll_datatype @ 0x0804087e
f c5000_check_for_lcss_continue @ 0x08040858
f c5000_jump_if_vod @ 0x0804087a

f dmr_pi_dummy @ 0x8040a00

f is_this_the_check_for_group_rx_list @ 0x08040b94

f msg_send_maybe @ 0x08024fb0
af+ 0x08024fb0 212 msg_send_maybe

f msg_send_maybe2 @ 0x08024dbc
af+ 0x08024dbc 166 msg_send_maybe2

f msg_create_menu_item_something @ 0x08024448
af+ 0x08024448 2 msg_create_menu_item_something

f msg_convert @ 0x08027656
af+ 0x08027656 112 msg_convert

af+ 0x08024658 190 msg_sms_post_showack 
f msg_sms_post_showack @ 0x08024658

f msg_sms_hdr @ 0x2001e1d0
f msg_sms_bdy @ 0x2001cefc

f msg_sms_hdr_prep @ 0x2001cb54
f msg_sms_bdy_prep @ 0x2001ccbc

f msg_bdy_prep_to_editbuf @ 0x08024402
f msg_editbuf_to_hdr_prep @ 0x080243d6 
f msg_stack_to_hdr_prep_options @ 0x080243b0

f msg_flash_write @ 0x08022fe8
f msg_process_sms2 @ 0x0803dd8c
f msg_process_sms @ 0x0803dd0c
af+ 0x0803dd0c 260 msg_process_sms

# ((( mag

f dispatch_event5_mbox @ 0x0803b8f4

f msg_last_event @ 0x2001e8f4
f msg_0x2001e8f3 @ 0x2001e8f3
f msg_status_flag1 @ 0x2001e8f5
f msg_timer_500 @ 0x2001e5d0
f msg_sms_flags_shifted @ 0x2001e895
f msg_dest_addr @ 0x2001e5dc

f msg_handle_types @ 0x0804c858
af+ 0x0804c858 2 msg_handle_types
f msg_handle_type_11 @ 0x804ca3c
f msg_handle_type_21 @ 0x804c888
f msg_handle_type_31 @ 0x804ca3c
f msg_handle_type_other @ 0x804cb00

f msg_handle_types @ 0x0804c858


# msg )))


f msg_f1 @ 0x08024ec4
af+ 0x08024ec4 162 msg_f1
f msg_f2 @ 0x08024f66
af+ 0x08024f66 64 msg_f2
f msg_f4_spi_rd @ 0x08022ebc
af+ 0x08022ebc 18 msg_f3_spi_rd

f spiflash_read_3ff30_288 @ 0x08022e88
af+ 0x08022e88 26 spiflash_read_3ff30_288
f spiflash_write_3ff30_288 @ 0x08022ea2
af+ 0x08022ea2 26 spiflash_write_3ff30_288


f msg_buffer @ 0x2001db2c
f msg_buff_complete @ 0x20018490

f c5000_master_handler @ 0x08041cfc

f sema1_poi @ 0x2001e650
f sema2_poi @ 0x2001e670 

#
f simplex_or_repeater_flagword @ 0x2001e898


#

f some_state_var @ 0x2001e8b8
f some_state_var2 @ 0x2001e534
f convert_freq_to_str @ 0x800e398

#

f c5000_set_local_addr @ 0x0803cae0
af+ 0x0803cae0 84 c5000_set_local_addr

f c5000_set_local_addr2 @ 0x0803cb34
af+ 0x0803cb34 96 c5000_set_local_addr2

f flash_write_50_at_40000 @ 0x08022e76
af+ 0x08022e76 18 flash_write_50_at_40000

f flash_read_50_at_40000 @ 0x08022e64
af+ 0x08022e64 18 flash_read_50_at_40000

f menu_add_number_of_menuentries_counts @ 0x0800fc54
af+ 0x0800fc54 48 menu_add_number_of_menuentries_counts

f menu_memory_poi @ 0x2001e700

#
f backlight_timer @ 0x2001e7f8 
f md380_menu_0x2001d3c1 @ 0x2001e914 
f OSTaskCreateExt @ 0x0804e580 
f ambe_outbuffer0 @ 0x20013f28 
f main_menu @ 0x0803b39a 
f Start_multiple_tasks @ 0x08046520 
f botlinetext @ 0x2001e410 
f md380_dfu_state @ 0x2001e962 
f ambe_decode_wav @ 0x08053680 
f ambe_encode_thing @ 0x080531d8 
f beep_process_unkown @ 0x2001e6c0 
f md380_RTC_GetDate @ 0x0802b50c 
f md380_blockadr @ 0x2001e754 
f md380_f_4137 @ 0x0802dfbc 
f aes_loadkey @ 0x08036cc0 
f OS_EXIT_CRITICAL @ 0x08043dec 
f usb_dnld_handle @ 0x0808ebee 
f dmr_before_squelch @ 0x08040ce6 
f md380_dfu_target_adr @ 0x20004a14 
f usb_do_setup @ 0x0808eb30 
f aes_startup_check @ 0x0802256a 
f md380_usbbuf @ 0x2001e0d0 
f welcomebmp @ 0x080f8510 
f Start_2_more_tasks__init_vocoder_tasks @ 0x08049e14 
f OSTaskNameSet @ 0x0804e64c 
f ambe_outbuffer1 @ 0x20013fc8 
f usb_send_packet @ 0x08059b02 
f OSSemPend @ 0x0803f754 
f md380_f_4102 @ 0x0804ec66 
f OS_ENTER_CRITICAL @ 0x08043de4 
f usb_serialnumber @ 0x0809662e 
f OSSemCreate @ 0x0803f708 
f md380_wt_programradio @ 0x080cff30 
f dmr_call_end @ 0x08041430 
f dmr_sms_arrive @ 0x08040de0 
f Get_Welcome_Line1_from_spi_flash @ 0x080226c0 
f aes_cipher @ 0x08036c38 
f c5000_spi0_readreg @ 0x0803ffd0 
f md380_packet @ 0x2001ae74 
f gfx_font_norm @ 0x080cf780 
f Start @ 0x080462bc 
f OSTimeDly @ 0x08033eb4 
f usb_upld_handle @ 0x0808f308 
f dmr_call_start @ 0x08040a02 
f md380_spiflash_read @ 0x08031476 
f ambe_inbuffer @ 0x2001410e 
f md380_packetlen @ 0x2001e758 
f md380_spiflash_disable @ 0x08031546 
f md380_RTC_GetTime @ 0x0802b3f6 
f ambe_unpack @ 0x0804b234 
f md380_spiflash_block_erase64k @ 0x080312aa 
f md380_program_radio_unprohibited @ 0x2001e574 
f c5000_spi0_writereg @ 0x0803ff84 
f usb_dfu_write @ 0x08090370 
f Beep_Process @ 0x0802f9dc 
f md380_thingy2 @ 0x2001e963 
f md380_spiflash_enable @ 0x0803152a 
f Edit_Message_Menu_Entry @ 0x08023ee4 
f md380_spi_sendrecv @ 0x080314bc 
f md380_menu_entry_selected @ 0x2001e903 
f usb_setcallbacks @ 0x08055100 
f gfx_font_small @ 0x0809a4c0 
f Get_Welcome_Line2_from_spi_flash @ 0x080226d2 
f md380_spiflash_wait @ 0x08031508 
f md380_usbstring @ 0x2001d504 
f md380_spiflash_sektor_erase4k @ 0x08031276 
f OSSemPost @ 0x0803f844 
f md380_OSMboxPost @ 0x0803119c 
f usb_dfu_read @ 0x080903c0 
f dmr_handle_data @ 0x0804dd70 
f ambe_mystery @ 0x20013594 
f md380_spiflash_write @ 0x0803155e 
f md380_spiflash_security_registers_read @ 0x080318b0 
f toplinetext @ 0x2001e3fc 
f md380_menu_0x2001d3f0 @ 0x2001b246 
f md380_create_main_meny_entry @ 0x0800c188 
f md380_create_menu_entry @ 0x0800c72e 
f Create_Menu_Entry_RX_QRG_2 @ 0x080157fc 
f md380_itow @ 0x08018b28 
f md380_menu_numerical_input @ 0x0801b042 
