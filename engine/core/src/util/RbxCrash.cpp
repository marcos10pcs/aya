/*
 *  RbxCrash.cpp
 *  MacClient
 *
 *  Created by elebel on 7/20/10.
 *  Copyright 2010 Roblox. All rights reserved.
 *
 */

#include "aya/Debug.hpp"

void RBXCRASH()
{
    Aya::Debugable::doCrash();
}

void RBXCRASH(const char* message)
{
    Aya::Debugable::doCrash(message);
}
