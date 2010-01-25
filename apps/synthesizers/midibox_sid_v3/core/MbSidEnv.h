/* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
// $Id$
/*
 * MIDIbox SID Envelope Generator
 *
 * ==========================================================================
 *
 *  Copyright (C) 2010 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MB_SID_ENV_H
#define _MB_SID_ENV_H

#include <mios32.h>
#include "MbSidStructs.h"
#include "MbSidClock.h"


typedef enum {
    MBSID_ENV_STATE_IDLE = 0,
    MBSID_ENV_STATE_ATTACK1,
    MBSID_ENV_STATE_ATTACK2,
    MBSID_ENV_STATE_DECAY1,
    MBSID_ENV_STATE_DECAY2,
    MBSID_ENV_STATE_SUSTAIN,
    MBSID_ENV_STATE_RELEASE1,
    MBSID_ENV_STATE_RELEASE2
} mbsid_env_state_t;


class MbSidEnv
{
public:
    // Constructor
    MbSidEnv();

    // Destructor
    ~MbSidEnv();

    // ENV init function
    void init(sid_se_engine_t _engine, u8 _updateSpeedFactor, sid_se_env_patch_t *_envPatch, MbSidClock *_mbSidClockPtr);

    // ENV handler (returns true when sustain phase reached)
    bool tick(void);

    // requests a restart and release phase
    u8 restartReq;
    u8 releaseReq;

    // requests to use accented parameters
    u8 accentReq;

    // engine type
    sid_se_engine_t engine;

    // update speed factor
    u8     updateSpeedFactor;

    // cross-references
    sid_se_env_patch_t *envPatch; // ENV-Patch
    MbSidClock *mbSidClockPtr;    // reference to clock generator
    s16    *modSrcEnv; // reference to SID_SE_MOD_SRC_ENVx
    s32    *modDstPitch; // reference to SID_SE_MOD_DST_PITCHx
    s32    *modDstPw; // reference to SID_SE_MOD_DST_PWx
    s32    *modDstFilter; // reference to SID_SE_MOD_DST_FILx
    u8     *decayA; // reference to alternative decay value (used on ACCENTed notes)
    u8     *accent; // reference to accent value (used on ACCENTed notes)


protected:
    bool tickLead(void);
    bool step(u16 target, u8 rate, u8 curve);

    mbsid_env_state_t state;
    u16 ctr;
    u16 delayCtr;

};

#endif /* _MB_SID_ENV_H */