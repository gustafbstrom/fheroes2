/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2BATTLE_ONLY_H
#define H2BATTLE_ONLY_H

#include <memory>
#include "army.h"
#include "heroes_recruits.h"
#include "image.h"
#include "players.h"
#include "skill_bar.h"

class ArmyBar;
class PrimarySkillsBar;
class SecondarySkillsBar;
class ArtifactsBar;
class MoraleIndicator;
class LuckIndicator;

namespace Battle
{
    struct ControlInfo;

    class Only
    {
    public:
        Only();

        bool ChangeSettings( void );
        void RedrawBaseInfo( const Point & );
        void StartBattle( void );
        void UpdateHero1( const Point & );
        void UpdateHero2( const Point & );

        static Recruits GetHeroesFromStreamBuf( StreamBuf & );

        Heroes * hero1;
        Heroes * hero2;

        Player player1;
        Player player2;

        Army * army1;
        Army * army2;
        Army monsters;

    private:
        std::unique_ptr<MoraleIndicator> moraleIndicator1;
        std::unique_ptr<MoraleIndicator> moraleIndicator2;

        std::unique_ptr<LuckIndicator> luckIndicator1;
        std::unique_ptr<LuckIndicator> luckIndicator2;

        std::unique_ptr<PrimarySkillsBar> primskill_bar1;
        std::unique_ptr<PrimarySkillsBar> primskill_bar2;

        std::unique_ptr<SecondarySkillsBar> secskill_bar1;
        std::unique_ptr<SecondarySkillsBar> secskill_bar2;

        std::unique_ptr<ArmyBar> selectArmy1;
        std::unique_ptr<ArmyBar> selectArmy2;

        std::unique_ptr<ArtifactsBar> selectArtifacts1;
        std::unique_ptr<ArtifactsBar> selectArtifacts2;

        ControlInfo * cinfo2;

        Rect rtPortrait1;
        Rect rtPortrait2;
    };
}

StreamBase & operator<<( StreamBase &, const Battle::Only & );
StreamBase & operator>>( StreamBase &, Battle::Only & );

#endif
