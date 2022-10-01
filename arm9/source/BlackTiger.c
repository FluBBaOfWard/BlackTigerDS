#include <nds.h>

#include "BlackTiger.h"
#include "Gfx.h"
#include "cpu.h"
#include "BlackTigerVideo/BlackTigerVideo.h"
#include "ARMZ80/ARMZ80.h"


int packState(void *statePtr) {
	int size = 0;
	size += blkTgrSaveState(statePtr+size, &blkTgrVideo_0);
	size += Z80SaveState(statePtr+size, &Z80OpTable);
	size += Z80SaveState(statePtr+size, &z80CPU1);
	return size;
}

void unpackState(const void *statePtr) {
	int size = 0;
	size += blkTgrLoadState(&blkTgrVideo_0, statePtr+size);
	size += Z80LoadState(&Z80OpTable, statePtr+size);
	size += Z80LoadState(&z80CPU1, statePtr+size);
	refreshGfx();
}

int getStateSize() {
	int size = 0;
	size += blkTgrGetStateSize();
	size += Z80GetStateSize();
	size += Z80GetStateSize();
	return size;
}

static const ArcadeRom blktigerRoms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"bdu-01a.5e", 0x08000, 0xa8f98f22},
	{"bdu-02a.6e", 0x10000, 0x7bef96e8},
	{"bdu-03a.8e", 0x10000, 0x4089e157},
	{"bd-04.9e",   0x10000, 0xed6af6ec},
	{"bd-05.10e",  0x10000, 0xae59b72e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd-06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x10000, "mcu", 0 )
