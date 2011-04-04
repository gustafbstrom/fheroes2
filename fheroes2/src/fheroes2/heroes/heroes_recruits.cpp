/*************************************************************************** 
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         * 
 *   This program is free software; you can redistribute it and/or modify  * 
 *   it under the terms of the GNU General Public License as published by  * 
 *   the Free Software Foundation; either version 2 of the License, or     * 
 *   (at your option) any later version.                                   * 
 *                                                                         * 
 *   This program is distributed in the hope that it will be useful,       * 
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 *   GNU General Public License for more details.                          * 
 *                                                                         * 
 *   You should have received a copy of the GNU General Public License     * 
 *   along with this program; if not, write to the                         * 
 *   Free Software Foundation, Inc.,                                       * 
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 ***************************************************************************/

#include "world.h"
#include "heroes_recruits.h"

Recruits::Recruits() : std::pair<Heroes::heroes_t, Heroes::heroes_t>(Heroes::UNKNOWN, Heroes::UNKNOWN)
{
}

void Recruits::Reset(void)
{
    first = Heroes::UNKNOWN;
    second = Heroes::UNKNOWN;
}

Heroes::heroes_t Recruits::GetID1(void) const
{
    return first;
}

Heroes::heroes_t Recruits::GetID2(void) const
{
    return second;
}

void Recruits::SetID1(u8 i)
{
    first = Heroes::UNKNOWN > i ? static_cast<Heroes::heroes_t>(i) : Heroes::UNKNOWN;
}

void Recruits::SetID2(u8 i)
{
    second = Heroes::UNKNOWN > i ? static_cast<Heroes::heroes_t>(i) : Heroes::UNKNOWN;
}

const Heroes* Recruits::GetHero1(void) const
{
    return world.GetHeroes(first);
}

const Heroes* Recruits::GetHero2(void) const
{
    return world.GetHeroes(second);
}

Heroes* Recruits::GetHero1(void)
{
    return world.GetHeroes(first);
}

Heroes* Recruits::GetHero2(void)
{
    return world.GetHeroes(second);
}

void Recruits::SetHero1(const Heroes* hero)
{
    first = hero ? hero->hid : Heroes::UNKNOWN;
}

void Recruits::SetHero2(const Heroes* hero)
{
    second = hero ? hero->hid : Heroes::UNKNOWN;
}

void Recruits::SetHero1(Heroes::heroes_t id)
{
    first = id;
}

void Recruits::SetHero2(Heroes::heroes_t id)
{
    second = id;
}
