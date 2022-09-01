#ifndef _SITHPARTICLE_H
#define _SITHPARTICLE_H

#include "types.h"

#define sithParticle_Startup_ADDR (0x004F18B0)
#define sithParticle_Shutdown_ADDR (0x004F18F0)
#define sithParticle_LoadEntry_ADDR (0x004F1910)
#define sithParticle_New_ADDR (0x004F1A00)
#define sithParticle_LoadThingParams_ADDR (0x004F1A60)
#define sithParticle_Tick_ADDR (0x004F1C30)
#define sithParticle_CreateThing_ADDR (0x004F1DA0)
#define sithParticle_Remove_ADDR (0x004F2010)
#define sithParticle_FreeEntry_ADDR (0x004F2080)
#define sithParticle_Free_ADDR (0x004F20B0)

enum SithParticleFlag
{
    SITHPARTICLE_FLAG_OUTWARD_EXPANDING = 0x1,
    SITHPARTICLE_FLAG_ANIMATE_CEL = 0x2,
    SITHPARTICLE_FLAG_RANDOM_START_CEL = 0x4,
    SITHPARTICLE_FLAG_FADE_OUT_OVER_TIME = 0x8,
    SITHPARTICLE_FLAG_EMIT_LIGHT = 0x10,
    SITHPARTICLE_FLAG_RANDOM_CEL_CHANGE = 0x20,
    SITHPARTICLE_UNKNOWN_40 = 0x40,
};

int sithParticle_Startup();
void sithParticle_Shutdown();
rdParticle* sithParticle_LoadEntry(const char *a1);
int sithParticle_New(sithWorld *world, int numParticles);
int sithParticle_LoadThingParams(stdConffileArg *arg, sithThing *thing, int param);
void sithParticle_Tick(sithThing *particle, float deltaMs);
void sithParticle_CreateThing(sithThing *thing);
void sithParticle_Remove(sithThing *particle);
void sithParticle_FreeEntry(sithThing *thing);
void sithParticle_Free(sithWorld *world);

#endif // _SITHPARTICLE_H
