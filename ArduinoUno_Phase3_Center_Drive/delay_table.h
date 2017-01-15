static const int delays90_1[] = {
     2250,   //  800 deg/sec
     1836,   //  980 deg/sec
     1597,   // 1126 deg/sec
     1434,   // 1254 deg/sec
     1314,   // 1369 deg/sec
     1220,   // 1474 deg/sec
     1144,   // 1572 deg/sec
     1081,   // 1663 deg/sec
     1028,   // 1750 deg/sec
      982,   // 1832 deg/sec
      941,   // 1911 deg/sec
      906,   // 1986 deg/sec
      874,   // 2059 deg/sec
      845,   // 2129 deg/sec
      819,   // 2196 deg/sec
      795,   // 2262 deg/sec
      773,   // 2325 deg/sec
      753,   // 2387 deg/sec
      735,   // 2448 deg/sec
      718,   // 2506 deg/sec
      701,   // 2564 deg/sec
      686,   // 2620 deg/sec
      672,   // 2675 deg/sec
      659,   // 2729 deg/sec
      647,   // 2782 deg/sec
      635,   // 2833 deg/sec
      646,   // 2782 deg/sec
      659,   // 2731 deg/sec
      672,   // 2678 deg/sec
      685,   // 2624 deg/sec
      700,   // 2569 deg/sec
      716,   // 2513 deg/sec
      732,   // 2456 deg/sec
      750,   // 2397 deg/sec
      769,   // 2337 deg/sec
      790,   // 2276 deg/sec
      813,   // 2213 deg/sec
      837,   // 2147 deg/sec
      864,   // 2080 deg/sec
      894,   // 2011 deg/sec
      927,   // 1940 deg/sec
      964,   // 1865 deg/sec
     1006,   // 1788 deg/sec
     1053,   // 1708 deg/sec
     1108,   // 1623 deg/sec
     1172,   // 1535 deg/sec
     1248,   // 1441 deg/sec
     1341,   // 1341 deg/sec
     1458,   // 1234 deg/sec
     1610,   // 1117 deg/sec
};

static const int delays180_1[] = {
     2250,   //  800 deg/sec
     1836,   //  980 deg/sec
     1597,   // 1126 deg/sec
     1434,   // 1254 deg/sec
     1314,   // 1369 deg/sec
     1220,   // 1474 deg/sec
     1144,   // 1572 deg/sec
     1081,   // 1663 deg/sec
     1028,   // 1750 deg/sec
      982,   // 1832 deg/sec
      941,   // 1911 deg/sec
      906,   // 1986 deg/sec
      874,   // 2059 deg/sec
      845,   // 2129 deg/sec
      819,   // 2196 deg/sec
      795,   // 2262 deg/sec
      773,   // 2325 deg/sec
      753,   // 2387 deg/sec
      735,   // 2448 deg/sec
      718,   // 2506 deg/sec
      701,   // 2564 deg/sec
      686,   // 2620 deg/sec
      672,   // 2675 deg/sec
      659,   // 2729 deg/sec
      647,   // 2782 deg/sec
      635,   // 2833 deg/sec
      624,   // 2884 deg/sec
      613,   // 2934 deg/sec
      603,   // 2983 deg/sec
      593,   // 3031 deg/sec
      584,   // 3079 deg/sec
      575,   // 3126 deg/sec
      567,   // 3172 deg/sec
      559,   // 3217 deg/sec
      551,   // 3262 deg/sec
      544,   // 3306 deg/sec
      537,   // 3350 deg/sec
      530,   // 3393 deg/sec
      523,   // 3435 deg/sec
      517,   // 3477 deg/sec
      511,   // 3518 deg/sec
      505,   // 3559 deg/sec
      500,   // 3600 deg/sec
      500,   // 3640 deg/sec
      500,   // 3680 deg/sec
      500,   // 3720 deg/sec
      500,   // 3760 deg/sec
      500,   // 3800 deg/sec
      500,   // 3840 deg/sec
      500,   // 3880 deg/sec
      500,   // 3920 deg/sec
      500,   // 3880 deg/sec
      500,   // 3840 deg/sec
      500,   // 3800 deg/sec
      500,   // 3760 deg/sec
      500,   // 3720 deg/sec
      500,   // 3680 deg/sec
      500,   // 3640 deg/sec
      500,   // 3600 deg/sec
      505,   // 3560 deg/sec
      511,   // 3519 deg/sec
      517,   // 3478 deg/sec
      523,   // 3437 deg/sec
      530,   // 3395 deg/sec
      536,   // 3353 deg/sec
      543,   // 3310 deg/sec
      551,   // 3266 deg/sec
      558,   // 3222 deg/sec
      566,   // 3177 deg/sec
      574,   // 3132 deg/sec
      583,   // 3086 deg/sec
      592,   // 3039 deg/sec
      601,   // 2992 deg/sec
      611,   // 2944 deg/sec
      621,   // 2895 deg/sec
      632,   // 2845 deg/sec
      643,   // 2795 deg/sec
      656,   // 2743 deg/sec
      668,   // 2691 deg/sec
      682,   // 2637 deg/sec
      696,   // 2583 deg/sec
      712,   // 2527 deg/sec
      728,   // 2470 deg/sec
      746,   // 2412 deg/sec
      765,   // 2352 deg/sec
      785,   // 2291 deg/sec
      807,   // 2228 deg/sec
      831,   // 2163 deg/sec
      858,   // 2097 deg/sec
      887,   // 2028 deg/sec
      919,   // 1957 deg/sec
      955,   // 1883 deg/sec
      995,   // 1807 deg/sec
     1041,   // 1727 deg/sec
     1094,   // 1644 deg/sec
     1156,   // 1556 deg/sec
     1229,   // 1464 deg/sec
     1317,   // 1366 deg/sec
     1427,   // 1260 deg/sec
     1569,   // 1146 deg/sec
};

