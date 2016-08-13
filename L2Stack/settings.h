/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This file is part of L2Stack
*/
#ifndef tet_settings
#define tet_settings

void settings_Load(System* sys, const string& fn, lua_State* L);
void settings_Save(System* sys, const string& fn, lua_State* L);

#endif
