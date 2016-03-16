# flash.r by Travis Goodspeed

# This is a Radare2 script for annotating the Tytera MD380
# application, version 2.032, which begins at 0x0800C000 in Flash
# memory.  Preceding it is a bootloader, which can be either the
# official factory bootloader or an aftermarket one.


# Begin by opening the application in R2 with this script.
# r2 -a arm -m 0x0800C000 -b 16 -i flash.r ../../firmware/D002.032.bin

# MD5 (../../firmware/D002.032.bin) = 8295594d00cb705eac7cd812642fccf2


## These annotations describe the functions that handle incoming
## audio, which is necessary to

CCa 0x803ec86 This function handles incoming audio calls.
af+ 0x803ec86 740 dmr_audio_start

CCa 0x0803ee36 Nop this to crudely match on all public calls.
CCa 0x0803ef10 Nop this to crudely match on all private calls.

CCa 0x0803ee14 R5 will count through each public talk group.
CCa 0x0803ee1e If none matched, jump to where we'll mute the audio.
CCa 0x0803ee80 Here, we've tried to match all of the talk groups.
CCa 0x0803ee84 Skip ahead a bit if a match has been found.

CCa 0x0803ee66 Value of 9 enables audio reception.
CCa 0x0803ee8a Value of 8 enables light, but audio is muted.




af+ 0x8043e78 268 F_4000
af+ 0x80969de 16 F_4001
af+ 0x808cc00 190 F_4002
af+ 0x808cba4 38 F_4003
af+ 0x808d3d8 3036 F_4004
af+ 0x808dfd0 520 F_4005
af+ 0x808e3ca 42 F_4006
af+ 0x808e3f4 76 F_4007
af+ 0x808e440 80 F_4008
af+ 0x808e490 54 F_4009
af+ 0x808e4c6 96 F_4010
af+ 0x808e526 44 F_4011
af+ 0x8091fb8 106 F_4012
af+ 0x8092022 54 F_4013
af+ 0x8092058 342 F_4014
af+ 0x80921ae 284 F_4015
af+ 0x80922ca 416 F_4016
af+ 0x809246a 128 F_4017
af+ 0x80924ea 104 F_4018
af+ 0x8092552 416 F_4019
af+ 0x80926f2 148 F_4020
af+ 0x8092786 10 F_4021
af+ 0x8092790 194 F_4022
af+ 0x8092852 72 F_4023
af+ 0x809289a 90 F_4024
af+ 0x8092a4c 1916 F_4025
af+ 0x80931c8 210 F_4026
af+ 0x809329a 72 F_4027
af+ 0x80932e2 136 F_4028
af+ 0x809336a 244 F_4029
af+ 0x809345e 202 F_4030
af+ 0x8093528 28 F_4031
af+ 0x8093544 158 F_4032
af+ 0x80935e2 136 F_4033
af+ 0x809366a 186 F_4034
af+ 0x8093724 74 F_4035
af+ 0x809376e 28 F_4036
af+ 0x809378a 28 F_4037

af+ 0x800d7f4 114 F_4038
af+ 0x800d86c 30 F_4039
af+ 0x802a87c 24 F_4040
af+ 0x8030a52 284 F_4041
af+ 0x8032cc2 18 F_4042
af+ 0x80426a8 30 F_4043
af+ 0x80495d4 28 F_4044
af+ 0x804fa2e 50 F_4045
af+ 0x8057754 70 F_4046
af+ 0x805f242 42 F_4047
af+ 0x805f4fc 20 F_4048
af+ 0x805f510 18 F_4049
af+ 0x805f6fa 44 F_4050
af+ 0x805f726 74 F_4051
af+ 0x805faea 14 F_4052
af+ 0x805faf8 20 F_4053
af+ 0x805fb0c 14 F_4054
af+ 0x808d266 132 F_4055
af+ 0x808d2ea 134 F_4056
af+ 0x808e1dc 100 F_4057
af+ 0x808e244 12 F_4058
af+ 0x808e254 98 F_4059
af+ 0x808e3a0 42 F_4060
af+ 0x809290a 72 F_4061
af+ 0x8092954 242 F_4062
af+ 0x80937b0 32 F_4063

af+ 0x80226f6 18 F_4064
af+ 0x802271a 18 F_4065
af+ 0x8026028 110 F_4066
af+ 0x8057136 22 F_4067
af+ 0x805714c 22 F_4068
af+ 0x805779a 20 F_4069
af+ 0x80577ec 40 F_4070
af+ 0x805f4f2 10 F_4071

af+ 0x804ce10 258 F_4072
af+ 0x804f61a 54 F_4073
af+ 0x804f508 78 F_4074
af+ 0x804f556 138 F_4075
af+ 0x804f5e0 58 F_4076
af+ 0x804fa60 48 F_4077
af+ 0x804fb00 22 F_4078
af+ 0x804fb16 170 F_4079
af+ 0x804fbc0 10 F_4080
af+ 0x804fbca 90 F_4081
af+ 0x804fc28 40 F_4082

af+ 0x8017612 56 F_4083
af+ 0x808ccbe 1444 F_4084

af+ 0x8017650 124 F_4085
af+ 0x801c4b0 94 F_4086
af+ 0x802a3fc 168 F_4087
af+ 0x802a824 88 F_4088
af+ 0x8035556 8 F_4089
af+ 0x804c0fe 82 F_4090
af+ 0x808cb84 30 F_4091
af+ 0x808e2bc 96 F_4092

af+ 0x80226e4 18 F_4093
af+ 0x8022708 18 F_4094
af+ 0x802a39c 96 F_4095
af+ 0x802fbb6 52 F_4096
af+ 0x804c1a8 40 F_4097

af+ 0x804c1d0 112 F_4098
af+ 0x804cd16 240 F_4099
af+ 0x804cf80 614 F_4100 

af+ 0x804c156 58 F_4101
af+ 0x804c2aa 286 F_4102
af+ 0x804c408 2 F_4103
af+ 0x804c458 518 F_4104
af+ 0x804c6a8 478 F_4105
af+ 0x804c8d4 486 F_4106
af+ 0x804cb08 438 F_4107
af+ 0x804ccc0 2 F_4108
af+ 0x804cf18 48 F_4109
af+ 0x804d25a 2 F_4110
af+ 0x804d25e 2 F_4111
af+ 0x804d262 2 F_4112
af+ 0x804d266 2 F_4113

af+ 0x8042282 96 F_4114
af+ 0x8042342 58 F_4115
af+ 0x804237c 94 F_4116

CCa 0x08041e24 PENDSVSET .. changes PendSV exception state to pending
CCa 0x08041e2c PENDSVSET .. changes PendSV exception state to pending 