static int interdelay_1 = 0;

static inline void reportSpeedInfo_1() {
    Serial.println( "" );
    Serial.println( "Speed 1 - safe" );
    Serial.println( "accelMag = 80.0" );
    Serial.println( "startSpd = 800.0" );
    Serial.println( "minDelta = 500" );
    Serial.println( "interdelay = 0" );
    Serial.println( " 90 turns - 48ms each, 1.21 for 25 moves" );
    Serial.println( "180 turns - 75ms each, 1.88 for 25 moves" );
}


static const int delays90_3[] = {
     1500,   // 1200 deg/sec
     1304,   // 1380 deg/sec
     1171,   // 1536 deg/sec
     1073,   // 1677 deg/sec
      996,   // 1805 deg/sec
      934,   // 1925 deg/sec
      883,   // 2037 deg/sec
      839,   // 2143 deg/sec
      801,   // 2244 deg/sec
      769,   // 2340 deg/sec
      739,   // 2432 deg/sec
      713,   // 2521 deg/sec
      690,   // 2607 deg/sec
      669,   // 2690 deg/sec
      649,   // 2770 deg/sec
      631,   // 2848 deg/sec
      615,   // 2924 deg/sec
      600,   // 2998 deg/sec
      586,   // 3070 deg/sec
      573,   // 3140 deg/sec
      560,   // 3209 deg/sec
      549,   // 3276 deg/sec
      538,   // 3342 deg/sec
      528,   // 3407 deg/sec
      518,   // 3470 deg/sec
      509,   // 3532 deg/sec
      518,   // 3471 deg/sec
      527,   // 3409 deg/sec
      537,   // 3346 deg/sec
      548,   // 3281 deg/sec
      559,   // 3215 deg/sec
      571,   // 3148 deg/sec
      584,   // 3079 deg/sec
      598,   // 3009 deg/sec
      612,   // 2938 deg/sec
      628,   // 2864 deg/sec
      645,   // 2789 deg/sec
      663,   // 2711 deg/sec
      683,   // 2632 deg/sec
      705,   // 2550 deg/sec
      730,   // 2465 deg/sec
      757,   // 2377 deg/sec
      787,   // 2286 deg/sec
      821,   // 2192 deg/sec
      859,   // 2093 deg/sec
      904,   // 1990 deg/sec
      956,   // 1882 deg/sec
     1018,   // 1767 deg/sec
     1094,   // 1645 deg/sec
     1188,   // 1513 deg/sec
};

