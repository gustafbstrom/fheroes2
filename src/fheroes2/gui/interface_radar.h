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

#ifndef H2INTERFACE_RADAR_H
#define H2INTERFACE_RADAR_H

#include "interface_border.h"
#include "ui_tool.h"

namespace Interface
{
    class Basic;

    class Radar : public BorderWindow
    {
    public:
        Radar( Basic & );

        void SetPos( s32, s32 );
        void SetRedraw( void ) const;
        void Build( void );
        void Redraw( void );
        void SetHide( bool );
        void ResetAreaSize( void );
        void QueueEventProcessing( void );

    private:
        void SavePosition( void );
        void Generate( void );
        void RedrawObjects( int color = 0xFF ) const;
        void RedrawCursor( void );
        void ChangeAreaSize( const Size & );

        Basic & interface;

        fheroes2::Image spriteArea;
        fheroes2::MovableSprite cursorArea;
        fheroes2::Point offset;
        bool hide;
    };
}

#endif