af+ 0x800c188 1448 F_84
af+ 0x800c730 86 F_249
af+ 0x800d88a 36 F_85
af+ 0x800d8ae 254 F_785
af+ 0x800da54 124 F_786
af+ 0x800dd40 284 F_1056
af+ 0x800de5c 96 F_972
af+ 0x800debc 126 F_787
af+ 0x800f422 48 F_251
af+ 0x800f464 32 F_86
af+ 0x8012528 148 F_253
af+ 0x80172ec 236 F_973
af+ 0x801b440 30 F_974
af+ 0x801b45e 24 F_975
af+ 0x801b476 118 F_1091
af+ 0x801b4ec 10 F_976
af+ 0x801b4f6 34 F_1057
af+ 0x801b518 68 F_977
af+ 0x801b55c 68 F_788
af+ 0x801b5a0 80 F_789
af+ 0x801b5f0 142 F_1092
af+ 0x801b67e 88 F_1058
af+ 0x801b6d6 44 F_978
af+ 0x801b702 36 F_979
af+ 0x801b726 20 F_790
af+ 0x801b73a 46 F_980
af+ 0x801b768 10 F_1093
af+ 0x801b772 54 F_981
af+ 0x801b7a8 54 F_1094
af+ 0x801b7de 36 F_1125
af+ 0x801b802 178 F_982
af+ 0x801b8b4 152 F_983
af+ 0x801b94c 132 F_984
af+ 0x801b9d0 38 F_1059
af+ 0x801b9f6 20 F_985
af+ 0x801ba0a 24 F_1151
af+ 0x801ba22 18 F_791
af+ 0x801ba34 120 F_1060
af+ 0x801baac 12 F_986
af+ 0x801bab8 44 F_1061
af+ 0x801bae4 332 F_1062
af+ 0x801bc30 28 F_987
af+ 0x801bc4c 208 F_792
af+ 0x801bd1c 68 F_988
af+ 0x801bd6a 418 F_793
af+ 0x801bf0c 92 F_254
af+ 0x801bf68 288 F_255
af+ 0x801c494 8 F_989
af+ 0x801c50e 120 F_794
af+ 0x801c5e0 8 F_795
af+ 0x801c5e8 8 F_796
af+ 0x801ca42 36 F_1063
af+ 0x801ca66 12 F_1095
af+ 0x801ca72 32 F_1064
af+ 0x801ca92 52 F_990
af+ 0x801cb04 30 F_797
af+ 0x801cbca 2 F_991
af+ 0x801cbcc 10 F_992
af+ 0x801ced2 40 F_993
af+ 0x801cf1c 18 F_798
af+ 0x801cf2e 18 F_799
af+ 0x802079c 294 F_800
af+ 0x8020974 16 F_801
af+ 0x802099c 154 F_994
af+ 0x8020a40 18 F_802
af+ 0x8020a58 3020 F_28
af+ 0x8021624 48 F_30
af+ 0x8021654 30 F_31
af+ 0x8021672 26 F_32
af+ 0x802168c 484 F_803
af+ 0x8021870 18 F_804
af+ 0x8021882 18 F_258
af+ 0x8021894 18 F_259
af+ 0x80218a6 18 F_805
af+ 0x80218d4 474 F_806
af+ 0x8021ac0 18 F_807
af+ 0x8021ad2 26 F_808
af+ 0x8021aec 22 F_995
af+ 0x8021b02 36 F_809
af+ 0x8021b6e 116 F_810
af+ 0x8021be2 26 F_811
af+ 0x8021bfc 156 F_812
af+ 0x8021c98 362 F_260
af+ 0x8021e1c 26 F_261
af+ 0x8021e4c 26 F_813
af+ 0x8021e66 26 F_814
af+ 0x8021e80 24 F_815
af+ 0x8021e98 24 F_816
af+ 0x8021f22 18 F_817
af+ 0x8021f34 18 F_818
af+ 0x8021f46 18 F_1065
af+ 0x80220f8 26 F_819
af+ 0x8022112 26 F_262
af+ 0x8022144 26 F_263
af+ 0x80221d6 18 F_264
af+ 0x80225b8 18 F_820
af+ 0x80227e8 158 F_265
af+ 0x8022886 86 F_266
af+ 0x8025e6c 362 F_1066
af+ 0x8025fdc 76 F_996
af+ 0x8026096 148 F_267
af+ 0x802612a 12 F_821
af+ 0x8026136 114 RTC_Init
af+ 0x8026138 112 F_1146
af+ 0x80261a8 14 F_998
af+ 0x80261b6 84 RTC_EnterInitMode
af+ 0x802620a 18 RTC_ExitInitMode
af+ 0x802621c 100 RTC_WaitForSynchro
af+ 0x8026280 202 RTC_SetTime
af+ 0x802634a 80 RTC_GetTime
af+ 0x802639a 198 RTC_SetDate
af+ 0x8026460 76 RTC_GetDate
af+ 0x80264ac 56 RTC_WakeUpClockConfig
af+ 0x80264e4 32 RTC_SetWakeUpCounter
af+ 0x8026504 122 RTC_RefClockCmd
af+ 0x8026594 2 F_822
af+ 0x8026596 24 something_to_store_to_RTC_backup_registers
af+ 0x80265b8 86 F_999
af+ 0x80265ba 84 RTC_ITConfig
af+ 0x8026614 2 F_1000
af+ 0x8026616 28 RTC_ClearFlag
af+ 0x802664c 22 F_92
af+ 0x8026662 24 RTC_Bcd2ToByte
af+ 0x802669c 70 F_276
af+ 0x80266e2 100 F_277
af+ 0x8026754 106 F_278
af+ 0x80267be 266 F_94
af+ 0x80268c8 122 F_1097
af+ 0x80269f4 166 F_95
af+ 0x8026a9a 22 GPIO_ReadInputDataBit
af+ 0x8026ab0 6  GPIO_ReadInputData
af+ 0x8026ab6 4 GPIO_SetBits
af+ 0x8026aba 4 GPIO_ResetBits
af+ 0x8026abe 14 GPIO_WriteBit
af+ 0x8026acc 82 F_96
af+ 0x8027a52 66 F_823
af+ 0x80286e8 580 F_286
af+ 0x8028960 674 F_287
af+ 0x802a1c0 324 F_288
af+ 0x802a8b0 48 F_289
af+ 0x802a8e0 56 F_99
af+ 0x802a918 36 F_291
af+ 0x802a93c 24 F_292
af+ 0x802a960 4084 Beep_Process
af+ 0x802ba44 52 F_293
af+ 0x802ba78 16 F_294
af+ 0x802ba88 32 F_295
af+ 0x802baa8 80 F_296
af+ 0x802baf8 72 F_297
af+ 0x802bb40 372 Af_Mute
af+ 0x802bcb8 340 Set_Vocoder
af+ 0x802be0c 340 F_107
af+ 0x802bf76 2 F_108_Nice
af+ 0x802e8d8 166 F_1001
af+ 0x802e9f0 54 F_824
af+ 0x802fb00 130 F_300
af+ 0x802fb82 52 F_1068
af+ 0x802fbea 76 F_1098
af+ 0x802fc36 332 F_1069
af+ 0x802fd82 70 F_301
af+ 0x802fdc8 58 F_302
af+ 0x802fe02 18 F_303
af+ 0x802fe14 34 F_304
af+ 0x802fe36 28 F_305
af+ 0x802fe52 24 F_65
af+ 0x802fe6a 704 F_1002
af+ 0x803013c 52 F_307
af+ 0x8030170 76 F_308
af+ 0x80301bc 78 F_309
af+ 0x8030250 76 F_310
af+ 0x803029c 258 F_311
af+ 0x80303b4 86 F_05
af+ 0x80308fe 142 F_33
af+ 0x803098c 198 F_314
af+ 0x8030b6e 50 F_110
af+ 0x8030ba0 184 F_316
af+ 0x8030c58 70 F_317
af+ 0x8030c9e 80 F_08
af+ 0x8030cee 68 F_09
af+ 0x8030e36 40 F_320
af+ 0x8030e5e 92 F_26
af+ 0x8030eba 92 F_322
af+ 0x8030f16 102 F_323
af+ 0x803114c 10 F_1152
af+ 0x8031166 152 F_1070
af+ 0x80311fe 38 F_1153
af+ 0x8031224 100 F_1154
af+ 0x8031288 170 F_1155
af+ 0x8031332 16 F_1156
af+ 0x8031342 276 F_1138
af+ 0x8031456 62 F_1157
af+ 0x8031494 22 F_1115
af+ 0x80314aa 24 F_825
af+ 0x803154c 16 F_1003
af+ 0x803156e 136 F_1071
af+ 0x80315f6 126 F_1116
af+ 0x8031674 118 F_826
af+ 0x8031694 62 F_67
af+ 0x80316ec 26 F_1072
af+ 0x8031710 66 F_827
af+ 0x8031752 30 F_1004
af+ 0x8031770 56 F_1073
af+ 0x80317a8 44 F_1099
af+ 0x80317d4 78 F_1100
af+ 0x8031822 18 F_1005
af+ 0x8031834 16 F_828
af+ 0x8031844 20 F_325
af+ 0x8031858 20 F_326
af+ 0x803186c 56 F_829
af+ 0x8031944 84 F_830
af+ 0x8031998 540 F_831
af+ 0x8031bb4 14 F_327
af+ 0x8031bc2 54 F_832
af+ 0x8031bf8 12 F_328
af+ 0x8031c10 24 F_1006
af+ 0x8031c28 24 F_1007
af+ 0x8031e20 80 F_833
af+ 0x8031e70 110 F_329
af+ 0x8031ede 28 F_111
af+ 0x8031efa 32 F_331
af+ 0x8031f40 36 F_1074
af+ 0x8031f64 12 F_1008
af+ 0x8031f74 8 F_834
af+ 0x8031f7c 6 F_835
af+ 0x8031f82 12 F_1139
af+ 0x8031f8e 20 F_1101
af+ 0x8031fb0 20 F_1102
af+ 0x8031fc4 392 F_1075
af+ 0x803214c 72 F_332
af+ 0x8032194 68 F_1117
af+ 0x80321d8 218 F_836
af+ 0x80322b2 216 F_837
af+ 0x8032620 348 F_838
af+ 0x8032628 136 F_3000
af+ 0x80326b0 16 F_1010
af+ 0x80326c4 26 F_1011
af+ 0x80326de 54 F_1148
af+ 0x8032714 66 F_1149
af+ 0x8032756 16 F_1012
af+ 0x8032766 14 F_1150
af+ 0x803277c 8 F_840
af+ 0x8032784 8 F_841
af+ 0x8032790 24 F_333
af+ 0x8032838 52 F_112
af+ 0x803286c 320 F_1013
af+ 0x80329c0 104 F_335
af+ 0x8032a3c 146 Function_Function_Function_Function_Called_Big_I2C_Function
af+ 0x8032ace 28 F_337
af+ 0x8032aea 10 F_114
af+ 0x8032af4 24 F_115
af+ 0x8032b0c 28 F_116
af+ 0x8032b82 36 F_117
af+ 0x8032ba6 36 F_118
af+ 0x8032c2e 18 F_343
af+ 0x8032c90 50 F_120
af+ 0x80353f0 8 F_1080
af+ 0x80353f8 50 F_1014
af+ 0x803542a 10 F_1015
af+ 0x8035434 10 F_345
af+ 0x803543e 226 F_121
af+ 0x8035520 46 F_1016
af+ 0x803554e 8 F_1017
af+ 0x803555e 32 F_347
af+ 0x803557e 32 F_842
af+ 0x803559e 32 F_843
af+ 0x80355be 32 RCC_APB1_peripheral_clock_enable_register
af+ 0x80355de 32 RCC_APB2_peripheral_clock_enable_register
af+ 0x80355fe 68 F_1018
af+ 0x803568c 8 F_350
af+ 0x8035694 10 F_351
af+ 0x80356a8 8 F_352
af+ 0x80356b0 136 F_353
af+ 0x8035738 236 F_354
af+ 0x8035824 92 F_844
af+ 0x8035880 42 F_355
af+ 0x80358aa 68 F_356
af+ 0x80358ee 148 F_357
af+ 0x8035982 46 F_358
af+ 0x80359bc 8 F_845
af+ 0x80359c4 64 F_1019
af+ 0x8035a08 8 F_846
af+ 0x8035a10 2 F_1020
af+ 0x8036344 22 F_1107
af+ 0x8036c74 54 F_359
af+ 0x803881c 18 F_1081
af+ 0x8039e74 12 F_1108
af+ 0x8039ea0 100 F_1109
af+ 0x8039f04 66 F_1082
af+ 0x8039f46 62 F_1083
af+ 0x8039f84 186 F_1084
af+ 0x803a04c 100 F_1110
af+ 0x803a0b0 36 F_1085
af+ 0x803a0d8 28 F_1111
af+ 0x803a160 90 F_360
af+ 0x803a1ba 132 F_361
af+ 0x803a23e 104 F_362
af+ 0x803af70 92 F_847
af+ 0x803afd8 92 F_848
af+ 0x803b044 58 F_363
af+ 0x803b15c 26 F_364
af+ 0x803b176 32 F_365
af+ 0x803b2e8 90 F_849
af+ 0x803b342 100 F_850
af+ 0x803b3a6 74 F_366
af+ 0x803b3f0 36 F_367
af+ 0x803b414 20 F_368
af+ 0x803b428 70 F_851
af+ 0x803b46e 92 F_369
af+ 0x803b510 14 F_852
af+ 0x803b524 2286 Call_Process
af+ 0x803be24 992 State_Change
af+ 0x803c244 772 ChAccess_Pr
af+ 0x803c57c 86 F_39
af+ 0x803c5d8 90 F_853
af+ 0x803c632 52 F_371
af+ 0x803c666 50 F_372
af+ 0x803c6b4 680 F_40
af+ 0x803c998 808 F_374
af+ 0x803cccc 590 F_375
af+ 0x803cf1a 62 F_41
af+ 0x803cf58 36 F_377
af+ 0x803cf7c 46 F_42
af+ 0x803cfb4 388 F_379
af+ 0x803d140 102 F_380
af+ 0x803d1fc 40 F_381
af+ 0x803d224 70 F_382
af+ 0x803d26a 62 F_383
af+ 0x803d2a8 40 F_384
af+ 0x803d2d0 40 F_385
af+ 0x803d2f8 40 F_854
af+ 0x803d320 38 F_386
af+ 0x803d346 40 F_387
af+ 0x803d36e 44 F_855
af+ 0x803d39a 38 F_388
af+ 0x803d3c0 40 F_389
af+ 0x803d3e8 40 F_390
af+ 0x803d410 44 F_856
af+ 0x803d43c 374 F_391
af+ 0x803d5c0 146 F_136
af+ 0x803d660 94 F_137
af+ 0x803d6d8 214 F_138
af+ 0x803d7c0 282 F_395
af+ 0x803d8da 192 F_396
af+ 0x803da00 44 F_857
af+ 0x803da2c 76 F_397
af+ 0x803da78 218 F_398
af+ 0x803db68 92 F_21
af+ 0x803dbc4 28 Sys_Inter
af+ 0x803dbe2 52 TimeSlot_Inter
af+ 0x803dc16 1206 F_43
af+ 0x803e0cc 476 F_44
af+ 0x803e2a8 38 F_402
af+ 0x803e2ce 38 F_68
af+ 0x803e2f4 42 F_404
af+ 0x803e31e 46 F_405
af+ 0x803e34c 52 F_48
af+ 0x803e380 332 F_407
af+ 0x803e4dc 68 F_408
af+ 0x803e528 44 F_409
af+ 0x803e594 86 F_410
af+ 0x803e600 218 F_411
af+ 0x803e6e4 196 F_412
af+ 0x803e7a8 340 F_413
af+ 0x803e900 868 F_414
af+ 0x803ec84 2 F_415
af+ 0x803ef6c 92 F_416
af+ 0x803efc8 90 F_858
af+ 0x803f03c 728 F_417
af+ 0x803f33c 818 F_418
af+ 0x803f6d8 1134 F_419
af+ 0x803fbcc 5656 F_420
af+ 0x80411e8 68 F_421
af+ 0x8041244 652 F_422
af+ 0x8041578 52 F_46
af+ 0x80415ac 390 F_424
af+ 0x804176c 106 F_425
af+ 0x80417d6 24 F_859
af+ 0x80417ee 6 F_426
af+ 0x80417f4 4 F_427
af+ 0x80417f8 4 F_428
af+ 0x80417fc 4 F_146_Nice
af+ 0x8041800 28 F_860
af+ 0x804181c 28 F_430
af+ 0x8041838 126 F_861
af+ 0x80418b6 140 F_862
af+ 0x8041942 138 F_863
af+ 0x80419cc 104 F_864
af+ 0x8041a54 34 F_865
af+ 0x8041a7e 18 F_866
af+ 0x8041a90 20 F_867
af+ 0x8041aa4 18 F_868
af+ 0x8041ab6 34 F_431
af+ 0x8041ad8 24 F_432
af+ 0x8041b1e 8 F_433
af+ 0x8041b26 18 F_869
af+ 0x8041b58 2 F_1086
af+ 0x8041b5c 64 F_870
af+ 0x8041b9c 256 F_434
af+ 0x8041cb0 28 F_871
af+ 0x8041ccc 28 F_435
af+ 0x8041ce8 86 F_436
af+ 0x8041d3e 4 F_147_Nice
af+ 0x8041d42 4 F_148_Nice
af+ 0x8041d46 24 F_439
af+ 0x8041d5e 44 F_440
af+ 0x8041d8a 20 F_441
af+ 0x8041df8 8 OS_ENTER_CRITICAL
af+ 0x8041e00 6 OS_EXIT_CRITICAL
af+ 0x8041e22 8 F_444
af+ 0x8041e2a 8 F_445
af+ 0x8041eac 56 F_446
af+ 0x8041f10 148 function_whith_lot_of_memmove_in_OS_ENTER_CRITICAL_1
af+ 0x8041fa4 58 F_447
af+ 0x8041fde 90 F_448
af+ 0x8042130 122 F_449
af+ 0x80421aa 140 F_450
af+ 0x8042236 50 F_451
af+ 0x8042268 26 F_872
af+ 0x80423ec 92 F_02
af+ 0x8042448 30 mem_transfet
af+ 0x8042468 30 uC_OS_II_Idle
af+ 0x804248c 318 function_whith_lot_of_memmove_in_OS_ENTER_CRITICAL_2
af+ 0x80426c8 536 Init_ADC
af+ 0x80428e0 332 F_455
af+ 0x8042a2c 332 F_456
af+ 0x8042b78 700 F_153
af+ 0x8042e34 162 F_154
af+ 0x8042ed6 120 F_459
af+ 0x8042f4e 62 F_155
af+ 0x8042f8c 396 F_156
af+ 0x8043118 124 F_462
af+ 0x8043194 54 F_157
af+ 0x8043204 934 F_158
af+ 0x80435b0 188 F_465
af+ 0x8043678 42 F_159
af+ 0x80436d4 834 F_160
af+ 0x8043a1c 110 F_468
af+ 0x8043aa0 74 F_161
af+ 0x8043b14 694 F_162
af+ 0x8043dd0 80 F_471
af+ 0x8043e28 64 F_163
af+ 0x8043e70 8 F_164
af+ 0x804403c 310 Start
af+ 0x8044172 298 F_474
af+ 0x804429c 684 F_166
af+ 0x8044548 226 RTC_Timer
af+ 0x8044674 626 FMTx_Process
af+ 0x80448e8 42 RF_Pll
af+ 0x8044900 20 PC_Tune
af+ 0x8044914 32 F_167
af+ 0x8044934 230 F_168
af+ 0x8044ab4 392 F_478
af+ 0x8044c3c 79 F_71
af+ 0x8044c82 678 F_480
af+ 0x8044f8c 118 F_481
af+ 0x8045002 366 F_482
af+ 0x8045198 108 F_483
af+ 0x8045218 96 F_484
af+ 0x80452b8 418 F_485
af+ 0x80454b0 94 F_486
af+ 0x804550e 136 F_873
af+ 0x8045596 70 F_487
af+ 0x80455dc 154 F_169
af+ 0x8045676 124 F_489
af+ 0x80456f2 108 F_73
af+ 0x804575e 50 F_74
af+ 0x8045790 50 F_75
af+ 0x80457c2 246 F_493
af+ 0x80458b8 90 F_494
af+ 0x8045912 40 F_495
af+ 0x804593a 40 F_76
af+ 0x8045962 56 F_77
af+ 0x804599a 94 F_78
af+ 0x80459f8 20 F_79
af+ 0x8045a68 248 F_874
af+ 0x8045b60 90 F_500
af+ 0x8045bc0 614 F_80_wtf
af+ 0x8045e26 288 F_502
af+ 0x8045f46 138 F_503
af+ 0x804601e 88 F_504
af+ 0x8046098 38 F_505
af+ 0x80460be 14 F_1158
af+ 0x80460cc 94 F_1134
af+ 0x804612a 62 F_1123
af+ 0x8046720 18 F_1021
af+ 0x8046734 8 F_1022
af+ 0x8046740 2 F_875
af+ 0x8046790 378 F_1023
af+ 0x804690c 22 Not_Big_I2C_Function33
af+ 0x8046914 14 F_506
af+ 0x8046922 38 F_507
af+ 0x8046948 38 F_508
af+ 0x804696e 186 Not_Big_I2C_Function3
af+ 0x8046a28 306 Big_I2C_Function
af+ 0x8046b5a 236  Verry_Big_Function_with_I2C1
af+ 0x8046c94 2 F_177_Nice
af+ 0x8046c98 8 F_1024
af+ 0x8046ca0 22 PWR_GetFlagStatus
af+ 0x8046cb6 14 Function_PWR_CR__
af+ 0x8046cd0 232 F_515
af+ 0x8046db8 10 F_516
af+ 0x8046dc2 62 F_179
af+ 0x8046e00 156 Function_Function_Function_Called_Big_I2C_Function
af+ 0x8046eb0 2 F_180_Nice
af+ 0x8046eb4 8 F_520
af+ 0x8046ebc 8 F_521
af+ 0x8046ec4 8 F_522
af+ 0x8046ecc 20 F_523
af+ 0x8046ee0 38 F_524
af+ 0x8046f06 38 F_525
af+ 0x8046f2c 38 F_526
af+ 0x8046f52 38 F_527
af+ 0x8046f78 8 F_528
af+ 0x8046f80 14 F_529
af+ 0x8046f8e 14 F_530
af+ 0x8046f9c 8 F_531
af+ 0x8046fa4 8 F_532
af+ 0x8046fac 14 F_533
af+ 0x8046fba 14 F_534
af+ 0x8046fc8 716 F_181
af+ 0x804763e 62 F_536
af+ 0x804767c 62 dma_cmd
af+ 0x80476ba 106 F_538
af+ 0x8047724 2 F_182_Nice
af+ 0x80477dc 10 F_540
af+ 0x80477e6 14 F_541
af+ 0x80477f4 28 F_542
af+ 0x8047810 28 F_543
af+ 0x804782c 28 F_544
af+ 0x8047848 334 F_545
af+ 0x8047996 16 F_183
af+ 0x80479a6 16 F_184
af+ 0x80479b6 24 F_185
af+ 0x80479e8 38 F_549
af+ 0x8047a0e 22 F_186
af+ 0x8047a24 70 F_551
af+ 0x8047b0c 2 F_187_Nice
af+ 0x8047b10 182 F_188
af+ 0x8047c64 2 F_189_Nice
af+ 0x8047c68 16 F_555
af+ 0x8047c7c 734 F_191
af+ 0x80487a8 158 F_557
af+ 0x8048c9c 202 F_558
af+ 0x8048e84 2 F_192_Nice
af+ 0x8048e88 120 F_560
af+ 0x8048f00 130 F_561
af+ 0x8048f82 184 F_562
af+ 0x804903a 14 F_193
af+ 0x8049048 52 F_876
af+ 0x8049164 110 F_194
af+ 0x80491d2 74 F_195
af+ 0x804921c 122 uC_OS_II_Tmr
af+ 0x80492d4 16 F_1112
af+ 0x80495f0 38 F_1136
af+ 0x8049cd8 102 F_877
af+ 0x8049d40 76 F_566
af+ 0x8049d8c 34 F_567
af+ 0x8049dae 26 F_568
af+ 0x8049dc8 212 F_569
af+ 0x8049e9c 10 ADC_SoftwareStartConv
af+ 0x8049eac 26 F_571
af+ 0x8049ec6 26 F_572
af+ 0x8049f08 532 F_1025
af+ 0x804a11c 98 F_1026
af+ 0x804a17e 16 F_878
af+ 0x804a18e 402 F_1087
af+ 0x804a420 90 F_573
af+ 0x804a47a 520 F_879
af+ 0x804a682 232 F_574
af+ 0x804a76a 52 F_575
af+ 0x804a79e 162 F_880
af+ 0x804a840 682 F_576
af+ 0x804aaea 340 F_577
af+ 0x804aca4 198 F_203
af+ 0x804ad6a 68 F_881
af+ 0x804adae 100 F_579
af+ 0x804ae14 92 F_204
af+ 0x804ae70 126 F_581
af+ 0x804aeee 120 F_582
af+ 0x804af68 568 Tone_fft
af+ 0x804b1a0 734 F_205
af+ 0x804b508 120 F_584
af+ 0x804b580 114 F_17
af+ 0x804b5f2 28 F_586
af+ 0x804b60e 36 F_587
af+ 0x804b632 24 F_588
af+ 0x804b64a 20 F_16
af+ 0x804b66c 190 F_590
af+ 0x804b7b0 8 F_206
af+ 0x804b7b8 8 F_207
af+ 0x804b7c0 324 F_882
af+ 0x804b90c 8  short_function_1
af+ 0x804b914 36 short_function_2
af+ 0x804b970 2 F_594
af+ 0x804b974 2 short_function_4
af+ 0x804b976 2 short_function_3
af+ 0x804bb40 180 F_597
af+ 0x804bbf4 204 F_598
af+ 0x804bcc0 90 F_599
af+ 0x804bd2c 86 DMA_Init
af+ 0x804bd82 26 DMA_Cmd
af+ 0x804bd9c 64 F_208
af+ 0x804be68 14 F_209
af+ 0x804be7c 20 F_604
af+ 0x804bea0 292 F_605
af+ 0x804c01c 32 F_606
af+ 0x804c150 6 F_607
af+ 0x804c240 106 F_608
af+ 0x804c3c8 64 F_609
af+ 0x804c40a 78 F_610
af+ 0x804c65e 74 F_611
af+ 0x804c886 78 F_218
af+ 0x804caba 78 F_219
af+ 0x804ccbe 2 F_220_Nice
af+ 0x804ccc2 84 F_221
af+ 0x804d258 2 F_222_Nice
af+ 0x804d25c 2 F_223_Nice
af+ 0x804d260 2 F_224_Nice
af+ 0x804d264 2 F_225_Nice
af+ 0x804d268 208 Read_Channel_Switch
af+ 0x804d338 136 This_function_called_Read_Channel_Switch
af+ 0x804d3c0 36 F_622
af+ 0x804d410 8 F_623
af+ 0x804d418 8 F_624
af+ 0x804d420 8 F_625
af+ 0x804d428 10 F_626
af+ 0x804d432 10 F_627
af+ 0x804d448 7540 F_628
af+ 0x804f1c0 124 F_82
af+ 0x804f28c 18 F_630
af+ 0x804f29e 150 F_883
af+ 0x804f33c 2 F_631
af+ 0x804f340 10 F_884
af+ 0x804f34a 104 F_632
af+ 0x804f3b2 12 F_885
af+ 0x804f3dc 136 F_633
af+ 0x804f464 18 F_634
af+ 0x804f49a 6 F_229
af+ 0x804f4b4 80 F_230
af+ 0x804f650 78 F_231
af+ 0x804f6ac 224 F_886
af+ 0x804f78c 52 F_887
af+ 0x804f7c8 238 F_638
af+ 0x804f8b8 50 F_232
af+ 0x804f8ea 18 F_233
af+ 0x804f8fc 44 F_234
af+ 0x804f928 28 F_642
af+ 0x804f944 28 F_235
af+ 0x804f964 120 F_1159
af+ 0x804f9dc 32 F_1145
af+ 0x804fcb0 234 I2C1_Function_1
af+ 0x804fda8 28 I2C1_Function_2
af+ 0x804fdc4 28 I2C1_Function_3
af+ 0x804fde0 28 I2C1_Function_4
af+ 0x804fdfc 22 I2C1_Function_5
af+ 0x804fe12 6 I2C1_Function_6
af+ 0x804fe18 6 I2C1_Function_7
af+ 0x804fe1e 52 I2C1_Function_8
af+ 0x804fe52 66 I2C1_Function_9
af+ 0x804fe94 212 F_653
af+ 0x804ff74 244 Function_Called_Big_I2C_Function
af+ 0x8050068 270 Function_Function_Called_Big_I2C_Function
af+ 0x8050176 2632 F_656
af+ 0x8050bcc 102 F_238
af+ 0x8050c40 2 F_239_Nice
af+ 0x8050e24 44 F_659
af+ 0x8050ed8 524 F_660
af+ 0x8051140 24 F_661
af+ 0x8051248 130 F_662
af+ 0x80512f0 46 F_663
af+ 0x8051334 126 F_664
af+ 0x8051498 76 F_665
af+ 0x805151c 64 F_888
af+ 0x805155c 20 F_666
af+ 0x8051570 14 F_667
af+ 0x8051e14 28 F_668
af+ 0x8051e30 418 F_669
af+ 0x8051fd4 582 F_670
af+ 0x805221c 144 F_671
af+ 0x80522ac 152 F_672
af+ 0x805235c 19184 F_245
af+ 0x80523a0 846 F_674
af+ 0x80526f0 878 F_675
af+ 0x8052a5e 266 F_676
af+ 0x8052b68 164 F_677
af+ 0x8052c0c 68 F_678
af+ 0x8052c50 4 F_679
af+ 0x8052e06 12 F_680
af+ 0x8052e9a 28 F_1027
af+ 0x8052eb6 16 F_1028
af+ 0x8052f34 2 F_681
af+ 0x8052f38 84 F_682
af+ 0x8052f8c 82 F_683
af+ 0x8052fde 130 F_684
af+ 0x8053060 434 F_685
af+ 0x8053212 682 F_686
af+ 0x80534d4 98 F_687
af+ 0x8053538 98 F_688
af+ 0x805359c 70 F_689
af+ 0x80535e4 142 F_690
af+ 0x8053674 84 F_691
af+ 0x80536c8 78 F_692
af+ 0x8053718 158 F_693
af+ 0x80537b8 70 F_694
af+ 0x8054cbc 240 F_695
af+ 0x8055038 142 F_696
af+ 0x80550c8 150 F_697
af+ 0x8055168 6 F_698
af+ 0x8055170 108 F_699
af+ 0x8055258 254 F_700
af+ 0x805535c 68 F_701
af+ 0x80553a0 228 F_702
af+ 0x8055484 416 F_703
af+ 0x8055628 146 F_704
af+ 0x80556bc 64 F_705
af+ 0x80556fc 60 F_706
af+ 0x805573c 200 F_707
af+ 0x8055808 526 F_708
af+ 0x8055a20 46 F_709
af+ 0x8055a78 142 F_889
af+ 0x8055b64 28 F_890
af+ 0x8055bac 42 F_891
af+ 0x8055c74 128 F_710
af+ 0x8055cf4 72 F_711
af+ 0x8055d3c 56 F_892
af+ 0x8055d74 1198 F_893
af+ 0x8056224 452 F_894
af+ 0x80563e8 50 F_712
af+ 0x805641c 188 F_713
af+ 0x8056580 62 F_714
af+ 0x80565c0 74 F_1029
af+ 0x8056640 58 F_715
af+ 0x8056680 120 F_716
af+ 0x8056700 130 F_895
af+ 0x8056800 120 F_717
af+ 0x8056880 48 F_718
af+ 0x80568c0 48 F_719
af+ 0x8056900 124 F_720
af+ 0x8056980 32 F_896
af+ 0x80569c0 130 F_721
af+ 0x8056a80 54 F_722
af+ 0x8056ac0 116 F_723
af+ 0x8056b40 44 F_724
af+ 0x8056b80 264 F_725
af+ 0x8056e4c 28 F_726
af+ 0x8056e68 104 F_727
af+ 0x8056ed4 42 F_728
af+ 0x8056efe 22 F_1088
af+ 0x8056f14 14 F_1030
af+ 0x8056f22 150 F_729
af+ 0x8056fb8 90 F_730
af+ 0x8057012 70 F_897
af+ 0x8057058 62 F_898
af+ 0x8057096 70 F_731
af+ 0x80570dc 70 F_899
af+ 0x8057122 20 F_1031
af+ 0x8057164 88 F_732
af+ 0x80571bc 80 F_733
af+ 0x805720c 360 F_734
af+ 0x8057374 296 F_900
af+ 0x805749c 88 F_901
af+ 0x80574f4 192 F_902
af+ 0x80575b4 56 F_903
af+ 0x80575ec 54 F_904
af+ 0x8057622 136 F_905
af+ 0x80576c0 52 F_906
af+ 0x80576f4 68 F_735
af+ 0x8057738 28 F_907
af+ 0x80577ae 40 F_908
af+ 0x80577d6 22 F_736
af+ 0x8057814 22 F_737
af+ 0x805782a 36 F_738
af+ 0x805784e 36 F_739
af+ 0x8057880 46 F_1032
af+ 0x80578c0 78 F_1033
af+ 0x8057940 118 F_1034
af+ 0x80579c0 356 F_1035
af+ 0x8057b40 162 F_909
af+ 0x8057c00 70 F_1036
af+ 0x8057c80 70 F_1037
af+ 0x8057d00 82 F_1038
af+ 0x8057d5c 180 F_910
af+ 0x8057e10 36 F_740
af+ 0x8057e34 42 F_911
af+ 0x8057e80 30 F_1039
af+ 0x8057ea0 32 F_741
af+ 0x8057ec0 38 F_912
af+ 0x8057f00 34 F_913
af+ 0x8057f24 20 F_914
af+ 0x8057f38 142 F_742
af+ 0x8058058 64 F_915
af+ 0x8058098 32 F_916
af+ 0x80580b8 162 F_917
af+ 0x805815c 20 F_743
af+ 0x8058170 68 F_918
af+ 0x80581dc 62 F_919
af+ 0x805821c 52 F_744
af+ 0x8058250 52 F_920
af+ 0x8058284 122 F_745
af+ 0x8058300 50 F_746
af+ 0x8058334 56 F_747
af+ 0x805836c 80 F_921
af+ 0x80583bc 522 F_748
af+ 0x80585c8 3752 F_749
af+ 0x8059470 30 F_750
af+ 0x8059490 176 F_751
af+ 0x8059540 170 F_922
af+ 0x80595ec 220 F_923
af+ 0x80596c8 80 F_924
af+ 0x8059718 2630 F_752
af+ 0x805a160 74 F_925
af+ 0x805a1ac 602 F_753
af+ 0x805a408 612 F_754
af+ 0x805a66c 1380 F_755
af+ 0x805abd0 1338 F_756
af+ 0x805b10c 120 F_926
af+ 0x805b184 64 F_927
af+ 0x805b1c4 230 F_928
af+ 0x805b2ac 106 F_1040
af+ 0x805b318 122 F_929
af+ 0x805b394 60 F_930
af+ 0x805b3d0 1812 F_757
af+ 0x805bae8 74 F_758
af+ 0x805bb34 126 F_759
af+ 0x805bbb4 502 F_760
af+ 0x805bdac 296 F_761
af+ 0x805bed4 594 F_762
af+ 0x805c128 4 F_763
af+ 0x805c12c 568 F_764
af+ 0x805c368 276 F_931
af+ 0x805c47c 404 F_765
af+ 0x805c610 20 F_766
af+ 0x805c624 182 F_767
af+ 0x805c6dc 156 F_768
af+ 0x805c778 108 F_769
af+ 0x805c7e4 18 F_770
af+ 0x805c7f8 1094 F_932
af+ 0x805cc40 86 F_771
af+ 0x805cc98 1084 F_772
af+ 0x805d0d4 22 F_773
af+ 0x805d0ec 44 F_774
af+ 0x805d11c 612 F_775
af+ 0x805d380 104 F_1041
af+ 0x805d3e8 150 F_933
af+ 0x805d480 406 F_776
af+ 0x805d618 236 F_777
af+ 0x805d708 76 F_778
af+ 0x805d754 130 F_779
af+ 0x805d85c 66 F_780
af+ 0x805d8a4 1634 F_781
af+ 0x805df08 260 F_1042
af+ 0x805e010 390 F_934
af+ 0x805e19c 440 F_935
af+ 0x805e354 560 F_936
af+ 0x805e584 1692 F_782
af+ 0x805ec20 240 F_783
af+ 0x805ed14 1114 F_784
af+ 0x805f180 32 F_1089
af+ 0x805f1a0 98 F_1043
af+ 0x805f202 64 F_1090
af+ 0x805f26c 218 F_937
af+ 0x805f346 182 F_938
af+ 0x805f3fc 24 F_939
af+ 0x805f414 26 F_940
af+ 0x805f42e 70 F_1044
af+ 0x805f474 62 F_1045
af+ 0x805f4bc 54 F_941
af+ 0x805f522 16 F_1046
af+ 0x805f532 368 F_942
af+ 0x805f6a2 88 F_1047
af+ 0x805f770 114 F_943
af+ 0x805f7e2 358 F_1048
af+ 0x805f948 294 F_1049
af+ 0x805fa6e 62 F_944
af+ 0x805faac 62 F_1050
af+ 0x805fb1a 80 F_945
af+ 0x805fbc0 282 F_946
af+ 0x805fd00 178 F_947
af+ 0x805fdb8 182 F_948
af+ 0x805fe74 98 F_949
af+ 0x805fed8 294 F_950
af+ 0x8060000 268 F_951
af+ 0x806010c 238 F_952
af+ 0x8060200 162 F_953
af+ 0x80602a8 298 F_954
af+ 0x80603d8 68 F_955
af+ 0x806041c 72 F_956
af+ 0x8060464 86 F_957
af+ 0x80604ba 122 F_958
af+ 0x8060534 82 F_959
af+ 0x8060586 42 F_960
af+ 0x80605b0 42 F_961
af+ 0x80605da 326 F_962
af+ 0x8060720 214 F_963
af+ 0x8060920 770 F_1051
af+ 0x8060c24 220 F_964
af+ 0x8060df0 72 F_965
af+ 0x8060e38 268 F_966
af+ 0x8060f4c 78 F_967
af+ 0x8060f9c 68 F_968
af+ 0x8060fe0 146 F_969
af+ 0x8061074 566 F_1052
af+ 0x80612ac 138 F_1053
af+ 0x8061340 252 F_970
af+ 0x8061440 50 F_971
af+ 0x8061480 14 F_1054
af+ 0x80614a4 14 F_1055
af+ 0x809573c 366 LED_Process


