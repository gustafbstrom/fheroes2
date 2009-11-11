/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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

#ifndef H2CLIENT_H
#define H2CLIENT_H

#include "gamedefs.h"
#include "player.h"

#ifdef WITH_NET

#include <vector>
#include "bitmodes.h"
#include "sdlnet.h"

enum status_t
{
    ST_CONNECT          = 0x0001,
    ST_ADMIN            = 0x0008,
    ST_SHUTDOWN         = 0x0010,
    ST_ALLOWPLAYERS     = 0x0020,
    ST_LOCALSERVER      = 0x0040,
    ST_TURN		= 0x0080,
};

class FH2Client : public Network::Socket, public BitModes, public Player
{
public:
    FH2Client();

    virtual ~FH2Client(){};

    bool IsConnected(void) const;

    bool Wait(Network::Message &, u16);
    bool Send(Network::Message &);
    bool Recv(Network::Message &);

    Network::Message packet;
};

#endif
#endif