static const int delays180_3[] = {
     1500,   // 1200 deg/sec
     1304,   // 1380 deg/sec
     1171,   // 1536 deg/sec
     1073,   // 1677 deg/sec
      996,   // 1805 deg/sec
      934,   // 1925 deg/sec
      883,   // 2037 deg/sec
      839,   // 2143 deg/sec
      801,   // 2244 deg/sec
      769,   // 2340 deg/sec
      739,   // 2432 deg/sec
      713,   // 2521 deg/sec
      690,   // 2607 deg/sec
      669,   // 2690 deg/sec
      649,   // 2770 deg/sec
      631,   // 2848 deg/sec
      615,   // 2924 deg/sec
      600,   // 2998 deg/sec
      586,   // 3070 deg/sec
      573,   // 3140 deg/sec
      560,   // 3209 deg/sec
      549,   // 3276 deg/sec
      538,   // 3342 deg/sec
      528,   // 3407 deg/sec
      518,   // 3470 deg/sec
      509,   // 3532 deg/sec
      500,   // 3593 deg/sec
      500,   // 3654 deg/sec
      500,   // 3714 deg/sec
      500,   // 3774 deg/sec
      500,   // 3834 deg/sec
      500,   // 3894 deg/sec
      500,   // 3954 deg/sec
      500,   // 4014 deg/sec
      500,   // 4074 deg/sec
      500,   // 4134 deg/sec
      500,   // 4194 deg/sec
      500,   // 4254 deg/sec
      500,   // 4314 deg/sec
      500,   // 4374 deg/sec
      500,   // 4434 deg/sec
      500,   // 4494 deg/sec
      500,   // 4554 deg/sec
      500,   // 4614 deg/sec
      500,   // 4674 deg/sec
      500,   // 4734 deg/sec
      500,   // 4794 deg/sec
      500,   // 4854 deg/sec
      500,   // 4914 deg/sec
      500,   // 4974 deg/sec
      500,   // 5034 deg/sec
      500,   // 4974 deg/sec
      500,   // 4914 deg/sec
      500,   // 4854 deg/sec
      500,   // 4794 deg/sec
      500,   // 4734 deg/sec
      500,   // 4674 deg/sec
      500,   // 4614 deg/sec
      500,   // 4554 deg/sec
      500,   // 4494 deg/sec
      500,   // 4434 deg/sec
      500,   // 4374 deg/sec
      500,   // 4314 deg/sec
      500,   // 4254 deg/sec
      500,   // 4194 deg/sec
      500,   // 4134 deg/sec
      500,   // 4074 deg/sec
      500,   // 4014 deg/sec
      500,   // 3954 deg/sec
      500,   // 3894 deg/sec
      500,   // 3834 deg/sec
      500,   // 3774 deg/sec
      500,   // 3714 deg/sec
      500,   // 3654 deg/sec
      500,   // 3594 deg/sec
      509,   // 3533 deg/sec
      518,   // 3472 deg/sec
      527,   // 3410 deg/sec
      537,   // 3347 deg/sec
      548,   // 3282 deg/sec
      559,   // 3217 deg/sec
      571,   // 3149 deg/sec
      584,   // 3081 deg/sec
      597,   // 3011 deg/sec
      612,   // 2939 deg/sec
      628,   // 2865 deg/sec
      645,   // 2790 deg/sec
      663,   // 2713 deg/sec
      683,   // 2633 deg/sec
      705,   // 2551 deg/sec
      729,   // 2466 deg/sec
      756,   // 2379 deg/sec
      786,   // 2288 deg/sec
      820,   // 2194 deg/sec
      858,   // 2095 deg/sec
      903,   // 1992 deg/sec
      955,   // 1884 deg/sec
     1017,   // 1769 deg/sec
     1092,   // 1647 deg/sec
     1186,   // 1516 deg/sec
};

static int interdelay_3 = 0;

static inline void reportSpeedInfo_3() {
    Serial.println( "" );
    Serial.println( "Speed 3 - faster" );
    Serial.println( "accelMag = 120.0" );
    Serial.println( "startSpd = 1200.0" );
    Serial.println( "minDelta = 500" );
    Serial.println( "interdelay = 0" );
    Serial.println( " 90 turns - 37ms each, 0.94 for 25 moves" );
    Serial.println( "180 turns - 62ms each, 1.56 for 25 moves" );
}