CCa 0x800df84 Function with check LCD_CS ....   why ....  when (set/noset) jump over function
CCa 0x800df8e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x801931c GPIO_SetBits(GPIOC, 0x40)
CCa 0x80193a6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801942e GPIO_SetBits(GPIOC, 0x40)
CCa 0x8019718 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801ca5c GPIO_SetBits(GPIOD, 0x40)
CCa 0x801f228 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f334 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f3b0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f40c GPIO_SetBits(GPIOC, 0x40)
CCa 0x801f878 GPIO_SetBits(GPIOC, 0x40)
CCa 0x801ffda GPIO_SetBits(GPIOC, 0x40)
CCa 0x80201c0 GPIO_SetBits(GPIOC, 0x40)
CCa 0x80261b8 ... maybe STM32LIB ErrorStatus RTC_EnterInitMode(void)
CCa 0x802620a ... maybe STM32LIB Fuction void RTC_ExitInitMode(void)
CCa 0x802621c ... maybe STM32LIB ErrorStatus RTC_WaitForSynchro(void)
CCa 0x8026280 ... maybe STM32LIB ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x0802631e call RTC_ExitInitMode(void)
CCa 0x0802632c call RTC_WaitForSynchro
CCa 0x0802634a ... maybe STM32LIB void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x0802639a ... maybe STM32LIB ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x08026460 ... maybe STM32LIB void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x080264ac ... maybe STM32LIB void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
CCa 0x080264e4 ... maybe STM32LIB void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
CCa 0x08026504 ... maybe STM32LIB ErrorStatus RTC_RefClockCmd(FunctionalState NewState)
CCa 0x08026596 something to store to RTC backup registers (RTC_BKPxR)
CCa 0x080265ba ... maybe STM32LIB void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState)
CCa 0x08026616 ... maybe STM32LIB void RTC_ClearFlag(uint32_t RTC_FLAG)
CCa 0x08026662 ... maybe STM32LIB
CCa 0x08026a9a ... maybe STM32LIB  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08026ab0 GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
CCa 0x08026ab6 ... maybe STM32LIB
CCa 0x08026aba ... maybe STM32LIB
CCa 0x08026abe ... maybe STM32LIB .. (r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08027c22 GPIO_SetBits(GPIOC, 0x40)
CCa 0x08027c2e GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028120 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802823a GPIO_SetBits(GPIOC, 0x40)
CCa 0x08028246 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282b6 GPIO_SetBits(GPIOC, 0x40)
CCa 0x080282c2 GPIO_SetBits(GPIOC, 0x40)
CCa 0x0802fb0e GPIO_SetBits(GPIOD, 0x80)
CCa 0x08030f40 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08030f64 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08032a3c Called from Task Start()
CCa 0x0803a2c6 mybee vox
CCa 0x0803d026 Switch LAMP on ....
CCa 0x0803dc3a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803dc48 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803dd70 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0b2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0be call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e0e2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e100 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e110 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e1f4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e296 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e2a2 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e50e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e51a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e542 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e54e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e5d8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e5e4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e69a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0803e6a4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080426c8 Function ... Init ADC1 (Bat) with dma  value via dma DMA2 DMA_S0CRin 0x2001cfcc
CCa 0x080443f8 Crate Process -LED Process- Thread Start  addr 0x809573d
CCa 0x08044620 set A7 POW_C
CCa 0x080458de call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080458e8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045952 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804595c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804598a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08045994 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08046ca0 ... maybe STM32LIB
CCa 0x08046ca2 PWR_CSR
CCa 0x08046cb6 maybe Function
CCa 0x08046cb6 PWR_CR
CCa 0x08046cbe PWR_CR
CCa 0x080475c2 Function Test gpio with address from address
CCa 0x080475e4 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08047620 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x08049e9c ... maybe STM32LIB ..Function start adc(r0=base address) Bit 30 SWSTART: Start conversion of regular channels
CCa 0x0804b584 Reset BSHIFT
CCa 0x0804b5a4 Set BSHIFT
CCa 0x0804b5b2 Set PLL_DAT/DMR_SDI
CCa 0x0804b5bc Reset PLL_DAT/DMR_SDI
CCa 0x0804b5d4 Reset BSHIFT
CCa 0x0804b5e2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804bd2c ... maybe STM32LIB
CCa 0x0804bd82 ... maybe STM32LIB
CCa 0x0804c24a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c258 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c266 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2c0 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2ce call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804c2dc call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ccca call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ccd6 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cce2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd26 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd32 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804cd3e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804ce18 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d272 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d27e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d28a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d29a add 1 for ECN0
CCa 0x0804d2a2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ae call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ba call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ca add 2 for ECN1
CCa 0x0804d2d2 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2de call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2ea call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d2fa add 4 for ECN2
CCa 0x0804d302 call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d30e call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d31a call  ret r0 read_an_test_gpio (r0=gpio base address ,r1= test_mask)
CCa 0x0804d32a add 8 for ECN3
CCa 0x0804d336 end of Function  read and .. channal switch
CCa 0x0804d57e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804d608 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804e9b8 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804eb1e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ec78 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0804ed92 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08093918 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095736 Somsing with leds
CCa 0x0809573d LED Process()
CCa 0x08095740 from Codeplug Led Indikatore Enable 0x2001c658  ... maybe
CCa 0x08095752 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095752 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x08095764 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095770 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095770 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809577a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809577a GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957c6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957c6 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x080957d0 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957d0 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x080957ec call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957ec GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080957f6 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080957f6 GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x08095842 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095842 GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x0809584c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809584c GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x0809586c call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809586c GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x08095876 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095876 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x08095884 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x08095884 GPIO_WriteBit(GPIOE, 1, 1) - RX_LED ON
CCa 0x0809588e call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809588e GPIO_WriteBit(GPIOE, 2, 1) - TX_LED ON
CCa 0x0809589a call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x0809589a GPIO_WriteBit(GPIOE, 1, 0) - RX_LED OFF
CCa 0x080958a4 call GPIO_Set_Reset_Bits(r2 == 0 reset/ r2 == 1 set , r0 base address, r1 val)
CCa 0x080958a4 GPIO_WriteBit(GPIOE, 2, 0) - TX_LED OFF
CCa 0x800d8d0 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x800d940 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x800ded4 config_byte_LED_enable_and_more
CCa 0x8012b9e config_byte_LED_enable_and_more
CCa 0x8012bae config_byte_LED_enable_and_more
CCa 0x8012bbe config_byte_LED_enable_and_more
CCa 0x8012bce config_byte_LED_enable_and_more
CCa 0x80154c2 config_byte_LED_enable_and_more
CCa 0x8016c02 config_byte_LED_enable_and_more
CCa 0x8016d26 config_byte_LED_enable_and_more
CCa 0x8016db0 config_byte_LED_enable_and_more
CCa 0x8016e5a config_byte_LED_enable_and_more
CCa 0x8017002 config_byte_LED_enable_and_more
CCa 0x8017218 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x801727c Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x801760c Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x8017644 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x8017bba config_byte_LED_enable_and_more
CCa 0x8017bc4 config_byte_LED_enable_and_more
CCa 0x8017bde config_byte_LED_enable_and_more
CCa 0x8017be8 config_byte_LED_enable_and_more
CCa 0x8017c02 config_byte_LED_enable_and_more
CCa 0x8017c0c config_byte_LED_enable_and_more
CCa 0x8017c18 config_byte_LED_enable_and_more
CCa 0x8017c22 config_byte_LED_enable_and_more
CCa 0x8017c2e config_byte_LED_enable_and_more
CCa 0x8017c38 config_byte_LED_enable_and_more
CCa 0x8017c44 config_byte_LED_enable_and_more
CCa 0x8017c4e config_byte_LED_enable_and_more
CCa 0x8017c5a config_byte_LED_enable_and_more
CCa 0x8017c64 config_byte_LED_enable_and_more
CCa 0x8017c70 config_byte_LED_enable_and_more
CCa 0x8017c7a config_byte_LED_enable_and_more
CCa 0x8017e80 config_byte_LED_enable_and_more
CCa 0x8017e8a config_byte_LED_enable_and_more
CCa 0x8017ea4 config_byte_LED_enable_and_more
CCa 0x8017eae config_byte_LED_enable_and_more
CCa 0x8017ec8 config_byte_LED_enable_and_more
CCa 0x8017ed2 config_byte_LED_enable_and_more
CCa 0x8017ede config_byte_LED_enable_and_more
CCa 0x8017ee8 config_byte_LED_enable_and_more
CCa 0x8017ef4 config_byte_LED_enable_and_more
CCa 0x8017efe config_byte_LED_enable_and_more
CCa 0x8017f0a config_byte_LED_enable_and_more
CCa 0x8017f14 config_byte_LED_enable_and_more
CCa 0x8017f22 config_byte_LED_enable_and_more
CCa 0x8017f2c config_byte_LED_enable_and_more
CCa 0x8017f38 config_byte_LED_enable_and_more
CCa 0x8017f42 config_byte_LED_enable_and_more
CCa 0x8018142 config_byte_LED_enable_and_more
CCa 0x8018256 config_byte_LED_enable_and_more
CCa 0x80182de config_byte_LED_enable_and_more
CCa 0x801851a config_byte_LED_enable_and_more
CCa 0x8018610 config_byte_LED_enable_and_more
CCa 0x8018c9e config_byte_LED_enable_and_more
CCa 0x8019188 config_byte_LED_enable_and_more
CCa 0x8019326 config_byte_LED_enable_and_more
CCa 0x80193b0 config_byte_LED_enable_and_more
CCa 0x8019438 config_byte_LED_enable_and_more
CCa 0x8019722 config_byte_LED_enable_and_more
CCa 0x801978e config_byte_LED_enable_and_more
CCa 0x80198a6 config_byte_LED_enable_and_more
CCa 0x80198ae config_byte_LED_enable_and_more
CCa 0x8019930 config_byte_LED_enable_and_more
CCa 0x8019938 config_byte_LED_enable_and_more
CCa 0x80199a6 config_byte_LED_enable_and_more
CCa 0x8019b34 config_byte_LED_enable_and_more
CCa 0x8019bc8 config_byte_LED_enable_and_more
CCa 0x8019c50 config_byte_LED_enable_and_more
CCa 0x8019cd2 config_byte_LED_enable_and_more
CCa 0x8019d60 config_byte_LED_enable_and_more
CCa 0x801a02a config_byte_LED_enable_and_more
CCa 0x801a544 config_byte_LED_enable_and_more
CCa 0x801a54c config_byte_LED_enable_and_more
CCa 0x801a554 config_byte_LED_enable_and_more
CCa 0x801a55c config_byte_LED_enable_and_more
CCa 0x801a564 config_byte_LED_enable_and_more
CCa 0x801a574 config_byte_LED_enable_and_more
CCa 0x801a584 config_byte_LED_enable_and_more
CCa 0x801a594 config_byte_LED_enable_and_more
CCa 0x801a5f0 config_byte_LED_enable_and_more
CCa 0x801a782 config_byte_LED_enable_and_more
CCa 0x801a78c config_byte_LED_enable_and_more
CCa 0x801a828 config_byte_LED_enable_and_more
CCa 0x801a832 config_byte_LED_enable_and_more
CCa 0x801a9ba config_byte_LED_enable_and_more
CCa 0x801ad0a config_byte_LED_enable_and_more
CCa 0x801afbe config_byte_LED_enable_and_more
CCa 0x801afcc config_byte_LED_enable_and_more
CCa 0x801afda config_byte_LED_enable_and_more
CCa 0x801afe8 config_byte_LED_enable_and_more
CCa 0x801f232 config_byte_LED_enable_and_more
CCa 0x801f33e config_byte_LED_enable_and_more
CCa 0x801f3ba config_byte_LED_enable_and_more
CCa 0x801f416 config_byte_LED_enable_and_more
CCa 0x801f558 config_byte_LED_enable_and_more
CCa 0x801f578 config_byte_LED_enable_and_more
CCa 0x801f7da config_byte_LED_enable_and_more
CCa 0x801f814 config_byte_LED_enable_and_more
CCa 0x801f882 config_byte_LED_enable_and_more
CCa 0x801f88c config_byte_LED_enable_and_more
CCa 0x801f996 config_byte_LED_enable_and_more
CCa 0x801fc04 config_byte_LED_enable_and_more
CCa 0x801fc9a config_byte_LED_enable_and_more
CCa 0x801fd12 config_byte_LED_enable_and_more
CCa 0x801ffe4 config_byte_LED_enable_and_more
CCa 0x80201ca config_byte_LED_enable_and_more
CCa 0x80207ce config_byte_LED_enable_and_more
CCa 0x80208e8 config_byte_LED_enable_and_more
CCa 0x80218ae config_byte_LED_enable_and_more
CCa 0x80218c0 config_byte_LED_enable_and_more
CCa 0x802352a config_byte_LED_enable_and_more
CCa 0x8023536 config_byte_LED_enable_and_more
CCa 0x8026136 ... maybe STM32LIB ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) \n Beginn Function ... Set RTC_CR (Hour format RTC_CR and RTC_PRER ) r0  arg[0] arg[1] arg[2]\narg[0] RTC_CR\arg[1] RTC_PRER\n arg[2] RTC_PRER
CCa 0x8026194 Call RTC_ExitInitMode()
CCa 0x802631e Call RTC_ExitInitMode()
CCa 0x8026434 Call RTC_ExitInitMode()
CCa 0x8027c18 config_byte_LED_enable_and_more
CCa 0x8027c38 config_byte_LED_enable_and_more
CCa 0x802812a config_byte_LED_enable_and_more
CCa 0x8028230 config_byte_LED_enable_and_more
CCa 0x8028250 config_byte_LED_enable_and_more
CCa 0x80282ac config_byte_LED_enable_and_more
CCa 0x80282cc config_byte_LED_enable_and_more
CCa 0x8028fda config_byte_LED_enable_and_more
CCa 0x80293a2 config_byte_LED_enable_and_more
CCa 0x80294e8 config_byte_LED_enable_and_more
CCa 0x80295a2 config_byte_LED_enable_and_more
CCa 0x80295ce config_byte_LED_enable_and_more
CCa 0x8029774 config_byte_LED_enable_and_more
CCa 0x80299d4 config_byte_LED_enable_and_more
CCa 0x80299fc config_byte_LED_enable_and_more
CCa 0x8029a04 config_byte_LED_enable_and_more
CCa 0x8029a0a config_byte_LED_enable_and_more
CCa 0x8029a12 config_byte_LED_enable_and_more
CCa 0x802a8a8 config_byte_LED_enable_and_more
CCa 0x802a91a config_byte_LED_enable_and_more
CCa 0x802af8a config_byte_LED_enable_and_more
CCa 0x802af98 config_byte_LED_enable_and_more
CCa 0x8030262 Call OS_ENTER_CRITICAL()
CCa 0x803027c Call OS_EXIT_CRITICAL()
CCa 0x80302d6 Call OS_ENTER_CRITICAL()
CCa 0x80302ec Call OS_EXIT_CRITICAL()
CCa 0x8030324 Call OS_EXIT_CRITICAL()
CCa 0x803032c Call OS_ENTER_CRITICAL()
CCa 0x8030394 Call OS_EXIT_CRITICAL()
CCa 0x80303c6 Call OS_ENTER_CRITICAL()
CCa 0x80303e2 Call OS_EXIT_CRITICAL()
CCa 0x80303f6 Call OS_EXIT_CRITICAL()
CCa 0x8030402 Call OS_EXIT_CRITICAL()
CCa 0x80308da config_byte_LED_enable_and_more
CCa 0x8030d56 config_byte_LED_enable_and_more
CCa 0x8030d66 config_byte_LED_enable_and_more
CCa 0x8030fa8 config_byte_LED_enable_and_more
CCa 0x8031018 config_byte_LED_enable_and_more
CCa 0x8031020 config_byte_LED_enable_and_more
CCa 0x80329da Call OS_ENTER_CRITICAL()
CCa 0x8032a1e Call OS_EXIT_CRITICAL()
CCa 0x80372ba config_byte_LED_enable_and_more
CCa 0x80372f4 config_byte_LED_enable_and_more
CCa 0x80375d2 config_byte_LED_enable_and_more
CCa 0x803760c config_byte_LED_enable_and_more
CCa 0x8037774 config_byte_LED_enable_and_more
CCa 0x80377a4 config_byte_LED_enable_and_more
CCa 0x8037824 config_byte_LED_enable_and_more
CCa 0x8037854 config_byte_LED_enable_and_more
CCa 0x803bb72 config_byte_LED_enable_and_more
CCa 0x803bb7c config_byte_LED_enable_and_more
CCa 0x803c04e config_byte_LED_enable_and_more
CCa 0x803c672 config_byte_LED_enable_and_more
CCa 0x803c6c0 config_byte_LED_enable_and_more
CCa 0x803cb6a config_byte_LED_enable_and_more
CCa 0x803cb78 config_byte_LED_enable_and_more
CCa 0x803cc2c config_byte_LED_enable_and_more
CCa 0x803cc3a config_byte_LED_enable_and_more
CCa 0x803d02e config_byte_LED_enable_and_more
CCa 0x803d0b8 config_byte_LED_enable_and_more
CCa 0x803d106 config_byte_LED_enable_and_more
CCa 0x803da10 Call OS_ENTER_CRITICAL()
CCa 0x803da22 Call OS_EXIT_CRITICAL()
CCa 0x803da3e Call OS_ENTER_CRITICAL()
CCa 0x803da58 Call OS_EXIT_CRITICAL()
CCa 0x803daaa Call OS_ENTER_CRITICAL()
CCa 0x803dabe Call OS_EXIT_CRITICAL()
CCa 0x803daf4 Call OS_EXIT_CRITICAL()
CCa 0x803dafc Call OS_ENTER_CRITICAL()
CCa 0x803db4c Call OS_EXIT_CRITICAL()
CCa 0x803db78 Call OS_ENTER_CRITICAL()
CCa 0x803db94 Call OS_EXIT_CRITICAL()
CCa 0x803dbb2 Call OS_EXIT_CRITICAL()
CCa 0x803dbbc Call OS_EXIT_CRITICAL()
CCa 0x803ddce config_byte_LED_enable_and_more
CCa 0x803dddc config_byte_LED_enable_and_more
CCa 0x803ddec config_byte_LED_enable_and_more
CCa 0x803eefe config_byte_LED_enable_and_more
CCa 0x803f0c6 config_byte_LED_enable_and_more
CCa 0x803f1d8 config_byte_LED_enable_and_more
CCa 0x803f2ca config_byte_LED_enable_and_more
CCa 0x803f3d6 config_byte_LED_enable_and_more
CCa 0x803f42c config_byte_LED_enable_and_more
CCa 0x803f462 config_byte_LED_enable_and_more
CCa 0x803f506 config_byte_LED_enable_and_more
CCa 0x803f5a0 config_byte_LED_enable_and_more
CCa 0x803f716 config_byte_LED_enable_and_more
CCa 0x803f75a config_byte_LED_enable_and_more
CCa 0x803f78c config_byte_LED_enable_and_more
CCa 0x803f7e8 config_byte_LED_enable_and_more
CCa 0x803f87e config_byte_LED_enable_and_more
CCa 0x803f902 config_byte_LED_enable_and_more
CCa 0x803f934 config_byte_LED_enable_and_more
CCa 0x803f95c config_byte_LED_enable_and_more
CCa 0x803f98c config_byte_LED_enable_and_more
CCa 0x803f9ce config_byte_LED_enable_and_more
CCa 0x803fa1c config_byte_LED_enable_and_more
CCa 0x803fa44 config_byte_LED_enable_and_more
CCa 0x803fa88 config_byte_LED_enable_and_more
CCa 0x803fb1c config_byte_LED_enable_and_more
CCa 0x8040152 config_byte_LED_enable_and_more
CCa 0x80401ce config_byte_LED_enable_and_more
CCa 0x80401e6 config_byte_LED_enable_and_more
CCa 0x80401fe config_byte_LED_enable_and_more
CCa 0x8040216 config_byte_LED_enable_and_more
CCa 0x804022c config_byte_LED_enable_and_more
CCa 0x8040244 config_byte_LED_enable_and_more
CCa 0x8040260 config_byte_LED_enable_and_more
CCa 0x80409e4 config_byte_LED_enable_and_more
CCa 0x8040a44 config_byte_LED_enable_and_more
CCa 0x8040a8c config_byte_LED_enable_and_more
CCa 0x8040bea config_byte_LED_enable_and_more
CCa 0x8040d7e config_byte_LED_enable_and_more
CCa 0x8040dec config_byte_LED_enable_and_more
CCa 0x8041120 config_byte_LED_enable_and_more
CCa 0x8041148 config_byte_LED_enable_and_more
CCa 0x8041ece Call OS_ENTER_CRITICAL()
CCa 0x8041ed4 Call OS_EXIT_CRITICAL()
CCa 0x8041f1e Call OS_ENTER_CRITICAL()
CCa 0x8041f9e Call OS_EXIT_CRITICAL()
CCa 0x8041fb2 Call OS_ENTER_CRITICAL()
CCa 0x8041fd8 Call OS_EXIT_CRITICAL()
CCa 0x8041fec Call OS_ENTER_CRITICAL()
CCa 0x804201c Call OS_EXIT_CRITICAL()
CCa 0x8042026 Call OS_EXIT_CRITICAL()
CCa 0x804202c Call OS_EXIT_CRITICAL()
CCa 0x8042032 Call OS_EXIT_CRITICAL()
CCa 0x8042082 Call OS_ENTER_CRITICAL()
CCa 0x8042098 Call OS_EXIT_CRITICAL()
CCa 0x80420ec Call OS_EXIT_CRITICAL()
CCa 0x80420f8 Call OS_ENTER_CRITICAL()
CCa 0x80423f0 Call OS_ENTER_CRITICAL()
CCa 0x8042442 Call OS_EXIT_CRITICAL()
CCa 0x8042468 runs with 1MHz
CCa 0x804246a Call OS_ENTER_CRITICAL()
CCa 0x804247c Call OS_EXIT_CRITICAL()
CCa 0x80424a4 Call OS_ENTER_CRITICAL()
CCa 0x80424bc Call OS_EXIT_CRITICAL()
CCa 0x804255c Call OS_ENTER_CRITICAL()
CCa 0x80425b6 Call OS_EXIT_CRITICAL()
CCa 0x80425c0 Call OS_EXIT_CRITICAL()
CCa 0x8043570 config_byte_LED_enable_and_more
CCa 0x8043cee config_byte_LED_enable_and_more
CCa 0x8043cfa config_byte_LED_enable_and_more
CCa 0x8043d04 config_byte_LED_enable_and_more
CCa 0x8044086 config_byte_LED_enable_and_more
CCa 0x804454a config_byte_LED_enable_and_more
CCa 0x80447a2 config_byte_LED_enable_and_more
CCa 0x80447ae config_byte_LED_enable_and_more
CCa 0x8045a9c Call OS_ENTER_CRITICAL()
CCa 0x8045ad8 Call OS_EXIT_CRITICAL()
CCa 0x8045afe Call OS_EXIT_CRITICAL()
CCa 0x8045b22 Call OS_EXIT_CRITICAL()
CCa 0x8045b48 Call OS_EXIT_CRITICAL()
CCa 0x8045b4e Call OS_EXIT_CRITICAL()
CCa 0x8045b7a Call OS_ENTER_CRITICAL()
CCa 0x8045ba4 Call OS_EXIT_CRITICAL()
CCa 0x8045bae Call OS_EXIT_CRITICAL()
CCa 0x8045c1c Call OS_ENTER_CRITICAL()
CCa 0x8045c62 Call OS_EXIT_CRITICAL()
CCa 0x8045c82 Call OS_EXIT_CRITICAL()
CCa 0x8045c8a Call OS_ENTER_CRITICAL()
CCa 0x8045cca Call OS_EXIT_CRITICAL()
CCa 0x8045d00 Call OS_EXIT_CRITICAL()
CCa 0x8045d20 Call OS_EXIT_CRITICAL()
CCa 0x8045d4a Call OS_EXIT_CRITICAL()
CCa 0x8045d6a Call OS_EXIT_CRITICAL()
CCa 0x8045d92 Call OS_EXIT_CRITICAL()
CCa 0x8045db2 Call OS_EXIT_CRITICAL()
CCa 0x8045dba Call OS_EXIT_CRITICAL()
CCa 0x8045e00 Call OS_EXIT_CRITICAL()
CCa 0x8045e1a Call OS_EXIT_CRITICAL()
CCa 0x8045e42 Call OS_ENTER_CRITICAL()
CCa 0x8045e72 Call OS_EXIT_CRITICAL()
CCa 0x8045f12 Call OS_EXIT_CRITICAL()
CCa 0x8045f20 Call OS_EXIT_CRITICAL()
CCa 0x8045f2e Call OS_ENTER_CRITICAL()
CCa 0x8045f38 Call OS_EXIT_CRITICAL()
CCa 0x8046a12 access auf 0x4000540 I2C1
CCa 0x8046a1c access auf 0x4000540 I2C1
CCa 0x8046a36 access auf 0x4000540 I2C1
CCa 0x8046a44 access auf 0x4000540 I2C1
CCa 0x8046a62 access auf 0x4000540 I2C1
CCa 0x8046a70 access auf 0x4000540 I2C1
CCa 0x8046a8c access auf 0x4000540 I2C1
CCa 0x8046a9a access auf 0x4000540 I2C1
CCa 0x8046aa8 access auf 0x4000540 I2C1
CCa 0x8046ac2 access auf 0x4000540 I2C1
CCa 0x8046ad0 access auf 0x4000540 I2C1
CCa 0x8046aee access auf 0x4000540 I2C1
CCa 0x8046afc access auf 0x4000540 I2C1
CCa 0x8046b18 access auf 0x4000540 I2C1
CCa 0x8046b32 access auf 0x4000540 I2C1
CCa 0x8046b4c access auf 0x4000540 I2C1
CCa 0x8046b68 access auf 0x4000540 I2C1
CCa 0x8046b76 access auf 0x4000540 I2C1
CCa 0x8046b94 access auf 0x4000540 I2C1
CCa 0x8046ba2 access auf 0x4000540 I2C1
CCa 0x8046bbe access auf 0x4000540 I2C1
CCa 0x8046bcc access auf 0x4000540 I2C1
CCa 0x8046bda access auf 0x4000540 I2C1
CCa 0x8046c02 access auf 0x4000540 I2C1
CCa 0x8046c10 access auf 0x4000540 I2C1
CCa 0x8046c1e access auf 0x4000540 I2C1
CCa 0x8046c38 access auf 0x4000540 I2C1
CCa 0x804a53e config_byte_LED_enable_and_more
CCa 0x804a548 config_byte_LED_enable_and_more
CCa 0x804a552 config_byte_LED_enable_and_more
CCa 0x804a61a config_byte_LED_enable_and_more
CCa 0x804a624 config_byte_LED_enable_and_more
CCa 0x804a62e config_byte_LED_enable_and_more
CCa 0x804a9cc config_byte_LED_enable_and_more
CCa 0x804a9d4 config_byte_LED_enable_and_more
CCa 0x804aa90 config_byte_LED_enable_and_more
CCa 0x804aa98 config_byte_LED_enable_and_more
CCa 0x804abe6 config_byte_LED_enable_and_more
CCa 0x804abee config_byte_LED_enable_and_more
CCa 0x804bb50 Call OS_ENTER_CRITICAL()
CCa 0x804bb62 Call OS_EXIT_CRITICAL()
CCa 0x804bb86 Call OS_EXIT_CRITICAL()
CCa 0x804bbc8 Call OS_ENTER_CRITICAL()
CCa 0x804bbdc Call OS_EXIT_CRITICAL()
CCa 0x804bbe8 Call OS_EXIT_CRITICAL()
CCa 0x804bc10 Call OS_ENTER_CRITICAL()
CCa 0x804bc22 Call OS_EXIT_CRITICAL()
CCa 0x804bc4a Call OS_EXIT_CRITICAL()
CCa 0x804bc92 Call OS_ENTER_CRITICAL()
CCa 0x804bca8 Call OS_EXIT_CRITICAL()
CCa 0x804bcb4 Call OS_EXIT_CRITICAL()
CCa 0x804bcd8 Call OS_ENTER_CRITICAL()
CCa 0x804bcf6 Call OS_EXIT_CRITICAL()
CCa 0x804bd04 Call OS_EXIT_CRITICAL()
CCa 0x804bd10 Call OS_EXIT_CRITICAL()
CCa 0x804c478 config_byte_LED_enable_and_more
CCa 0x804f2e0 Call RTC_RefClockCmd(FunctionalState NewState)
CCa 0x804f318 Call RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
CCa 0x804f31e Call RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
CCa 0x804f324 Call RTC_RefClockCmd(FunctionalState NewState)
CCa 0x808d0a2 Call RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d0c8 Call RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x808d5f6 Call RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
CCa 0x808d624 Call RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
CCa 0x808d6a2 config_byte_LED_enable_and_more
CCa 0x808d96e config_byte_LED_enable_and_more
CCa 0x808d976 config_byte_LED_enable_and_more
CCa 0x808d984 config_byte_LED_enable_and_more
CCa 0x808d9c8 config_byte_LED_enable_and_more
CCa 0x809381e Call OS_ENTER_CRITICAL()
CCa 0x8093830 Call OS_EXIT_CRITICAL()
CCa 0x8093b36 Call OS_ENTER_CRITICAL()
CCa 0x8093b44 Call OS_EXIT_CRITICAL()
CCa 0x8095740 config_byte_LED_enable_and_more
CCa 0x80957ac config_byte_LED_enable_and_more

