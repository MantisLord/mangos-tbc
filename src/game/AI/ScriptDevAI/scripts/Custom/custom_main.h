typedef struct maxStyles_struct {
    uint8 maxMale;
    uint8 maxFemale;
} maxStyles_t;

maxStyles_t maxHairStyles[MAX_RACES] =
{
    {0,0},
    {11,18},    // RACE_HUMAN           = 1,
    {6,6},      // RACE_ORC             = 2,
    {10,13},    // RACE_DWARF           = 3,
    {6,6},      // RACE_NIGHTELF        = 4,
    {10,9},     // RACE_UNDEAD_PLAYER   = 5,
    {7,6},      // RACE_TAUREN          = 6,
    {6,6},      // RACE_GNOME           = 7,
    {5,4},      // RACE_TROLL           = 8,
    {0,0},      // RACE_GOBLIN          = 9,
    {9,13},     // RACE_BLOODELF        = 10,
    {7,10},     // RACE_DRAENEI         = 11
};

uint8 maxHairColor[MAX_RACES] =
{
    0,
    9,          // RACE_HUMAN           = 1,
    7,          // RACE_ORC             = 2,
    9,          // RACE_DWARF           = 3,
    7,          // RACE_NIGHTELF        = 4,
    9,          // RACE_UNDEAD_PLAYER   = 5,
    2,          // RACE_TAUREN          = 6,
    8,          // RACE_GNOME           = 7,
    9,          // RACE_TROLL           = 8,
    0,          // RACE_GOBLIN          = 9,
    9,          // RACE_BLOODELF        = 10,
    6,          // RACE_DRAENEI         = 11
};

maxStyles_t maxFacialFeatures[MAX_RACES] =
{
    {0,0},
    {8,6},      // RACE_HUMAN           = 1,
    {10,6},     // RACE_ORC             = 2,
    {10,5},     // RACE_DWARF           = 3,
    {5,9},      // RACE_NIGHTELF        = 4,
    {16,7},     // RACE_UNDEAD_PLAYER   = 5,
    {6,4},      // RACE_TAUREN          = 6,
    {7,6},      // RACE_GNOME           = 7,
    {10,5},     // RACE_TROLL           = 8,
    {0,0},      // RACE_GOBLIN          = 9,
    {10,9},     // RACE_BLOODELF        = 10,
    {7,6},      // RACE_DRAENEI         = 11
};

maxStyles_t maxSkins[MAX_RACES] =
{
    {0,0},
    {11,11},    // RACE_HUMAN           = 1,
    {99,99},    // RACE_ORC             = 2,
    {18,10},    // RACE_DWARF           = 3,
    {99,99},    // RACE_NIGHTELF        = 4,
    {99,99},    // RACE_UNDEAD_PLAYER   = 5,
    {99,99},    // RACE_TAUREN          = 6,
    {99,99},    // RACE_GNOME           = 7,
    {99,99},    // RACE_TROLL           = 8,
    {99,99},    // RACE_GOBLIN          = 9,
    {15,15},    // RACE_BLOODELF        = 10,
    {13,11},    // RACE_DRAENEI         = 11
};

maxStyles_t maxFaces[MAX_RACES] =
{
    {0,0},
    {99,99},    // RACE_HUMAN           = 1,
    {99,99},    // RACE_ORC             = 2,
    {99,99},    // RACE_DWARF           = 3,
    {99,99},    // RACE_NIGHTELF        = 4,
    {99,99},    // RACE_UNDEAD_PLAYER   = 5,
    {99,99},    // RACE_TAUREN          = 6,
    {99,99},    // RACE_GNOME           = 7,
    {99,99},    // RACE_TROLL           = 8,
    {99,99},    // RACE_GOBLIN          = 9,
    {99,99},    // RACE_BLOODELF        = 10,
    {9,99},     // RACE_DRAENEI         = 11
};