static const int delays90_4[] = {
     1500,   // 1200 deg/sec
     1301,   // 1383 deg/sec
     1167,   // 1541 deg/sec
     1068,   // 1684 deg/sec
      991,   // 1814 deg/sec
      929,   // 1935 deg/sec
      878,   // 2049 deg/sec
      834,   // 2156 deg/sec
      797,   // 2258 deg/sec
      764,   // 2355 deg/sec
      735,   // 2448 deg/sec
      709,   // 2538 deg/sec
      685,   // 2624 deg/sec
      664,   // 2708 deg/sec
      645,   // 2789 deg/sec
      627,   // 2868 deg/sec
      611,   // 2944 deg/sec
      596,   // 3019 deg/sec
      582,   // 3092 deg/sec
      569,   // 3163 deg/sec
      556,   // 3232 deg/sec
      545,   // 3300 deg/sec
      534,   // 3367 deg/sec
      524,   // 3432 deg/sec
      514,   // 3496 deg/sec
      505,   // 3559 deg/sec
      514,   // 3497 deg/sec
      524,   // 3434 deg/sec
      534,   // 3370 deg/sec
      544,   // 3305 deg/sec
      555,   // 3239 deg/sec
      567,   // 3171 deg/sec
      580,   // 3101 deg/sec
      593,   // 3031 deg/sec
      608,   // 2958 deg/sec
      624,   // 2884 deg/sec
      640,   // 2808 deg/sec
      659,   // 2730 deg/sec
      679,   // 2649 deg/sec
      701,   // 2566 deg/sec
      725,   // 2481 deg/sec
      752,   // 2392 deg/sec
      782,   // 2301 deg/sec
      816,   // 2205 deg/sec
      854,   // 2106 deg/sec
      899,   // 2001 deg/sec
      951,   // 1892 deg/sec
     1013,   // 1776 deg/sec
     1089,   // 1652 deg/sec
     1184,   // 1519 deg/sec
};

static const int delays180_4[] = {
     1500,   // 1200 deg/sec
     1301,   // 1383 deg/sec
     1167,   // 1541 deg/sec
     1068,   // 1684 deg/sec
      991,   // 1814 deg/sec
      929,   // 1935 deg/sec
      878,   // 2049 deg/sec
      834,   // 2156 deg/sec
      797,   // 2258 deg/sec
      764,   // 2355 deg/sec
      735,   // 2448 deg/sec
      709,   // 2538 deg/sec
      685,   // 2624 deg/sec
      664,   // 2708 deg/sec
      645,   // 2789 deg/sec
      627,   // 2868 deg/sec
      611,   // 2944 deg/sec
      596,   // 3019 deg/sec
      582,   // 3092 deg/sec
      569,   // 3163 deg/sec
      556,   // 3232 deg/sec
      545,   // 3300 deg/sec
      534,   // 3367 deg/sec
      524,   // 3432 deg/sec
      514,   // 3496 deg/sec
      505,   // 3559 deg/sec
      497,   // 3620 deg/sec
      488,   // 3681 deg/sec
      481,   // 3741 deg/sec
      473,   // 3799 deg/sec
      466,   // 3857 deg/sec
      459,   // 3914 deg/sec
      453,   // 3970 deg/sec
      447,   // 4025 deg/sec
      441,   // 4080 deg/sec
      435,   // 4134 deg/sec
      429,   // 4187 deg/sec
      424,   // 4239 deg/sec
      419,   // 4291 deg/sec
      414,   // 4342 deg/sec
      409,   // 4393 deg/sec
      405,   // 4443 deg/sec
      400,   // 4492 deg/sec
      400,   // 4541 deg/sec
      400,   // 4590 deg/sec
      400,   // 4639 deg/sec
      400,   // 4688 deg/sec
      400,   // 4736 deg/sec
      400,   // 4785 deg/sec
      400,   // 4834 deg/sec
      400,   // 4883 deg/sec
      400,   // 4834 deg/sec
      400,   // 4785 deg/sec
      400,   // 4736 deg/sec
      400,   // 4688 deg/sec
      400,   // 4639 deg/sec
      400,   // 4590 deg/sec
      400,   // 4541 deg/sec
      400,   // 4492 deg/sec
      405,   // 4443 deg/sec
      409,   // 4394 deg/sec
      414,   // 4344 deg/sec
      419,   // 4293 deg/sec
      424,   // 4242 deg/sec
      429,   // 4191 deg/sec
      434,   // 4138 deg/sec
      440,   // 4085 deg/sec
      446,   // 4031 deg/sec
      452,   // 3977 deg/sec
      458,   // 3922 deg/sec
      465,   // 3866 deg/sec
      472,   // 3809 deg/sec
      479,   // 3751 deg/sec
      487,   // 3693 deg/sec
      495,   // 3633 deg/sec
      503,   // 3573 deg/sec
      512,   // 3511 deg/sec
      521,   // 3449 deg/sec
      531,   // 3385 deg/sec
      542,   // 3320 deg/sec
      553,   // 3254 deg/sec
      564,   // 3187 deg/sec
      577,   // 3118 deg/sec
      590,   // 3047 deg/sec
      604,   // 2975 deg/sec
      620,   // 2902 deg/sec
      636,   // 2826 deg/sec
      654,   // 2748 deg/sec
      674,   // 2668 deg/sec
      695,   // 2586 deg/sec
      719,   // 2501 deg/sec
      745,   // 2413 deg/sec
      774,   // 2322 deg/sec
      807,   // 2228 deg/sec
      845,   // 2129 deg/sec
      888,   // 2026 deg/sec
      938,   // 1918 deg/sec
      997,   // 1803 deg/sec
     1070,   // 1682 deg/sec
     1160,   // 1551 deg/sec
};

