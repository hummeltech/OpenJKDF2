#include "jkRes.h"

#include "../jk.h"
#include "Win95/stdGob.h"
#include "Main/Main.h"
#include "General/stdFileUtil.h"
#include "General/stdFnames.h"
#include "stdPlatform.h"
#include "Win95/Windows.h"
#include "Engine/sith.h"
#include "General/util.h"

static int jkRes_bInit;

int jkRes_Startup(common_functions *a1)
{
    if ( jkRes_bInit )
        return 0;

    _memcpy(&lowLevelHS, a1, 0x70u);
    pLowLevelHS = (common_functions *)&lowLevelHS;
    _memset(&jkRes_gCtx, 0, sizeof(jkRes_gCtx));
    jkRes_pHS = a1;

    jkRes_HookHS();
    if ( Main_path[0] )
    {
        jkRes_New(Main_path);
    }
    jkRes_LoadNew(&jkRes_gCtx.gobs[3], "resource", 1);
    _memset(jkRes_aFiles, 0, sizeof(jkRes_aFiles));
    jkRes_bInit = 1;
    return 1;
}

void jkRes_New(char *path)
{
    for (int v1 = 0; v1 < jkRes_gCtx.gobs[0].numGobs; v1++)
    {
        stdGob_Free(jkRes_gCtx.gobs[0].gobs[v1]);
    }

    jkRes_gCtx.gobs[0].numGobs = 0;

    if ( *path )
        jkRes_LoadNew(jkRes_gCtx.gobs, path, 1);
}

int jkRes_LoadCd(char *a1)
{
    unsigned int v1; // esi
    stdGob **v2; // edi
    char v4[128]; // [esp+Ch] [ebp-80h] BYREF

    strncpy(jkRes_curDir, a1, 0x7Fu);
    v1 = 0;
    jkRes_curDir[127] = 0;
    if ( jkRes_gCtx.gobs[4].numGobs )
    {
        v2 = jkRes_gCtx.gobs[4].gobs;
        do
        {
            stdGob_Free(*v2);
            ++v1;
            ++v2;
        }
        while ( v1 < jkRes_gCtx.gobs[4].numGobs );
    }

    jkRes_gCtx.gobs[4].numGobs = 0;
    if ( *a1 )
    {
        __snprintf(v4, 0x80u, "%s\\gamedata\\resource", jkRes_curDir);
        return jkRes_LoadNew(&jkRes_gCtx.gobs[4], v4, Windows_installType != 9);
    }
    return 0;
}

void jkRes_HookHS()
{
    if (!jkRes_bHookedHS)
    {
        jkRes_pHS->fileOpen = jkRes_FileOpen;
        jkRes_pHS->fileClose = jkRes_FileClose;
        jkRes_pHS->fileRead = jkRes_FileRead;
        jkRes_pHS->fileGets = jkRes_FileGets;
        jkRes_pHS->fileGetws = jkRes_FileGetws;
        jkRes_pHS->fileWrite = jkRes_FileWrite;
        jkRes_pHS->feof = jkRes_FEof;
        jkRes_pHS->ftell = jkRes_FTell;
        jkRes_pHS->fseek = jkRes_FSeek;
        jkRes_pHS->fileSize = jkRes_FileSize;
        jkRes_pHS->filePrintf = jkRes_FilePrintf;
    }
    ++jkRes_bHookedHS;
}

void jkRes_UnhookHS()
{
    if (jkRes_bHookedHS)
    {
        jkRes_pHS->fileOpen = lowLevelHS.fileOpen;
        jkRes_pHS->fileClose = lowLevelHS.fileClose;
        jkRes_pHS->fileRead = lowLevelHS.fileRead;
        jkRes_pHS->fileGets = lowLevelHS.fileGets;
        jkRes_pHS->fileGetws = lowLevelHS.fileGetws;
        jkRes_pHS->fileWrite = lowLevelHS.fileWrite;
        jkRes_pHS->feof = lowLevelHS.feof;
        jkRes_pHS->ftell = lowLevelHS.ftell;
        jkRes_pHS->fseek = lowLevelHS.fseek;
        jkRes_pHS->fileSize = lowLevelHS.fileSize;
        jkRes_pHS->filePrintf = lowLevelHS.filePrintf;
        --jkRes_bHookedHS;
    }
}