//	{"bd.6k",      0x1000, 0xac7d14f1},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"bd-15.2n",   0x08000, 0x70175d78},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"bd-12.5b",   0x10000, 0xc4524993},
	{"bd-11.4b",   0x10000, 0x7932c86f},
	{"bd-14.9b",   0x10000, 0xdc49593a},
	{"bd-13.8b",   0x10000, 0x7ed7a122},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd-08.5a",   0x10000, 0xe2f17438},
	{"bd-07.4a",   0x10000, 0x5fccbd27},
	{"bd-10.9a",   0x10000, 0xfc33ccc6},
	{"bd-09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blktigeraRoms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"bdu-01.5e",  0x08000, 0x47b13922},
	{"bdu-02.6e",  0x10000, 0x2e0daf1b},
	{"bdu-03.8e",  0x10000, 0x3b67dfec},
	{"bd-04.9e",   0x10000, 0xed6af6ec},
	{"bd-05.10e",  0x10000, 0xae59b72e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd-06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x10000, "mcu", 0 )
//	{"bd.6k",      0x1000, 0xac7d14f1},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"bd-15.2n",   0x08000, 0x70175d78},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"bd-12.5b",   0x10000, 0xc4524993},
	{"bd-11.4b",   0x10000, 0x7932c86f},
	{"bd-14.9b",   0x10000, 0xdc49593a},
	{"bd-13.8b",   0x10000, 0x7ed7a122},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd-08.5a",   0x10000, 0xe2f17438},
	{"bd-07.4a",   0x10000, 0x5fccbd27},
	{"bd-10.9a",   0x10000, 0xfc33ccc6},
	{"bd-09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blktigerb1Roms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"btiger1.f6", 0x08000, 0x9d8464e8},
	{"bdu-02a.6e", 0x10000, 0x7bef96e8},
	{"btiger3.j6", 0x10000, 0x52c56ed1},
	{"bd-04.9e",   0x10000, 0xed6af6ec},
	{"bd-05.10e",  0x10000, 0xae59b72e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd-06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"bd-15.2n",   0x08000, 0x70175d78},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"bd-12.5b",   0x10000, 0xc4524993},
	{"bd-11.4b",   0x10000, 0x7932c86f},
	{"bd-14.9b",   0x10000, 0xdc49593a},
	{"bd-13.8b",   0x10000, 0x7ed7a122},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd-08.5a",   0x10000, 0xe2f17438},
	{"bd-07.4a",   0x10000, 0x5fccbd27},
	{"bd-10.9a",   0x10000, 0xfc33ccc6},
	{"bd-09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blktigerb2Roms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"1.bin",      0x08000, 0x47e2b21e},
	{"bdu-02a.6e", 0x10000, 0x7bef96e8},
	{"3.bin",      0x10000, 0x52c56ed1},
	{"bd-04.9e",   0x10000, 0xed6af6ec},
	{"bd-05.10e",  0x10000, 0xae59b72e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd-06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"bd-15.2n",   0x08000, 0x70175d78},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"bd-12.5b",   0x10000, 0xc4524993},
	{"bd-11.4b",   0x10000, 0x7932c86f},
	{"bd-14.9b",   0x10000, 0xdc49593a},
	{"bd-13.8b",   0x10000, 0x7ed7a122},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd-08.5a",   0x10000, 0xe2f17438},
	{"bd-07.4a",   0x10000, 0x5fccbd27},
	{"bd-10.9a",   0x10000, 0xfc33ccc6},
	{"bd-09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blkdrgonRoms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"bd_01.5e",   0x08000, 0x27ccdfbc},
	{"bd_02.6e",   0x10000, 0x7d39c26f},
	{"bd_03.8e",   0x10000, 0xd1bf3757},
	{"bd_04.9e",   0x10000, 0x4d1d6680},
	{"bd_05.10e",  0x10000, 0xc8d0c45e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd_06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x10000, "mcu", 0 )
//	{"bd.6k",      0x1000, 0xac7d14f1},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"bd_15.2n",   0x08000, 0x3821ab29},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"bd_12.5b",   0x10000, 0x22d0a4b0},
	{"bd_11.4b",   0x10000, 0xc8b5fc52},
	{"bd_14.9b",   0x10000, 0x9498c378},
	{"bd_13.8b",   0x10000, 0x5b0df8ce},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd_08.5a",   0x10000, 0xe2f17438},
	{"bd_07.4a",   0x10000, 0x5fccbd27},
	{"bd_10.9a",   0x10000, 0xfc33ccc6},
	{"bd_09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blkdrgonbRoms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images
	{"a1",         0x08000, 0x7caf2ba0},
	{"blkdrgon.6e",0x10000, 0x7d39c26f},
	{"a3",         0x10000, 0xf4cd0f39},
	{"blkdrgon.9e",0x10000, 0x4d1d6680},
	{"blkdrgon.10e",0x10000, 0xc8d0c45e},
	// ROM_REGION( 0x10000, "audiocpu", 0 )
	{"bd-06.1l",   0x8000, 0x2cf54274},
	// ROM_REGION( 0x08000, "gfx1", 0 )
	{"b5",         0x08000, 0x852ad2b7},
	// ROM_REGION( 0x40000, "gfx2", 0 )
	{"blkdrgon.5b",0x10000, 0x22d0a4b0},
	{"b1",         0x10000, 0x053ab15c},
	{"blkdrgon.9b",0x10000, 0x9498c378},
	{"b3",         0x10000, 0x9dc6e943},
	// ROM_REGION( 0x40000, "gfx3", 0 )
	{"bd-08.5a",   0x10000, 0xe2f17438},
	{"bd-07.4a",   0x10000, 0x5fccbd27},
	{"bd-10.9a",   0x10000, 0xfc33ccc6},
	{"bd-09.8a",   0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown)
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

static const ArcadeRom blktigerb3Roms[19] = {
	// ROM_REGION( 0x50000, "maincpu", 0 ) // 64k for code + banked ROMs images // == same as blktigerb2 maincpu
	{"1.5e",       0x08000, 0x47e2b21e},
	{"2.6e",       0x10000, 0x7bef96e8},
	{"3.8e",       0x10000, 0x52c56ed1},
	{"4.9e",       0x10000, 0xed6af6ec},
	{"5.10e",      0x10000, 0xae59b72e},
	// ROM_REGION( 0x10000, "audiocpu", 0 ) // == same as other sets but with an address swap
	{"6.1l",       0x8000, 0x6dfab115},
	// ROM_REGION( 0x08000, "gfx1", 0 )    // == same as blkdrgon
	{"15.2n",      0x08000, 0x3821ab29},
	// ROM_REGION( 0x40000, "gfx2", 0 )    // == same as other sets
	{"12.5b",      0x10000, 0xc4524993},
	{"11.4b",      0x10000, 0x7932c86f},
	{"14.9b",      0x10000, 0xdc49593a},
	{"13.8b",      0x10000, 0x7ed7a122},
	// ROM_REGION( 0x40000, "gfx3", 0 )    // == same as other sets
	{"8.5a",       0x10000, 0xe2f17438},
	{"7.4a",       0x10000, 0x5fccbd27},
	{"10.9a",      0x10000, 0xfc33ccc6},
	{"9.8a",       0x10000, 0xf449de01},
	// ROM_REGION( 0x0400, "proms", 0 )    // PROMs (function unknown), missing in this dump
	{"bd01.8j",    0x0100, 0x29b459e5},
	{"bd02.9j",    0x0100, 0x8b741e66},
	{"bd03.11k",   0x0100, 0x27201c75},
	{"bd04.11l",   0x0100, 0xe5490b68},
};

const ArcadeGame blktigerGames[GAME_COUNT] = {
	AC_GAME("blktiger", "Black Tiger", blktigerRoms)
	AC_GAME("blktigera", "Black Tiger (older)", blktigeraRoms)
	AC_GAME("blktigerb1", "Black Tiger (bootleg set 1)", blktigerb1Roms)
	AC_GAME("blktigerb2", "Black Tiger (bootleg set 2)", blktigerb2Roms)
	AC_GAME("blkdrgon", "Black Dragon (Japan)", blkdrgonRoms)
	AC_GAME("blkdrgonb", "Black Dragon (bootleg)", blkdrgonbRoms)
	AC_GAME("blktigerb3", "Black Tiger / Black Dragon (mixed bootleg?)", blktigerb3Roms)
};
