// SR:AU - Samus Returns amiibo unlocker - Unlock amiibo gated content in SR.
// Copyright (C) 2018 Valentijn "ev1l0rd" V.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#ifndef _STRUCTH_
#define _STRUCTH_

typedef struct {
    bool PAL;
    bool USA;
    bool JPN;
    int total_regions;
} Regions;

typedef struct {
    bool profile0;
    bool profile1;
    bool profile2;
    int total_saves;
} SavesList;

typedef struct {
    int difficulty;
    bool energy_tank;
    bool energy_filled;
    bool missile_tank;
    bool missile_filled;
    bool aeion_tank;
    bool aeion_filled;
    bool amiibo_unlocked;
} SaveStatus;

typedef enum {
    MAIN_SCREEN,
    VERSION_TO_EDIT,
    SELECT_SAVE,
    OPEN_SAVE,
    READ_SAVE,
    FUSION_OR_NOT,
    THE_WIZARD_IS_BUSY,
    SUCCESS,
    FAILURE
} States;

typedef enum {
    SD_CARD,
    GAME_CARD
} InstallType;

#endif