int jkRes_FileExists(char *fpath, char *a2, int len)
{
    jkResFile *resFile;

    int fd = jkRes_FileOpen(fpath, "r");

    if (!fd)
        return 0;

    resFile = &jkRes_aFiles[fd-1];
    _strncpy(a2, resFile->fpath, len - 1);
    a2[len - 1] = 0;

    if ( resFile->useLowLevel )
        pLowLevelHS->fileClose(resFile->fsHandle);
    else
        stdGob_FileClose(resFile->gobHandle);

    resFile->bOpened = 0;
    return 1;
}

int jkRes_ReadKey()
{
    int keyval;

    int fd = pHS->fileOpen("jk_.cd", "rb");
    if (!fd)
        return 0;

    pHS->fileRead(fd, &keyval, 4);
    pHS->fileClose(fd);
    if (keyval == 0x69973284)
    {
        return 0;
    }
    else if (keyval == 0x699232C4)
    {
        return 1;
    }
    else if (keyval == 0x69923384)
    {
        return 2;
    }
    return 0;
}

int jkRes_LoadNew(jkResGob *resGob, char *name, int a3)
{
    int result; // eax
    stdFileSearch *v15; // ebp
    stdFileSearchResult v18; // [esp+8h] [ebp-10Ch] BYREF

    _strncpy(resGob->name, name, 0x7Fu);
    result = a3;
    resGob->name[127] = 0;
    resGob->numGobs = 0;
    if (!a3)
        return 0;

    jkRes_UnhookHS();

    v15 = stdFileUtil_NewFind(name, 3, "gob");
    while (stdFileUtil_FindNext(v15, &v18))
    {
        if ( resGob->numGobs >= 0x40u )
            break;
        if ( v18.fpath[0] != '.' )
        {
            __snprintf(jkRes_idkGobPath, 0x80u, "%s%c%s", name, LEC_PATH_SEPARATOR_CHR, v18.fpath);
            resGob->gobs[resGob->numGobs] = stdGob_Load(jkRes_idkGobPath, 16, 0);

            if ( resGob->gobs[resGob->numGobs] )
                resGob->numGobs++;
        }
    }

    stdFileUtil_DisposeFind(v15);

    jkRes_HookHS();
    
    return jkRes_bHookedHS;
}

int jkRes_NewGob(jkResGob *gobFullpath, char *gobFolder, char *gobFname)
{
    jkRes_UnhookHS();
    
    gobFullpath->numGobs = 0;
    __snprintf(jkRes_idkGobPath, 0x80u, "%s%c%s", gobFolder, LEC_PATH_SEPARATOR_CHR, gobFname);
    if ( util_FileExists(jkRes_idkGobPath) )
    {
        if ( gobFullpath->numGobs < 0x40u )
        {
            gobFullpath->gobs[gobFullpath->numGobs] = stdGob_Load(jkRes_idkGobPath, 16, 0);
            if ( gobFullpath->gobs[gobFullpath->numGobs] )
                gobFullpath->numGobs++;
        }
    }

    jkRes_HookHS();    
    
    return jkRes_bHookedHS;
}