static int interdelay_4 = 0;

static inline void reportSpeedInfo_4() {
    Serial.println( "" );
    Serial.println( "Speed 4 - ludicrous" );
    Serial.println( "accelMag = 122.0" );
    Serial.println( "startSpd = 1200.0" );
    Serial.println( "minDelta = 400" );
    Serial.println( "interdelay = 0" );
    Serial.println( " 90 turns - 37ms each, 0.93 for 25 moves" );
    Serial.println( "180 turns - 58ms each, 1.47 for 25 moves" );
}


static const int delays90_5[] = {
     1440,   // 1250 deg/sec
     1252,   // 1437 deg/sec
     1124,   // 1600 deg/sec
     1030,   // 1746 deg/sec
      957,   // 1880 deg/sec
      897,   // 2004 deg/sec
      848,   // 2121 deg/sec
      806,   // 2231 deg/sec
      770,   // 2336 deg/sec
      738,   // 2436 deg/sec
      710,   // 2532 deg/sec
      685,   // 2625 deg/sec
      663,   // 2714 deg/sec
      642,   // 2800 deg/sec
      624,   // 2884 deg/sec
      607,   // 2965 deg/sec
      591,   // 3044 deg/sec
      576,   // 3120 deg/sec
      563,   // 3195 deg/sec
      550,   // 3269 deg/sec
      538,   // 3340 deg/sec
      527,   // 3410 deg/sec
      517,   // 3479 deg/sec
      507,   // 3546 deg/sec
      498,   // 3612 deg/sec
      489,   // 3677 deg/sec
      498,   // 3613 deg/sec
      507,   // 3549 deg/sec
      516,   // 3483 deg/sec
      526,   // 3415 deg/sec
      537,   // 3347 deg/sec
      549,   // 3277 deg/sec
      561,   // 3206 deg/sec
      574,   // 3133 deg/sec
      588,   // 3058 deg/sec
      603,   // 2981 deg/sec
      619,   // 2903 deg/sec
      637,   // 2822 deg/sec
      656,   // 2739 deg/sec
      678,   // 2654 deg/sec
      701,   // 2566 deg/sec
      727,   // 2475 deg/sec
      756,   // 2380 deg/sec
      788,   // 2282 deg/sec
      825,   // 2179 deg/sec
      868,   // 2072 deg/sec
      918,   // 1959 deg/sec
      978,   // 1840 deg/sec
     1050,   // 1713 deg/sec
     1141,   // 1576 deg/sec
};

