#include <stdio.h>
#include <stdint.h>

/*ARM64 logical instructions have an 'interesting' immediate encoding.
  All valid values are in the table below, which we perform a binary
  search over*/
#define IMM_NR 1302
static uint32_t imm_table[][2] =
{
	{0x800, 0x00000001},
	{0xfc0, 0x00000002},
	{0x801, 0x00000003},
	{0xf80, 0x00000004},
	{0xfc1, 0x00000006},
	{0x802, 0x00000007},
	{0xf40, 0x00000008},
	{0xf81, 0x0000000c},
	{0xfc2, 0x0000000e},
	{0x803, 0x0000000f},
	{0xf00, 0x00000010},
	{0xf41, 0x00000018},
	{0xf82, 0x0000001c},
	{0xfc3, 0x0000001e},
	{0x804, 0x0000001f},
	{0xec0, 0x00000020},
	{0xf01, 0x00000030},
	{0xf42, 0x00000038},
	{0xf83, 0x0000003c},
	{0xfc4, 0x0000003e},
	{0x805, 0x0000003f},
	{0xe80, 0x00000040},
	{0xec1, 0x00000060},
	{0xf02, 0x00000070},
	{0xf43, 0x00000078},
	{0xf84, 0x0000007c},
	{0xfc5, 0x0000007e},
	{0x806, 0x0000007f},
	{0xe40, 0x00000080},
	{0xe81, 0x000000c0},
	{0xec2, 0x000000e0},
	{0xf03, 0x000000f0},
	{0xf44, 0x000000f8},
	{0xf85, 0x000000fc},
	{0xfc6, 0x000000fe},
	{0x807, 0x000000ff},
	{0xe00, 0x00000100},
	{0xe41, 0x00000180},
	{0xe82, 0x000001c0},
	{0xec3, 0x000001e0},
	{0xf04, 0x000001f0},
	{0xf45, 0x000001f8},
	{0xf86, 0x000001fc},
	{0xfc7, 0x000001fe},
	{0x808, 0x000001ff},
	{0xdc0, 0x00000200},
	{0xe01, 0x00000300},
	{0xe42, 0x00000380},
	{0xe83, 0x000003c0},
	{0xec4, 0x000003e0},
	{0xf05, 0x000003f0},
	{0xf46, 0x000003f8},
	{0xf87, 0x000003fc},
	{0xfc8, 0x000003fe},
	{0x809, 0x000003ff},
	{0xd80, 0x00000400},
	{0xdc1, 0x00000600},
	{0xe02, 0x00000700},
	{0xe43, 0x00000780},
	{0xe84, 0x000007c0},
	{0xec5, 0x000007e0},
	{0xf06, 0x000007f0},
	{0xf47, 0x000007f8},
	{0xf88, 0x000007fc},
	{0xfc9, 0x000007fe},
	{0x80a, 0x000007ff},
	{0xd40, 0x00000800},
	{0xd81, 0x00000c00},
	{0xdc2, 0x00000e00},
	{0xe03, 0x00000f00},
	{0xe44, 0x00000f80},
	{0xe85, 0x00000fc0},
	{0xec6, 0x00000fe0},
	{0xf07, 0x00000ff0},
	{0xf48, 0x00000ff8},
	{0xf89, 0x00000ffc},
	{0xfca, 0x00000ffe},
	{0x80b, 0x00000fff},
	{0xd00, 0x00001000},
	{0xd41, 0x00001800},
	{0xd82, 0x00001c00},
	{0xdc3, 0x00001e00},
	{0xe04, 0x00001f00},
	{0xe45, 0x00001f80},
	{0xe86, 0x00001fc0},
	{0xec7, 0x00001fe0},
	{0xf08, 0x00001ff0},
	{0xf49, 0x00001ff8},
	{0xf8a, 0x00001ffc},
	{0xfcb, 0x00001ffe},
	{0x80c, 0x00001fff},
	{0xcc0, 0x00002000},
	{0xd01, 0x00003000},
	{0xd42, 0x00003800},
	{0xd83, 0x00003c00},
	{0xdc4, 0x00003e00},
	{0xe05, 0x00003f00},
	{0xe46, 0x00003f80},
	{0xe87, 0x00003fc0},
	{0xec8, 0x00003fe0},
	{0xf09, 0x00003ff0},
	{0xf4a, 0x00003ff8},
	{0xf8b, 0x00003ffc},
	{0xfcc, 0x00003ffe},
	{0x80d, 0x00003fff},
	{0xc80, 0x00004000},
	{0xcc1, 0x00006000},
	{0xd02, 0x00007000},
	{0xd43, 0x00007800},
	{0xd84, 0x00007c00},
	{0xdc5, 0x00007e00},
	{0xe06, 0x00007f00},
	{0xe47, 0x00007f80},
	{0xe88, 0x00007fc0},
	{0xec9, 0x00007fe0},
	{0xf0a, 0x00007ff0},
	{0xf4b, 0x00007ff8},
	{0xf8c, 0x00007ffc},
	{0xfcd, 0x00007ffe},
	{0x80e, 0x00007fff},
	{0xc40, 0x00008000},
	{0xc81, 0x0000c000},
	{0xcc2, 0x0000e000},
	{0xd03, 0x0000f000},
	{0xd44, 0x0000f800},
	{0xd85, 0x0000fc00},
	{0xdc6, 0x0000fe00},
	{0xe07, 0x0000ff00},
	{0xe48, 0x0000ff80},
	{0xe89, 0x0000ffc0},
	{0xeca, 0x0000ffe0},
	{0xf0b, 0x0000fff0},
	{0xf4c, 0x0000fff8},
	{0xf8d, 0x0000fffc},
	{0xfce, 0x0000fffe},
	{0x80f, 0x0000ffff},
	{0xc00, 0x00010000},
	{0xc20, 0x00010001},
	{0xc41, 0x00018000},
	{0xc82, 0x0001c000},
	{0xcc3, 0x0001e000},
	{0xd04, 0x0001f000},
	{0xd45, 0x0001f800},
	{0xd86, 0x0001fc00},
	{0xdc7, 0x0001fe00},
	{0xe08, 0x0001ff00},
	{0xe49, 0x0001ff80},
	{0xe8a, 0x0001ffc0},
	{0xecb, 0x0001ffe0},
	{0xf0c, 0x0001fff0},
	{0xf4d, 0x0001fff8},
	{0xf8e, 0x0001fffc},
	{0xfcf, 0x0001fffe},
	{0x810, 0x0001ffff},
	{0xbc0, 0x00020000},
	{0xfe0, 0x00020002},
	{0xc01, 0x00030000},
	{0xc21, 0x00030003},
	{0xc42, 0x00038000},
	{0xc83, 0x0003c000},
	{0xcc4, 0x0003e000},
	{0xd05, 0x0003f000},
	{0xd46, 0x0003f800},
	{0xd87, 0x0003fc00},
	{0xdc8, 0x0003fe00},
	{0xe09, 0x0003ff00},
	{0xe4a, 0x0003ff80},
	{0xe8b, 0x0003ffc0},
	{0xecc, 0x0003ffe0},
	{0xf0d, 0x0003fff0},
	{0xf4e, 0x0003fff8},
	{0xf8f, 0x0003fffc},
	{0xfd0, 0x0003fffe},
	{0x811, 0x0003ffff},
	{0xb80, 0x00040000},
	{0xfa0, 0x00040004},
	{0xbc1, 0x00060000},
	{0xfe1, 0x00060006},
	{0xc02, 0x00070000},
	{0xc22, 0x00070007},
	{0xc43, 0x00078000},
	{0xc84, 0x0007c000},
	{0xcc5, 0x0007e000},
	{0xd06, 0x0007f000},
	{0xd47, 0x0007f800},
	{0xd88, 0x0007fc00},
	{0xdc9, 0x0007fe00},
	{0xe0a, 0x0007ff00},
	{0xe4b, 0x0007ff80},
	{0xe8c, 0x0007ffc0},
	{0xecd, 0x0007ffe0},
	{0xf0e, 0x0007fff0},
	{0xf4f, 0x0007fff8},
	{0xf90, 0x0007fffc},
	{0xfd1, 0x0007fffe},
	{0x812, 0x0007ffff},
	{0xb40, 0x00080000},
	{0xf60, 0x00080008},
	{0xb81, 0x000c0000},
	{0xfa1, 0x000c000c},
	{0xbc2, 0x000e0000},
	{0xfe2, 0x000e000e},
	{0xc03, 0x000f0000},
	{0xc23, 0x000f000f},
	{0xc44, 0x000f8000},
	{0xc85, 0x000fc000},
	{0xcc6, 0x000fe000},
	{0xd07, 0x000ff000},
	{0xd48, 0x000ff800},
	{0xd89, 0x000ffc00},
	{0xdca, 0x000ffe00},
	{0xe0b, 0x000fff00},
	{0xe4c, 0x000fff80},
	{0xe8d, 0x000fffc0},
	{0xece, 0x000fffe0},
	{0xf0f, 0x000ffff0},
	{0xf50, 0x000ffff8},
	{0xf91, 0x000ffffc},
	{0xfd2, 0x000ffffe},
	{0x813, 0x000fffff},
	{0xb00, 0x00100000},
	{0xf20, 0x00100010},
	{0xb41, 0x00180000},
	{0xf61, 0x00180018},
	{0xb82, 0x001c0000},
	{0xfa2, 0x001c001c},
	{0xbc3, 0x001e0000},
	{0xfe3, 0x001e001e},
	{0xc04, 0x001f0000},
	{0xc24, 0x001f001f},
	{0xc45, 0x001f8000},
	{0xc86, 0x001fc000},
	{0xcc7, 0x001fe000},
	{0xd08, 0x001ff000},
	{0xd49, 0x001ff800},
	{0xd8a, 0x001ffc00},
	{0xdcb, 0x001ffe00},
	{0xe0c, 0x001fff00},
	{0xe4d, 0x001fff80},
	{0xe8e, 0x001fffc0},
	{0xecf, 0x001fffe0},
	{0xf10, 0x001ffff0},
	{0xf51, 0x001ffff8},
	{0xf92, 0x001ffffc},
	{0xfd3, 0x001ffffe},
	{0x814, 0x001fffff},
	{0xac0, 0x00200000},
	{0xee0, 0x00200020},
	{0xb01, 0x00300000},
	{0xf21, 0x00300030},
	{0xb42, 0x00380000},
	{0xf62, 0x00380038},
	{0xb83, 0x003c0000},
	{0xfa3, 0x003c003c},
	{0xbc4, 0x003e0000},
	{0xfe4, 0x003e003e},
	{0xc05, 0x003f0000},
	{0xc25, 0x003f003f},
	{0xc46, 0x003f8000},
	{0xc87, 0x003fc000},
	{0xcc8, 0x003fe000},
	{0xd09, 0x003ff000},
	{0xd4a, 0x003ff800},
	{0xd8b, 0x003ffc00},
	{0xdcc, 0x003ffe00},
	{0xe0d, 0x003fff00},
	{0xe4e, 0x003fff80},
	{0xe8f, 0x003fffc0},
	{0xed0, 0x003fffe0},
	{0xf11, 0x003ffff0},
	{0xf52, 0x003ffff8},
	{0xf93, 0x003ffffc},
	{0xfd4, 0x003ffffe},
	{0x815, 0x003fffff},
	{0xa80, 0x00400000},
	{0xea0, 0x00400040},
	{0xac1, 0x00600000},
	{0xee1, 0x00600060},
	{0xb02, 0x00700000},
	{0xf22, 0x00700070},
	{0xb43, 0x00780000},
	{0xf63, 0x00780078},
	{0xb84, 0x007c0000},
	{0xfa4, 0x007c007c},
	{0xbc5, 0x007e0000},
	{0xfe5, 0x007e007e},
	{0xc06, 0x007f0000},
	{0xc26, 0x007f007f},
	{0xc47, 0x007f8000},
	{0xc88, 0x007fc000},
	{0xcc9, 0x007fe000},
	{0xd0a, 0x007ff000},
	{0xd4b, 0x007ff800},
	{0xd8c, 0x007ffc00},
	{0xdcd, 0x007ffe00},
	{0xe0e, 0x007fff00},
	{0xe4f, 0x007fff80},
	{0xe90, 0x007fffc0},
	{0xed1, 0x007fffe0},
	{0xf12, 0x007ffff0},
	{0xf53, 0x007ffff8},
	{0xf94, 0x007ffffc},
	{0xfd5, 0x007ffffe},
	{0x816, 0x007fffff},
	{0xa40, 0x00800000},
	{0xe60, 0x00800080},
	{0xa81, 0x00c00000},
	{0xea1, 0x00c000c0},
	{0xac2, 0x00e00000},
	{0xee2, 0x00e000e0},
	{0xb03, 0x00f00000},
	{0xf23, 0x00f000f0},
	{0xb44, 0x00f80000},
	{0xf64, 0x00f800f8},
	{0xb85, 0x00fc0000},
	{0xfa5, 0x00fc00fc},
	{0xbc6, 0x00fe0000},
	{0xfe6, 0x00fe00fe},
	{0xc07, 0x00ff0000},
	{0xc27, 0x00ff00ff},
	{0xc48, 0x00ff8000},
	{0xc89, 0x00ffc000},
	{0xcca, 0x00ffe000},
	{0xd0b, 0x00fff000},
	{0xd4c, 0x00fff800},
	{0xd8d, 0x00fffc00},
	{0xdce, 0x00fffe00},
	{0xe0f, 0x00ffff00},
	{0xe50, 0x00ffff80},
	{0xe91, 0x00ffffc0},
	{0xed2, 0x00ffffe0},
	{0xf13, 0x00fffff0},
	{0xf54, 0x00fffff8},
	{0xf95, 0x00fffffc},
	{0xfd6, 0x00fffffe},
	{0x817, 0x00ffffff},
	{0xa00, 0x01000000},
	{0xe20, 0x01000100},
	{0xe30, 0x01010101},
	{0xa41, 0x01800000},
	{0xe61, 0x01800180},
	{0xa82, 0x01c00000},
	{0xea2, 0x01c001c0},
	{0xac3, 0x01e00000},
	{0xee3, 0x01e001e0},
	{0xb04, 0x01f00000},
	{0xf24, 0x01f001f0},
	{0xb45, 0x01f80000},
	{0xf65, 0x01f801f8},
	{0xb86, 0x01fc0000},
	{0xfa6, 0x01fc01fc},
	{0xbc7, 0x01fe0000},
	{0xfe7, 0x01fe01fe},
	{0xc08, 0x01ff0000},
	{0xc28, 0x01ff01ff},
	{0xc49, 0x01ff8000},
	{0xc8a, 0x01ffc000},
	{0xccb, 0x01ffe000},
	{0xd0c, 0x01fff000},
	{0xd4d, 0x01fff800},
	{0xd8e, 0x01fffc00},
	{0xdcf, 0x01fffe00},
	{0xe10, 0x01ffff00},
	{0xe51, 0x01ffff80},
	{0xe92, 0x01ffffc0},
	{0xed3, 0x01ffffe0},
	{0xf14, 0x01fffff0},
	{0xf55, 0x01fffff8},
	{0xf96, 0x01fffffc},
	{0xfd7, 0x01fffffe},
	{0x818, 0x01ffffff},
	{0x9c0, 0x02000000},
	{0xde0, 0x02000200},
	{0xff0, 0x02020202},
	{0xa01, 0x03000000},
	{0xe21, 0x03000300},
	{0xe31, 0x03030303},
	{0xa42, 0x03800000},
	{0xe62, 0x03800380},
	{0xa83, 0x03c00000},
	{0xea3, 0x03c003c0},
	{0xac4, 0x03e00000},
	{0xee4, 0x03e003e0},
	{0xb05, 0x03f00000},
	{0xf25, 0x03f003f0},
	{0xb46, 0x03f80000},
	{0xf66, 0x03f803f8},
	{0xb87, 0x03fc0000},
	{0xfa7, 0x03fc03fc},
	{0xbc8, 0x03fe0000},
	{0xfe8, 0x03fe03fe},
	{0xc09, 0x03ff0000},
	{0xc29, 0x03ff03ff},
	{0xc4a, 0x03ff8000},
	{0xc8b, 0x03ffc000},
	{0xccc, 0x03ffe000},
	{0xd0d, 0x03fff000},
	{0xd4e, 0x03fff800},
	{0xd8f, 0x03fffc00},
	{0xdd0, 0x03fffe00},
	{0xe11, 0x03ffff00},
	{0xe52, 0x03ffff80},
	{0xe93, 0x03ffffc0},
	{0xed4, 0x03ffffe0},
	{0xf15, 0x03fffff0},
	{0xf56, 0x03fffff8},
	{0xf97, 0x03fffffc},
	{0xfd8, 0x03fffffe},
	{0x819, 0x03ffffff},
	{0x980, 0x04000000},
	{0xda0, 0x04000400},
	{0xfb0, 0x04040404},
	{0x9c1, 0x06000000},
	{0xde1, 0x06000600},
	{0xff1, 0x06060606},
	{0xa02, 0x07000000},
	{0xe22, 0x07000700},
	{0xe32, 0x07070707},
	{0xa43, 0x07800000},
	{0xe63, 0x07800780},
	{0xa84, 0x07c00000},
	{0xea4, 0x07c007c0},
	{0xac5, 0x07e00000},
	{0xee5, 0x07e007e0},
	{0xb06, 0x07f00000},
	{0xf26, 0x07f007f0},
	{0xb47, 0x07f80000},
	{0xf67, 0x07f807f8},
	{0xb88, 0x07fc0000},
	{0xfa8, 0x07fc07fc},
	{0xbc9, 0x07fe0000},
	{0xfe9, 0x07fe07fe},
	{0xc0a, 0x07ff0000},
	{0xc2a, 0x07ff07ff},
	{0xc4b, 0x07ff8000},
	{0xc8c, 0x07ffc000},
	{0xccd, 0x07ffe000},
	{0xd0e, 0x07fff000},
	{0xd4f, 0x07fff800},
	{0xd90, 0x07fffc00},
	{0xdd1, 0x07fffe00},
	{0xe12, 0x07ffff00},
	{0xe53, 0x07ffff80},
	{0xe94, 0x07ffffc0},
	{0xed5, 0x07ffffe0},
	{0xf16, 0x07fffff0},
	{0xf57, 0x07fffff8},
	{0xf98, 0x07fffffc},
	{0xfd9, 0x07fffffe},
	{0x81a, 0x07ffffff},
	{0x940, 0x08000000},
	{0xd60, 0x08000800},
	{0xf70, 0x08080808},
	{0x981, 0x0c000000},
	{0xda1, 0x0c000c00},
	{0xfb1, 0x0c0c0c0c},
	{0x9c2, 0x0e000000},
	{0xde2, 0x0e000e00},
	{0xff2, 0x0e0e0e0e},
	{0xa03, 0x0f000000},
	{0xe23, 0x0f000f00},
	{0xe33, 0x0f0f0f0f},
	{0xa44, 0x0f800000},
	{0xe64, 0x0f800f80},
	{0xa85, 0x0fc00000},
	{0xea5, 0x0fc00fc0},
	{0xac6, 0x0fe00000},
	{0xee6, 0x0fe00fe0},
	{0xb07, 0x0ff00000},
	{0xf27, 0x0ff00ff0},
	{0xb48, 0x0ff80000},
	{0xf68, 0x0ff80ff8},
	{0xb89, 0x0ffc0000},
	{0xfa9, 0x0ffc0ffc},
	{0xbca, 0x0ffe0000},
	{0xfea, 0x0ffe0ffe},
	{0xc0b, 0x0fff0000},
	{0xc2b, 0x0fff0fff},
	{0xc4c, 0x0fff8000},
	{0xc8d, 0x0fffc000},
	{0xcce, 0x0fffe000},
	{0xd0f, 0x0ffff000},
	{0xd50, 0x0ffff800},
	{0xd91, 0x0ffffc00},
	{0xdd2, 0x0ffffe00},
	{0xe13, 0x0fffff00},
	{0xe54, 0x0fffff80},
	{0xe95, 0x0fffffc0},
	{0xed6, 0x0fffffe0},
	{0xf17, 0x0ffffff0},
	{0xf58, 0x0ffffff8},
	{0xf99, 0x0ffffffc},
	{0xfda, 0x0ffffffe},
	{0x81b, 0x0fffffff},
	{0x900, 0x10000000},
	{0xd20, 0x10001000},
	{0xf30, 0x10101010},
	{0xf38, 0x11111111},
	{0x941, 0x18000000},
	{0xd61, 0x18001800},
	{0xf71, 0x18181818},
	{0x982, 0x1c000000},
	{0xda2, 0x1c001c00},
	{0xfb2, 0x1c1c1c1c},
	{0x9c3, 0x1e000000},
	{0xde3, 0x1e001e00},
	{0xff3, 0x1e1e1e1e},
	{0xa04, 0x1f000000},
	{0xe24, 0x1f001f00},
	{0xe34, 0x1f1f1f1f},
	{0xa45, 0x1f800000},
	{0xe65, 0x1f801f80},
	{0xa86, 0x1fc00000},
	{0xea6, 0x1fc01fc0},
	{0xac7, 0x1fe00000},
	{0xee7, 0x1fe01fe0},
	{0xb08, 0x1ff00000},
	{0xf28, 0x1ff01ff0},
	{0xb49, 0x1ff80000},
	{0xf69, 0x1ff81ff8},
	{0xb8a, 0x1ffc0000},
	{0xfaa, 0x1ffc1ffc},
	{0xbcb, 0x1ffe0000},
	{0xfeb, 0x1ffe1ffe},
	{0xc0c, 0x1fff0000},
	{0xc2c, 0x1fff1fff},
	{0xc4d, 0x1fff8000},
	{0xc8e, 0x1fffc000},
	{0xccf, 0x1fffe000},
	{0xd10, 0x1ffff000},
	{0xd51, 0x1ffff800},
	{0xd92, 0x1ffffc00},
	{0xdd3, 0x1ffffe00},
	{0xe14, 0x1fffff00},
	{0xe55, 0x1fffff80},
	{0xe96, 0x1fffffc0},
	{0xed7, 0x1fffffe0},
	{0xf18, 0x1ffffff0},
	{0xf59, 0x1ffffff8},
	{0xf9a, 0x1ffffffc},
	{0xfdb, 0x1ffffffe},
	{0x81c, 0x1fffffff},
	{0x8c0, 0x20000000},
	{0xce0, 0x20002000},
	{0xef0, 0x20202020},
	{0xff8, 0x22222222},
	{0x901, 0x30000000},
	{0xd21, 0x30003000},
	{0xf31, 0x30303030},
	{0xf39, 0x33333333},
	{0x942, 0x38000000},
	{0xd62, 0x38003800},
	{0xf72, 0x38383838},
	{0x983, 0x3c000000},
	{0xda3, 0x3c003c00},
	{0xfb3, 0x3c3c3c3c},
	{0x9c4, 0x3e000000},
	{0xde4, 0x3e003e00},
	{0xff4, 0x3e3e3e3e},
	{0xa05, 0x3f000000},
	{0xe25, 0x3f003f00},
	{0xe35, 0x3f3f3f3f},
	{0xa46, 0x3f800000},
	{0xe66, 0x3f803f80},
	{0xa87, 0x3fc00000},
	{0xea7, 0x3fc03fc0},
	{0xac8, 0x3fe00000},
	{0xee8, 0x3fe03fe0},
	{0xb09, 0x3ff00000},
	{0xf29, 0x3ff03ff0},
	{0xb4a, 0x3ff80000},
	{0xf6a, 0x3ff83ff8},
	{0xb8b, 0x3ffc0000},
	{0xfab, 0x3ffc3ffc},
	{0xbcc, 0x3ffe0000},
	{0xfec, 0x3ffe3ffe},
	{0xc0d, 0x3fff0000},
	{0xc2d, 0x3fff3fff},
	{0xc4e, 0x3fff8000},
	{0xc8f, 0x3fffc000},
	{0xcd0, 0x3fffe000},
	{0xd11, 0x3ffff000},
	{0xd52, 0x3ffff800},
	{0xd93, 0x3ffffc00},
	{0xdd4, 0x3ffffe00},
	{0xe15, 0x3fffff00},
	{0xe56, 0x3fffff80},
	{0xe97, 0x3fffffc0},
	{0xed8, 0x3fffffe0},
	{0xf19, 0x3ffffff0},
	{0xf5a, 0x3ffffff8},
	{0xf9b, 0x3ffffffc},
	{0xfdc, 0x3ffffffe},
	{0x81d, 0x3fffffff},
	{0x880, 0x40000000},
	{0xca0, 0x40004000},
	{0xeb0, 0x40404040},
	{0xfb8, 0x44444444},
	{0xfbc, 0x55555555},
	{0x8c1, 0x60000000},
	{0xce1, 0x60006000},
	{0xef1, 0x60606060},
	{0xff9, 0x66666666},
	{0x902, 0x70000000},
	{0xd22, 0x70007000},
	{0xf32, 0x70707070},
	{0xf3a, 0x77777777},
	{0x943, 0x78000000},
	{0xd63, 0x78007800},
	{0xf73, 0x78787878},
	{0x984, 0x7c000000},
	{0xda4, 0x7c007c00},
	{0xfb4, 0x7c7c7c7c},
	{0x9c5, 0x7e000000},
	{0xde5, 0x7e007e00},
	{0xff5, 0x7e7e7e7e},
	{0xa06, 0x7f000000},
	{0xe26, 0x7f007f00},
	{0xe36, 0x7f7f7f7f},
	{0xa47, 0x7f800000},
	{0xe67, 0x7f807f80},
	{0xa88, 0x7fc00000},
	{0xea8, 0x7fc07fc0},
	{0xac9, 0x7fe00000},
	{0xee9, 0x7fe07fe0},
	{0xb0a, 0x7ff00000},
	{0xf2a, 0x7ff07ff0},
	{0xb4b, 0x7ff80000},
	{0xf6b, 0x7ff87ff8},
	{0xb8c, 0x7ffc0000},
	{0xfac, 0x7ffc7ffc},
	{0xbcd, 0x7ffe0000},
	{0xfed, 0x7ffe7ffe},
	{0xc0e, 0x7fff0000},
	{0xc2e, 0x7fff7fff},
	{0xc4f, 0x7fff8000},
	{0xc90, 0x7fffc000},
	{0xcd1, 0x7fffe000},
	{0xd12, 0x7ffff000},
	{0xd53, 0x7ffff800},
	{0xd94, 0x7ffffc00},
	{0xdd5, 0x7ffffe00},
	{0xe16, 0x7fffff00},
	{0xe57, 0x7fffff80},
	{0xe98, 0x7fffffc0},
	{0xed9, 0x7fffffe0},
	{0xf1a, 0x7ffffff0},
	{0xf5b, 0x7ffffff8},
	{0xf9c, 0x7ffffffc},
	{0xfdd, 0x7ffffffe},
	{0x81e, 0x7fffffff},
	{0x840, 0x80000000},
	{0x841, 0x80000001},
	{0x842, 0x80000003},
	{0x843, 0x80000007},
	{0x844, 0x8000000f},
	{0x845, 0x8000001f},
	{0x846, 0x8000003f},
	{0x847, 0x8000007f},
	{0x848, 0x800000ff},
	{0x849, 0x800001ff},
	{0x84a, 0x800003ff},
	{0x84b, 0x800007ff},
	{0x84c, 0x80000fff},
	{0x84d, 0x80001fff},
	{0x84e, 0x80003fff},
	{0x84f, 0x80007fff},
	{0xc60, 0x80008000},
	{0x850, 0x8000ffff},
	{0xc61, 0x80018001},
	{0x851, 0x8001ffff},
	{0xc62, 0x80038003},
	{0x852, 0x8003ffff},
	{0xc63, 0x80078007},
	{0x853, 0x8007ffff},
	{0xc64, 0x800f800f},
	{0x854, 0x800fffff},
	{0xc65, 0x801f801f},
	{0x855, 0x801fffff},
	{0xc66, 0x803f803f},
	{0x856, 0x803fffff},
	{0xc67, 0x807f807f},
	{0x857, 0x807fffff},
	{0xe70, 0x80808080},
	{0xc68, 0x80ff80ff},
	{0x858, 0x80ffffff},
	{0xe71, 0x81818181},
	{0xc69, 0x81ff81ff},
	{0x859, 0x81ffffff},
	{0xe72, 0x83838383},
	{0xc6a, 0x83ff83ff},
	{0x85a, 0x83ffffff},
	{0xe73, 0x87878787},
	{0xc6b, 0x87ff87ff},
	{0x85b, 0x87ffffff},
	{0xf78, 0x88888888},
	{0xe74, 0x8f8f8f8f},
	{0xc6c, 0x8fff8fff},
	{0x85c, 0x8fffffff},
	{0xf79, 0x99999999},
	{0xe75, 0x9f9f9f9f},
	{0xc6d, 0x9fff9fff},
	{0x85d, 0x9fffffff},
	{0xffc, 0xaaaaaaaa},
	{0xf7a, 0xbbbbbbbb},
	{0xe76, 0xbfbfbfbf},
	{0xc6e, 0xbfffbfff},
	{0x85e, 0xbfffffff},
	{0x881, 0xc0000000},
	{0x882, 0xc0000001},
	{0x883, 0xc0000003},
	{0x884, 0xc0000007},
	{0x885, 0xc000000f},
	{0x886, 0xc000001f},
	{0x887, 0xc000003f},
	{0x888, 0xc000007f},
	{0x889, 0xc00000ff},
	{0x88a, 0xc00001ff},
	{0x88b, 0xc00003ff},
	{0x88c, 0xc00007ff},
	{0x88d, 0xc0000fff},
	{0x88e, 0xc0001fff},
	{0x88f, 0xc0003fff},
	{0x890, 0xc0007fff},
	{0xca1, 0xc000c000},
	{0x891, 0xc000ffff},
	{0xca2, 0xc001c001},
	{0x892, 0xc001ffff},
	{0xca3, 0xc003c003},
	{0x893, 0xc003ffff},
	{0xca4, 0xc007c007},
	{0x894, 0xc007ffff},
	{0xca5, 0xc00fc00f},
	{0x895, 0xc00fffff},
	{0xca6, 0xc01fc01f},
	{0x896, 0xc01fffff},
	{0xca7, 0xc03fc03f},
	{0x897, 0xc03fffff},
	{0xca8, 0xc07fc07f},
	{0x898, 0xc07fffff},
	{0xeb1, 0xc0c0c0c0},
	{0xca9, 0xc0ffc0ff},
	{0x899, 0xc0ffffff},
	{0xeb2, 0xc1c1c1c1},
	{0xcaa, 0xc1ffc1ff},
	{0x89a, 0xc1ffffff},
	{0xeb3, 0xc3c3c3c3},
	{0xcab, 0xc3ffc3ff},
	{0x89b, 0xc3ffffff},
	{0xeb4, 0xc7c7c7c7},
	{0xcac, 0xc7ffc7ff},
	{0x89c, 0xc7ffffff},
	{0xfb9, 0xcccccccc},
	{0xeb5, 0xcfcfcfcf},
	{0xcad, 0xcfffcfff},
	{0x89d, 0xcfffffff},
	{0xfba, 0xdddddddd},
	{0xeb6, 0xdfdfdfdf},
	{0xcae, 0xdfffdfff},
	{0x89e, 0xdfffffff},
	{0x8c2, 0xe0000000},
	{0x8c3, 0xe0000001},
	{0x8c4, 0xe0000003},
	{0x8c5, 0xe0000007},
	{0x8c6, 0xe000000f},
	{0x8c7, 0xe000001f},
	{0x8c8, 0xe000003f},
	{0x8c9, 0xe000007f},
	{0x8ca, 0xe00000ff},
	{0x8cb, 0xe00001ff},
	{0x8cc, 0xe00003ff},
	{0x8cd, 0xe00007ff},
	{0x8ce, 0xe0000fff},
	{0x8cf, 0xe0001fff},
	{0x8d0, 0xe0003fff},
	{0x8d1, 0xe0007fff},
	{0xce2, 0xe000e000},
	{0x8d2, 0xe000ffff},
	{0xce3, 0xe001e001},
	{0x8d3, 0xe001ffff},
	{0xce4, 0xe003e003},
	{0x8d4, 0xe003ffff},
	{0xce5, 0xe007e007},
	{0x8d5, 0xe007ffff},
	{0xce6, 0xe00fe00f},
	{0x8d6, 0xe00fffff},
	{0xce7, 0xe01fe01f},
	{0x8d7, 0xe01fffff},
	{0xce8, 0xe03fe03f},
	{0x8d8, 0xe03fffff},
	{0xce9, 0xe07fe07f},
	{0x8d9, 0xe07fffff},
	{0xef2, 0xe0e0e0e0},
	{0xcea, 0xe0ffe0ff},
	{0x8da, 0xe0ffffff},
	{0xef3, 0xe1e1e1e1},
	{0xceb, 0xe1ffe1ff},
	{0x8db, 0xe1ffffff},
	{0xef4, 0xe3e3e3e3},
	{0xcec, 0xe3ffe3ff},
	{0x8dc, 0xe3ffffff},
	{0xef5, 0xe7e7e7e7},
	{0xced, 0xe7ffe7ff},
	{0x8dd, 0xe7ffffff},
	{0xffa, 0xeeeeeeee},
	{0xef6, 0xefefefef},
	{0xcee, 0xefffefff},
	{0x8de, 0xefffffff},
	{0x903, 0xf0000000},
	{0x904, 0xf0000001},
	{0x905, 0xf0000003},
	{0x906, 0xf0000007},
	{0x907, 0xf000000f},
	{0x908, 0xf000001f},
	{0x909, 0xf000003f},
	{0x90a, 0xf000007f},
	{0x90b, 0xf00000ff},
	{0x90c, 0xf00001ff},
	{0x90d, 0xf00003ff},
	{0x90e, 0xf00007ff},
	{0x90f, 0xf0000fff},
	{0x910, 0xf0001fff},
	{0x911, 0xf0003fff},
	{0x912, 0xf0007fff},
	{0xd23, 0xf000f000},
	{0x913, 0xf000ffff},
	{0xd24, 0xf001f001},
	{0x914, 0xf001ffff},
	{0xd25, 0xf003f003},
	{0x915, 0xf003ffff},
	{0xd26, 0xf007f007},
	{0x916, 0xf007ffff},
	{0xd27, 0xf00ff00f},
	{0x917, 0xf00fffff},
	{0xd28, 0xf01ff01f},
	{0x918, 0xf01fffff},
	{0xd29, 0xf03ff03f},
	{0x919, 0xf03fffff},
	{0xd2a, 0xf07ff07f},
	{0x91a, 0xf07fffff},
	{0xf33, 0xf0f0f0f0},
	{0xd2b, 0xf0fff0ff},
	{0x91b, 0xf0ffffff},
	{0xf34, 0xf1f1f1f1},
	{0xd2c, 0xf1fff1ff},
	{0x91c, 0xf1ffffff},
	{0xf35, 0xf3f3f3f3},
	{0xd2d, 0xf3fff3ff},
	{0x91d, 0xf3ffffff},
	{0xf36, 0xf7f7f7f7},
	{0xd2e, 0xf7fff7ff},
	{0x91e, 0xf7ffffff},
	{0x944, 0xf8000000},
	{0x945, 0xf8000001},
	{0x946, 0xf8000003},
	{0x947, 0xf8000007},
	{0x948, 0xf800000f},
	{0x949, 0xf800001f},
	{0x94a, 0xf800003f},
	{0x94b, 0xf800007f},
	{0x94c, 0xf80000ff},
	{0x94d, 0xf80001ff},
	{0x94e, 0xf80003ff},
	{0x94f, 0xf80007ff},
	{0x950, 0xf8000fff},
	{0x951, 0xf8001fff},
	{0x952, 0xf8003fff},
	{0x953, 0xf8007fff},
	{0xd64, 0xf800f800},
	{0x954, 0xf800ffff},
	{0xd65, 0xf801f801},
	{0x955, 0xf801ffff},
	{0xd66, 0xf803f803},
	{0x956, 0xf803ffff},
	{0xd67, 0xf807f807},
	{0x957, 0xf807ffff},
	{0xd68, 0xf80ff80f},
	{0x958, 0xf80fffff},
	{0xd69, 0xf81ff81f},
	{0x959, 0xf81fffff},
	{0xd6a, 0xf83ff83f},
	{0x95a, 0xf83fffff},
	{0xd6b, 0xf87ff87f},
	{0x95b, 0xf87fffff},
	{0xf74, 0xf8f8f8f8},
	{0xd6c, 0xf8fff8ff},
	{0x95c, 0xf8ffffff},
	{0xf75, 0xf9f9f9f9},
	{0xd6d, 0xf9fff9ff},
	{0x95d, 0xf9ffffff},
	{0xf76, 0xfbfbfbfb},
	{0xd6e, 0xfbfffbff},
	{0x95e, 0xfbffffff},
	{0x985, 0xfc000000},
	{0x986, 0xfc000001},
	{0x987, 0xfc000003},
	{0x988, 0xfc000007},
	{0x989, 0xfc00000f},
	{0x98a, 0xfc00001f},
	{0x98b, 0xfc00003f},
	{0x98c, 0xfc00007f},
	{0x98d, 0xfc0000ff},
	{0x98e, 0xfc0001ff},
	{0x98f, 0xfc0003ff},
	{0x990, 0xfc0007ff},
	{0x991, 0xfc000fff},
	{0x992, 0xfc001fff},
	{0x993, 0xfc003fff},
	{0x994, 0xfc007fff},
	{0xda5, 0xfc00fc00},
	{0x995, 0xfc00ffff},
	{0xda6, 0xfc01fc01},
	{0x996, 0xfc01ffff},
	{0xda7, 0xfc03fc03},
	{0x997, 0xfc03ffff},
	{0xda8, 0xfc07fc07},
	{0x998, 0xfc07ffff},
	{0xda9, 0xfc0ffc0f},
	{0x999, 0xfc0fffff},
	{0xdaa, 0xfc1ffc1f},
	{0x99a, 0xfc1fffff},
	{0xdab, 0xfc3ffc3f},
	{0x99b, 0xfc3fffff},
	{0xdac, 0xfc7ffc7f},
	{0x99c, 0xfc7fffff},
	{0xfb5, 0xfcfcfcfc},
	{0xdad, 0xfcfffcff},
	{0x99d, 0xfcffffff},
	{0xfb6, 0xfdfdfdfd},
	{0xdae, 0xfdfffdff},
	{0x99e, 0xfdffffff},
	{0x9c6, 0xfe000000},
	{0x9c7, 0xfe000001},
	{0x9c8, 0xfe000003},
	{0x9c9, 0xfe000007},
	{0x9ca, 0xfe00000f},
	{0x9cb, 0xfe00001f},
	{0x9cc, 0xfe00003f},
	{0x9cd, 0xfe00007f},
	{0x9ce, 0xfe0000ff},
	{0x9cf, 0xfe0001ff},
	{0x9d0, 0xfe0003ff},
	{0x9d1, 0xfe0007ff},
	{0x9d2, 0xfe000fff},
	{0x9d3, 0xfe001fff},
	{0x9d4, 0xfe003fff},
	{0x9d5, 0xfe007fff},
	{0xde6, 0xfe00fe00},
	{0x9d6, 0xfe00ffff},
	{0xde7, 0xfe01fe01},
	{0x9d7, 0xfe01ffff},
	{0xde8, 0xfe03fe03},
	{0x9d8, 0xfe03ffff},
	{0xde9, 0xfe07fe07},
	{0x9d9, 0xfe07ffff},
	{0xdea, 0xfe0ffe0f},
	{0x9da, 0xfe0fffff},
	{0xdeb, 0xfe1ffe1f},
	{0x9db, 0xfe1fffff},
	{0xdec, 0xfe3ffe3f},
	{0x9dc, 0xfe3fffff},
	{0xded, 0xfe7ffe7f},
	{0x9dd, 0xfe7fffff},
	{0xff6, 0xfefefefe},
	{0xdee, 0xfefffeff},
	{0x9de, 0xfeffffff},
	{0xa07, 0xff000000},
	{0xa08, 0xff000001},
	{0xa09, 0xff000003},
	{0xa0a, 0xff000007},
	{0xa0b, 0xff00000f},
	{0xa0c, 0xff00001f},
	{0xa0d, 0xff00003f},
	{0xa0e, 0xff00007f},
	{0xa0f, 0xff0000ff},
	{0xa10, 0xff0001ff},
	{0xa11, 0xff0003ff},
	{0xa12, 0xff0007ff},
	{0xa13, 0xff000fff},
	{0xa14, 0xff001fff},
	{0xa15, 0xff003fff},
	{0xa16, 0xff007fff},
	{0xe27, 0xff00ff00},
	{0xa17, 0xff00ffff},
	{0xe28, 0xff01ff01},
	{0xa18, 0xff01ffff},
	{0xe29, 0xff03ff03},
	{0xa19, 0xff03ffff},
	{0xe2a, 0xff07ff07},
	{0xa1a, 0xff07ffff},
	{0xe2b, 0xff0fff0f},
	{0xa1b, 0xff0fffff},
	{0xe2c, 0xff1fff1f},
	{0xa1c, 0xff1fffff},
	{0xe2d, 0xff3fff3f},
	{0xa1d, 0xff3fffff},
	{0xe2e, 0xff7fff7f},
	{0xa1e, 0xff7fffff},
	{0xa48, 0xff800000},
	{0xa49, 0xff800001},
	{0xa4a, 0xff800003},
	{0xa4b, 0xff800007},
	{0xa4c, 0xff80000f},
	{0xa4d, 0xff80001f},
	{0xa4e, 0xff80003f},
	{0xa4f, 0xff80007f},
	{0xa50, 0xff8000ff},
	{0xa51, 0xff8001ff},
	{0xa52, 0xff8003ff},
	{0xa53, 0xff8007ff},
	{0xa54, 0xff800fff},
	{0xa55, 0xff801fff},
	{0xa56, 0xff803fff},
	{0xa57, 0xff807fff},
	{0xe68, 0xff80ff80},
	{0xa58, 0xff80ffff},
	{0xe69, 0xff81ff81},
	{0xa59, 0xff81ffff},
	{0xe6a, 0xff83ff83},
	{0xa5a, 0xff83ffff},
	{0xe6b, 0xff87ff87},
	{0xa5b, 0xff87ffff},
	{0xe6c, 0xff8fff8f},
	{0xa5c, 0xff8fffff},
	{0xe6d, 0xff9fff9f},
	{0xa5d, 0xff9fffff},
	{0xe6e, 0xffbfffbf},
	{0xa5e, 0xffbfffff},
	{0xa89, 0xffc00000},
	{0xa8a, 0xffc00001},
	{0xa8b, 0xffc00003},
	{0xa8c, 0xffc00007},
	{0xa8d, 0xffc0000f},
	{0xa8e, 0xffc0001f},
	{0xa8f, 0xffc0003f},
	{0xa90, 0xffc0007f},
	{0xa91, 0xffc000ff},
	{0xa92, 0xffc001ff},
	{0xa93, 0xffc003ff},
	{0xa94, 0xffc007ff},
	{0xa95, 0xffc00fff},
	{0xa96, 0xffc01fff},
	{0xa97, 0xffc03fff},
	{0xa98, 0xffc07fff},
	{0xea9, 0xffc0ffc0},
	{0xa99, 0xffc0ffff},
	{0xeaa, 0xffc1ffc1},
	{0xa9a, 0xffc1ffff},
	{0xeab, 0xffc3ffc3},
	{0xa9b, 0xffc3ffff},
	{0xeac, 0xffc7ffc7},
	{0xa9c, 0xffc7ffff},
	{0xead, 0xffcfffcf},
	{0xa9d, 0xffcfffff},
	{0xeae, 0xffdfffdf},
	{0xa9e, 0xffdfffff},
	{0xaca, 0xffe00000},
	{0xacb, 0xffe00001},
	{0xacc, 0xffe00003},
	{0xacd, 0xffe00007},
	{0xace, 0xffe0000f},
	{0xacf, 0xffe0001f},
	{0xad0, 0xffe0003f},
	{0xad1, 0xffe0007f},
	{0xad2, 0xffe000ff},
	{0xad3, 0xffe001ff},
	{0xad4, 0xffe003ff},
	{0xad5, 0xffe007ff},
	{0xad6, 0xffe00fff},
	{0xad7, 0xffe01fff},
	{0xad8, 0xffe03fff},
	{0xad9, 0xffe07fff},
	{0xeea, 0xffe0ffe0},
	{0xada, 0xffe0ffff},
	{0xeeb, 0xffe1ffe1},
	{0xadb, 0xffe1ffff},
	{0xeec, 0xffe3ffe3},
	{0xadc, 0xffe3ffff},
	{0xeed, 0xffe7ffe7},
	{0xadd, 0xffe7ffff},
	{0xeee, 0xffefffef},
	{0xade, 0xffefffff},
	{0xb0b, 0xfff00000},
	{0xb0c, 0xfff00001},
	{0xb0d, 0xfff00003},
	{0xb0e, 0xfff00007},
	{0xb0f, 0xfff0000f},
	{0xb10, 0xfff0001f},
	{0xb11, 0xfff0003f},
	{0xb12, 0xfff0007f},
	{0xb13, 0xfff000ff},
	{0xb14, 0xfff001ff},
	{0xb15, 0xfff003ff},
	{0xb16, 0xfff007ff},
	{0xb17, 0xfff00fff},
	{0xb18, 0xfff01fff},
	{0xb19, 0xfff03fff},
	{0xb1a, 0xfff07fff},
	{0xf2b, 0xfff0fff0},
	{0xb1b, 0xfff0ffff},
	{0xf2c, 0xfff1fff1},
	{0xb1c, 0xfff1ffff},
	{0xf2d, 0xfff3fff3},
	{0xb1d, 0xfff3ffff},
	{0xf2e, 0xfff7fff7},
	{0xb1e, 0xfff7ffff},
	{0xb4c, 0xfff80000},
	{0xb4d, 0xfff80001},
	{0xb4e, 0xfff80003},
	{0xb4f, 0xfff80007},
	{0xb50, 0xfff8000f},
	{0xb51, 0xfff8001f},
	{0xb52, 0xfff8003f},
	{0xb53, 0xfff8007f},
	{0xb54, 0xfff800ff},
	{0xb55, 0xfff801ff},
	{0xb56, 0xfff803ff},
	{0xb57, 0xfff807ff},
	{0xb58, 0xfff80fff},
	{0xb59, 0xfff81fff},
	{0xb5a, 0xfff83fff},
	{0xb5b, 0xfff87fff},
	{0xf6c, 0xfff8fff8},
	{0xb5c, 0xfff8ffff},
	{0xf6d, 0xfff9fff9},
	{0xb5d, 0xfff9ffff},
	{0xf6e, 0xfffbfffb},
	{0xb5e, 0xfffbffff},
	{0xb8d, 0xfffc0000},
	{0xb8e, 0xfffc0001},
	{0xb8f, 0xfffc0003},
	{0xb90, 0xfffc0007},
	{0xb91, 0xfffc000f},
	{0xb92, 0xfffc001f},
	{0xb93, 0xfffc003f},
	{0xb94, 0xfffc007f},
	{0xb95, 0xfffc00ff},
	{0xb96, 0xfffc01ff},
	{0xb97, 0xfffc03ff},
	{0xb98, 0xfffc07ff},
	{0xb99, 0xfffc0fff},
	{0xb9a, 0xfffc1fff},
	{0xb9b, 0xfffc3fff},
	{0xb9c, 0xfffc7fff},
	{0xfad, 0xfffcfffc},
	{0xb9d, 0xfffcffff},
	{0xfae, 0xfffdfffd},
	{0xb9e, 0xfffdffff},
	{0xbce, 0xfffe0000},
	{0xbcf, 0xfffe0001},
	{0xbd0, 0xfffe0003},
	{0xbd1, 0xfffe0007},
	{0xbd2, 0xfffe000f},
	{0xbd3, 0xfffe001f},
	{0xbd4, 0xfffe003f},
	{0xbd5, 0xfffe007f},
	{0xbd6, 0xfffe00ff},
	{0xbd7, 0xfffe01ff},
	{0xbd8, 0xfffe03ff},
	{0xbd9, 0xfffe07ff},
	{0xbda, 0xfffe0fff},
	{0xbdb, 0xfffe1fff},
	{0xbdc, 0xfffe3fff},
	{0xbdd, 0xfffe7fff},
	{0xfee, 0xfffefffe},
	{0xbde, 0xfffeffff},
	{0xc0f, 0xffff0000},
	{0xc10, 0xffff0001},
	{0xc11, 0xffff0003},
	{0xc12, 0xffff0007},
	{0xc13, 0xffff000f},
	{0xc14, 0xffff001f},
	{0xc15, 0xffff003f},
	{0xc16, 0xffff007f},
	{0xc17, 0xffff00ff},
	{0xc18, 0xffff01ff},
	{0xc19, 0xffff03ff},
	{0xc1a, 0xffff07ff},
	{0xc1b, 0xffff0fff},
	{0xc1c, 0xffff1fff},
	{0xc1d, 0xffff3fff},
	{0xc1e, 0xffff7fff},
	{0xc50, 0xffff8000},
	{0xc51, 0xffff8001},
	{0xc52, 0xffff8003},
	{0xc53, 0xffff8007},
	{0xc54, 0xffff800f},
	{0xc55, 0xffff801f},
	{0xc56, 0xffff803f},
	{0xc57, 0xffff807f},
	{0xc58, 0xffff80ff},
	{0xc59, 0xffff81ff},
	{0xc5a, 0xffff83ff},
	{0xc5b, 0xffff87ff},
	{0xc5c, 0xffff8fff},
	{0xc5d, 0xffff9fff},
	{0xc5e, 0xffffbfff},
	{0xc91, 0xffffc000},
	{0xc92, 0xffffc001},
	{0xc93, 0xffffc003},
	{0xc94, 0xffffc007},
	{0xc95, 0xffffc00f},
	{0xc96, 0xffffc01f},
	{0xc97, 0xffffc03f},
	{0xc98, 0xffffc07f},
	{0xc99, 0xffffc0ff},
	{0xc9a, 0xffffc1ff},
	{0xc9b, 0xffffc3ff},
	{0xc9c, 0xffffc7ff},
	{0xc9d, 0xffffcfff},
	{0xc9e, 0xffffdfff},
	{0xcd2, 0xffffe000},
	{0xcd3, 0xffffe001},
	{0xcd4, 0xffffe003},
	{0xcd5, 0xffffe007},
	{0xcd6, 0xffffe00f},
	{0xcd7, 0xffffe01f},
	{0xcd8, 0xffffe03f},
	{0xcd9, 0xffffe07f},
	{0xcda, 0xffffe0ff},
	{0xcdb, 0xffffe1ff},
	{0xcdc, 0xffffe3ff},
	{0xcdd, 0xffffe7ff},
	{0xcde, 0xffffefff},
	{0xd13, 0xfffff000},
	{0xd14, 0xfffff001},
	{0xd15, 0xfffff003},
	{0xd16, 0xfffff007},
	{0xd17, 0xfffff00f},
	{0xd18, 0xfffff01f},
	{0xd19, 0xfffff03f},
	{0xd1a, 0xfffff07f},
	{0xd1b, 0xfffff0ff},
	{0xd1c, 0xfffff1ff},
	{0xd1d, 0xfffff3ff},
	{0xd1e, 0xfffff7ff},
	{0xd54, 0xfffff800},
	{0xd55, 0xfffff801},
	{0xd56, 0xfffff803},
	{0xd57, 0xfffff807},
	{0xd58, 0xfffff80f},
	{0xd59, 0xfffff81f},
	{0xd5a, 0xfffff83f},
	{0xd5b, 0xfffff87f},
	{0xd5c, 0xfffff8ff},
	{0xd5d, 0xfffff9ff},
	{0xd5e, 0xfffffbff},
	{0xd95, 0xfffffc00},
	{0xd96, 0xfffffc01},
	{0xd97, 0xfffffc03},
	{0xd98, 0xfffffc07},
	{0xd99, 0xfffffc0f},
	{0xd9a, 0xfffffc1f},
	{0xd9b, 0xfffffc3f},
	{0xd9c, 0xfffffc7f},
	{0xd9d, 0xfffffcff},
	{0xd9e, 0xfffffdff},
	{0xdd6, 0xfffffe00},
	{0xdd7, 0xfffffe01},
	{0xdd8, 0xfffffe03},
	{0xdd9, 0xfffffe07},
	{0xdda, 0xfffffe0f},
	{0xddb, 0xfffffe1f},
	{0xddc, 0xfffffe3f},
	{0xddd, 0xfffffe7f},
	{0xdde, 0xfffffeff},
	{0xe17, 0xffffff00},
	{0xe18, 0xffffff01},
	{0xe19, 0xffffff03},
	{0xe1a, 0xffffff07},
	{0xe1b, 0xffffff0f},
	{0xe1c, 0xffffff1f},
	{0xe1d, 0xffffff3f},
	{0xe1e, 0xffffff7f},
	{0xe58, 0xffffff80},
	{0xe59, 0xffffff81},
	{0xe5a, 0xffffff83},
	{0xe5b, 0xffffff87},
	{0xe5c, 0xffffff8f},
	{0xe5d, 0xffffff9f},
	{0xe5e, 0xffffffbf},
	{0xe99, 0xffffffc0},
	{0xe9a, 0xffffffc1},
	{0xe9b, 0xffffffc3},
	{0xe9c, 0xffffffc7},
	{0xe9d, 0xffffffcf},
	{0xe9e, 0xffffffdf},
	{0xeda, 0xffffffe0},
	{0xedb, 0xffffffe1},
	{0xedc, 0xffffffe3},
	{0xedd, 0xffffffe7},
	{0xede, 0xffffffef},
	{0xf1b, 0xfffffff0},
	{0xf1c, 0xfffffff1},
	{0xf1d, 0xfffffff3},
	{0xf1e, 0xfffffff7},
	{0xf5c, 0xfffffff8},
	{0xf5d, 0xfffffff9},
	{0xf5e, 0xfffffffb},
	{0xf9d, 0xfffffffc},
	{0xf9e, 0xfffffffd},
	{0xfde, 0xfffffffe},
};

uint32_t host_arm64_find_imm(uint32_t data)
{
	int l = 0, r = IMM_NR - 1;

	while (l <= r)
	{
		int m = (l + r) >> 1;

		if (imm_table[m][1] < data)
			l = m+1;
		else if (imm_table[m][1] > data)
			r = m-1;
		else
			return imm_table[m][0];
	}
	return 0;
}