int jkRes_LoadCD(int a1)
{
    int v1; // eax
    int v2; // esi
    unsigned int v3; // edi
    stdGob **v4; // esi
    unsigned int v5; // edi
    stdGob **v6; // esi
    wchar_t *v7; // eax
    wchar_t *v8; // eax
    wchar_t *v9; // eax
    unsigned int v10; // esi
    stdGob **v11; // edi
    unsigned int v12; // esi
    stdGob **v13; // edi
    unsigned int v14; // edi
    stdGob **v15; // esi
    unsigned int v16; // esi
    stdGob **v17; // edi
    unsigned int v18; // esi
    stdGob **v19; // edi
    unsigned int v20; // edi
    stdGob **v21; // esi
    int v23; // [esp+10h] [ebp-18Ch]
    int v24; // [esp+14h] [ebp-188h]
    int keyval; // [esp+18h] [ebp-184h] BYREF
    char v26[128]; // [esp+1Ch] [ebp-180h] BYREF
    char a2[128]; // [esp+9Ch] [ebp-100h] BYREF
    wchar_t v28[64]; // [esp+11Ch] [ebp-80h] BYREF

    v23 = 0;
    v24 = 0;
    while ( 1 )
    {
        v1 = pHS->fileOpen("jk_.cd", "rb");
        v2 = v1;
        if ( !v1 )
            goto LABEL_11;
        pHS->fileRead(v1, &keyval, 4);
        if ( keyval == 0x69973284 )
            goto LABEL_9;
        if ( !a1 )
        {
            if ( keyval != 0x699232C4 && keyval != 0x69923384 )
                goto LABEL_10;
LABEL_9:
            v23 = 1;
            goto LABEL_10;
        }
        if ( keyval == ((a1 << (a1 + 5)) | 0x69923284) )
            goto LABEL_9;
LABEL_10:
        pHS->fileClose(v2);
LABEL_11:
        if ( v23 )
        {
            if ( v24 )
            {
                strncpy(jkRes_curDir, jkRes_curDir, 0x7Fu);
                v10 = 0;
                jkRes_curDir[127] = 0;
                if ( jkRes_gCtx.gobs[4].numGobs )
                {
                    v11 = jkRes_gCtx.gobs[4].gobs;
                    do
                    {
                        stdGob_Free(*v11);
                        ++v10;
                        ++v11;
                    }
                    while ( v10 < jkRes_gCtx.gobs[4].numGobs );
                }
                jkRes_gCtx.gobs[4].numGobs = 0;
                if ( jkRes_curDir[0] )
                {
                    __snprintf(a2, 0x80u, "%s%cgamedata%cresource", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR);
                    jkRes_LoadNew(&jkRes_gCtx.gobs[4], a2, Windows_installType != 9);
                }
                sith_set_some_text_jk1(jkRes_episodeGobName);
                v12 = 0;
                if ( jkRes_gCtx.gobs[1].numGobs )
                {
                    v13 = jkRes_gCtx.gobs[1].gobs;
                    do
                    {
                        stdGob_Free(*v13);
                        ++v12;
                        ++v13;
                    }
                    while ( v12 < jkRes_gCtx.gobs[1].numGobs );
                }
                v14 = 0;
                jkRes_gCtx.gobs[1].numGobs = 0;
                if ( jkRes_gCtx.gobs[2].numGobs )
                {
                    v15 = jkRes_gCtx.gobs[2].gobs;
                    do
                    {
                        stdGob_Free(*v15);
                        ++v14;
                        ++v15;
                    }
                    while ( v14 < jkRes_gCtx.gobs[2].numGobs );
                }
                jkRes_gCtx.gobs[2].numGobs = 0;
                strncpy(jkRes_episodeGobName, jkRes_episodeGobName, 0x1Fu);
                jkRes_episodeGobName[31] = 0;
                if ( jkRes_episodeGobName[0] )
                {
                    __snprintf(v26, 0x80u, "%s.gob", jkRes_episodeGobName);
                    __snprintf(jkRes_gCtx.gobs[1].name, 0x80u, "episode%c%s", LEC_PATH_SEPARATOR_CHR, jkRes_episodeGobName);
                    jkRes_NewGob(&jkRes_gCtx.gobs[1], "episode", v26);
                    if ( jkRes_curDir[0] )
                    {
                        if ( Windows_installType < 1 )
                        {
                            __snprintf(std_genBuffer, 0x80u, "%s%cgamedata%cepisode", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR);
                            __snprintf(jkRes_gCtx.gobs[2].name, 0x80u, "%s%cgamedata%cepisode%c%s", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR, jkRes_episodeGobName);
                            jkRes_NewGob(&jkRes_gCtx.gobs[2], std_genBuffer, v26);
                        }
                    }
                }
            }
            goto LABEL_39;
        }
        v3 = 0;
        if ( jkRes_gCtx.gobs[2].numGobs )
        {
            v4 = jkRes_gCtx.gobs[2].gobs;
            do
            {
                stdGob_Free(*v4);
                ++v3;
                ++v4;
            }
            while ( v3 < jkRes_gCtx.gobs[2].numGobs );
        }
        v5 = 0;
        jkRes_gCtx.gobs[2].numGobs = 0;
        if ( jkRes_gCtx.gobs[4].numGobs )
        {
            v6 = jkRes_gCtx.gobs[4].gobs;
            do
            {
                stdGob_Free(*v6);
                ++v5;
                ++v6;
            }
            while ( v5 < jkRes_gCtx.gobs[4].numGobs );
        }
        jkRes_gCtx.gobs[4].numGobs = 0;
        v24 = 1;
        if ( a1 )
        {
            v8 = jkStrings_GetText("GUI_INSERTCD");
            jk_snwprintf(v28, 0x40u, v8, a1);
        }
        else
        {
            v7 = jkStrings_GetText("GUI_INSERTANYCD");
            jk_snwprintf(v28, 0x40u, v7);
        }
        v9 = jkStrings_GetText("GUI_INSERTCDTITLE");
        if ( !jkGuiDialog_OkCancelDialog(v9, v28) )
            break;
LABEL_39:
        if ( v23 )
            return 1;
    }
    strncpy(jkRes_curDir, jkRes_curDir, 0x7Fu);
    v16 = 0;
    jkRes_curDir[127] = 0;
    if ( jkRes_gCtx.gobs[4].numGobs )
    {
        v17 = jkRes_gCtx.gobs[4].gobs;
        do
        {
            stdGob_Free(*v17);
            ++v16;
            ++v17;
        }
        while ( v16 < jkRes_gCtx.gobs[4].numGobs );
    }
    jkRes_gCtx.gobs[4].numGobs = 0;
    if ( jkRes_curDir[0] )
    {
        __snprintf(a2, 0x80u, "%s%cgamedata%cresource", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR);
        jkRes_LoadNew(&jkRes_gCtx.gobs[4], a2, Windows_installType != 9);
    }
    sith_set_some_text_jk1(jkRes_episodeGobName);
    v18 = 0;
    if ( jkRes_gCtx.gobs[1].numGobs )
    {
        v19 = jkRes_gCtx.gobs[1].gobs;
        do
        {
            stdGob_Free(*v19);
            ++v18;
            ++v19;
        }
        while ( v18 < jkRes_gCtx.gobs[1].numGobs );
    }
    v20 = 0;
    jkRes_gCtx.gobs[1].numGobs = 0;
    if ( jkRes_gCtx.gobs[2].numGobs )
    {
        v21 = jkRes_gCtx.gobs[2].gobs;
        do
        {
            stdGob_Free(*v21);
            ++v20;
            ++v21;
        }
        while ( v20 < jkRes_gCtx.gobs[2].numGobs );
    }
    jkRes_gCtx.gobs[2].numGobs = 0;
    strncpy(jkRes_episodeGobName, jkRes_episodeGobName, 0x1Fu);
    jkRes_episodeGobName[31] = 0;
    if ( jkRes_episodeGobName[0] )
    {
        __snprintf(v26, 0x80u, "%s.gob", jkRes_episodeGobName);
        __snprintf(jkRes_gCtx.gobs[1].name, 0x80u, "episode%c%s", LEC_PATH_SEPARATOR_CHR, jkRes_episodeGobName);
        jkRes_NewGob(&jkRes_gCtx.gobs[1], "episode", v26);
        if ( jkRes_curDir[0] )
        {
            if ( Windows_installType < 1 )
            {
                __snprintf(std_genBuffer, 0x80u, "%s%cgamedata%cepisode", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR);
                __snprintf(jkRes_gCtx.gobs[2].name, 0x80u, "%s%cgamedata%cepisode%c%s", jkRes_curDir, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR, LEC_PATH_SEPARATOR_CHR, jkRes_episodeGobName);
                jkRes_NewGob(&jkRes_gCtx.gobs[2], std_genBuffer, v26);
            }
        }
    }
    return 0;
}