static const int delays180_5[] = {
     1440,   // 1250 deg/sec
     1252,   // 1437 deg/sec
     1124,   // 1600 deg/sec
     1030,   // 1746 deg/sec
      957,   // 1880 deg/sec
      897,   // 2004 deg/sec
      848,   // 2121 deg/sec
      806,   // 2231 deg/sec
      770,   // 2336 deg/sec
      738,   // 2436 deg/sec
      710,   // 2532 deg/sec
      685,   // 2625 deg/sec
      663,   // 2714 deg/sec
      642,   // 2800 deg/sec
      624,   // 2884 deg/sec
      607,   // 2965 deg/sec
      591,   // 3044 deg/sec
      576,   // 3120 deg/sec
      563,   // 3195 deg/sec
      550,   // 3269 deg/sec
      538,   // 3340 deg/sec
      527,   // 3410 deg/sec
      517,   // 3479 deg/sec
      507,   // 3546 deg/sec
      498,   // 3612 deg/sec
      489,   // 3677 deg/sec
      481,   // 3741 deg/sec
      473,   // 3803 deg/sec
      465,   // 3865 deg/sec
      458,   // 3925 deg/sec
      451,   // 3985 deg/sec
      445,   // 4043 deg/sec
      438,   // 4101 deg/sec
      432,   // 4158 deg/sec
      427,   // 4215 deg/sec
      421,   // 4270 deg/sec
      416,   // 4325 deg/sec
      411,   // 4379 deg/sec
      406,   // 4432 deg/sec
      401,   // 4485 deg/sec
      396,   // 4537 deg/sec
      392,   // 4589 deg/sec
      387,   // 4640 deg/sec
      383,   // 4690 deg/sec
      379,   // 4740 deg/sec
      375,   // 4790 deg/sec
      371,   // 4839 deg/sec
      368,   // 4887 deg/sec
      364,   // 4935 deg/sec
      361,   // 4982 deg/sec
      357,   // 5029 deg/sec
      361,   // 4983 deg/sec
      364,   // 4936 deg/sec
      368,   // 4888 deg/sec
      371,   // 4840 deg/sec
      375,   // 4792 deg/sec
      379,   // 4743 deg/sec
      383,   // 4694 deg/sec
      387,   // 4644 deg/sec
      391,   // 4594 deg/sec
      396,   // 4543 deg/sec
      400,   // 4491 deg/sec
      405,   // 4439 deg/sec
      410,   // 4386 deg/sec
      415,   // 4333 deg/sec
      420,   // 4279 deg/sec
      426,   // 4224 deg/sec
      431,   // 4169 deg/sec
      437,   // 4113 deg/sec
      443,   // 4056 deg/sec
      450,   // 3998 deg/sec
      456,   // 3940 deg/sec
      463,   // 3880 deg/sec
      471,   // 3820 deg/sec
      478,   // 3759 deg/sec
      486,   // 3697 deg/sec
      495,   // 3633 deg/sec
      504,   // 3569 deg/sec
      513,   // 3503 deg/sec
      523,   // 3437 deg/sec
      534,   // 3369 deg/sec
      545,   // 3299 deg/sec
      557,   // 3228 deg/sec
      570,   // 3156 deg/sec
      584,   // 3082 deg/sec
      598,   // 3006 deg/sec
      614,   // 2928 deg/sec
      631,   // 2848 deg/sec
      650,   // 2766 deg/sec
      671,   // 2681 deg/sec
      693,   // 2594 deg/sec
      718,   // 2504 deg/sec
      746,   // 2410 deg/sec
      778,   // 2313 deg/sec
      813,   // 2212 deg/sec
      854,   // 2106 deg/sec
      901,   // 1995 deg/sec
      958,   // 1878 deg/sec
     1026,   // 1753 deg/sec
     1110,   // 1620 deg/sec
};

static int interdelay_5 = 0;

static inline void reportSpeedInfo_5() {
    Serial.println( "" );
    Serial.println( "Speed 5 - ludicrouser" );
    Serial.println( "accelMag = 130.0" );
    Serial.println( "startSpd = 1250.0" );
    Serial.println( "minDelta = 350" );
    Serial.println( "interdelay = 0" );
    Serial.println( " 90 turns - 35ms each, 0.90 for 25 moves" );
    Serial.println( "180 turns - 56ms each, 1.41 for 25 moves" );
}


static inline void reportSpeedInfo() {
    switch( speed ) {
    default:
    case 1: reportSpeedInfo_1(); break;
    case 3: reportSpeedInfo_3(); break;
    case 4: reportSpeedInfo_4(); break;
    case 5: reportSpeedInfo_5(); break;
    }
}

static inline int getInterDelay() {
    switch( speed ) {
    default:
    case 1: return interdelay_1;
    case 3: return interdelay_3;
    case 4: return interdelay_4;
    case 5: return interdelay_5;
    }
}

static inline int getDelay90( int i ) {
    switch( speed ) {
    default:
    case 1: return delays90_1[ i ];
    case 3: return delays90_3[ i ];
    case 4: return delays90_4[ i ];
    case 5: return delays90_5[ i ];
    }
}

static inline int getDelay180( int i ) {
    switch( speed ) {
    default:
    case 1: return delays180_1[ i ];
    case 3: return delays180_3[ i ];
    case 4: return delays180_4[ i ];
    case 5: return delays180_5[ i ];
    }
}
