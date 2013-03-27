/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <QtEndian>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPixmapCache>

#include "program.h"
#include "engine.h"

H2::File::File()
{
}

H2::File::File(const QString & fn) : QFile(fn)
{
}

qint16 H2::File::readLE16(void)
{
    qint16 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE16:" << "out of range";

    return res;
}

qint32 H2::File::readLE32(void)
{
    qint32 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, sizeof(res));
	res = qFromLittleEndian(res);
    }
    else
	qWarning() << "H2::File::readLE32:" << "out of range";

    return res;
}

qint8 H2::File::readByte(void)
{
    qint8 res = 0;

    if(pos() + sizeof(res) <= size())
    {
	read((char*) &res, 1);
    }
    else
	qWarning() << "H2::File::readByte:" << "out of range";

    return res;
}

QString H2::File::readString(size_t sz)
{
    return QString(readBlock(sz));
}

QByteArray H2::File::readBlock(size_t sz, int ps)
{
    QByteArray res;

    if(0 <= ps)
	seek(ps);

    if(pos() + sz <= size())
    {
	res.reserve(sz);
	res.resize(sz);

	read(res.data(), sz);
    }
    else
	qWarning() << "H2::File::readBlock:" << "out of range";

    return res;
}

H2::ICNSprite::ICNSprite(const mp2icn_t & icn, const char* buf, quint32 size, const QVector<QRgb> & pals)
    : QImage(icn.width, icn.height, QImage::Format_ARGB32)
{
    const quint8* ptr = (const quint8*) buf;
    const quint8* outOfRange = ptr + size;

    fill(qRgba(0, 0, 0, 0));

    if(0x20 == icn.type)
	DrawVariant2(ptr, outOfRange, pals);
    else
	DrawVariant1(ptr, outOfRange, pals);
}

void H2::ICNSprite::DrawVariant1(const quint8* ptr, const quint8* outOfRange, const QVector<QRgb> & pals)
{
    int col = 0;
    int posX = 0;
    int posY = 0;

    QRgb shadow = qRgba(0, 0, 0, 0x40);

    while(1)
    {
        // 0x00 - end line
        if(0 == *ptr)
        {
            posY++;
            posX = 0;
            ptr++;
        }
        else
        // 0x7F - count data
        if(0x80 > *ptr)
        {
            col = *ptr;
            ptr++;
            while(col-- && ptr < outOfRange)
            {
                setPixel(posX, posY, pals[*ptr]);
                posX++;
                ptr++;
            }
        }
        else
        // 0x80 - end data
        if(0x80 == *ptr)
        {
            break;
        }
        else
        // 0xBF - skip data
        if(0xC0 > *ptr)
        {
            posX += *ptr - 0x80;
            ptr++;
        }
        else
        // 0xC0 - shadow
        if(0xC0 == *ptr)
        {
            ptr++;
            col = *ptr % 4 ? *ptr % 4 : *(++ptr);
            while(col--){ setPixel(posX, posY, shadow); posX++; }
            ptr++;
        }
        else
        // 0xC1
        if(0xC1 == *ptr)
        {
            ptr++;
            col = *ptr;
            ptr++;
            while(col--){ setPixel(posY, posY, pals[*ptr]); posX++; }
            ptr++;
        }
        else
        {
            col = *ptr - 0xC0;
            ptr++;
            while(col--){ setPixel(posX, posY, pals[*ptr]); posX++; }
            ptr++;
        }

        if(ptr >= outOfRange)
        {
            qWarning() << "H2::ICNSprite:DrawVariant1:" << "parse out of range";
            break;
        }
    }
}

void H2::ICNSprite::DrawVariant2(const quint8* ptr, const quint8* outOfRange, const QVector<QRgb> & pals)
{
    int col = 0;
    int posX = 0;
    int posY = 0;

    while(1)
    {
        // 0x00 - end line
        if(0 == *ptr)
        {
            posY++;
            posX = 0;
            ptr++;
        }
        else
        // 0x7F - count data
        if(0x80 > *ptr)
        {
	    col = *ptr;
            while(col--) { setPixel(posX, posY, pals[1]); posX++; }
            ptr++;
        }
        else
        // 0x80 - end data
        if(0x80 == *ptr)
        {
            break;
        }
        else
        // other - skip data
        {
	    posX += *ptr - 0x80;
            ptr++;
        }

        if(ptr >= outOfRange)
        {
            qWarning() << "H2::ICNSprite:DrawVariant2:" << "parse out of range";
            break;
        }
    }
}

mp2icn_t::mp2icn_t(const char* buf)
{
    offsetX = qFromLittleEndian(*((quint16*) buf));
    offsetY = qFromLittleEndian(*((quint16*) (buf + 2)));
    width = qFromLittleEndian(*((quint16*) (buf + 4)));
    height = qFromLittleEndian(*((quint16*) (buf + 6)));
    type = *((quint8*) (buf + 8));
    offsetData = qFromLittleEndian(*((quint32*) (buf + 9)));
}

mp2lev_t::mp2lev_t()
{
    object = 0;
    index = 0;
    uniq = 0;
}

mp2til_t::mp2til_t()
{
    tileSprite = Rand(16, 19);
    quantity1 = 0;
    quantity2 = 0;
    tileShape = 0;
    tileObject = 0;
    indexExt = 0;
}

mp2til_t H2::File::readMP2Til(void)
{
    mp2til_t res;

    res.tileSprite = readLE16();
    res.level1.object = readByte();
    res.level1.index = readByte();
    res.quantity1 = readByte();
    res.quantity2 = readByte();
    res.level2.object = readByte();
    res.level2.index = readByte();
    res.tileShape = readByte();
    res.tileObject = readByte();
    res.indexExt = readLE16();
    res.level1.uniq = readLE32();
    res.level2.uniq = readLE32();

    return res;
}

mp2ext_t::mp2ext_t()
{
    indexExt = 0;
    quantity = 0;
}

mp2ext_t H2::File::readMP2Ext(void)
{
    mp2ext_t res;

    res.indexExt = readLE16();
    res.level1.object = 2 * readByte();
    res.level1.index = readByte();
    res.quantity = readByte();
    res.level2.object = readByte();
    res.level2.index = readByte();
    res.level1.uniq = readLE32();
    res.level2.uniq = readLE32();

    return res;
}

bool AGG::File::exists(const QString & str) const
{
    return items.end() != items.find(str);
}

QByteArray AGG::File::readRawData(const QString & name)
{
    QMap<QString, Item>::const_iterator it = items.find(name);

    if(items.end() != it)
	return readBlock((*it).size, (*it).offset);
    else
	qCritical() << "AGG::File::readRawData:" << "item" << qPrintable(name) << "not found";

    return NULL;
}

bool AGG::File::loadFile(const QString & fn)
{
    if(isOpen()) close();
    if(fn.isEmpty()) return false;

    setFileName(fn);
    if(open(QIODevice::ReadOnly))
    {
	qDebug() << "AGG::File::loadFile:" << qPrintable(fn);
	quint16 countItems = readLE16();

	qDebug() << "AGG::File::loadFile:" << "count items:" << countItems;
	const int sizeName = 15;

	for(int it = 0; it < countItems; ++it)
	{
	    int posname = size() - sizeName * (countItems - it);

	    Item & item = items[QString(readBlock(sizeName, posname))];

	    seek(sizeof(countItems) + it * (3 * sizeof(quint32) /* crcItem + offsetItem + sizeItem */)); 

	    item.crc = readLE32();
	    item.offset = readLE32();
	    item.size = readLE32();
	}

	return true;
    }
    else
	qCritical() << "AGG::File::loadFile:" << "Can not read file " << qPrintable(fn);

    return false;
}

QPixmap AGG::File::getImageTIL(const QString & id, quint16 index, QVector<QRgb> & colors)
{
    QString key = id + QString::number(index);
    QPixmap result = NULL;

    if(! QPixmapCache::find(key, & result) &&
	! items.isEmpty())
    {
	QByteArray buf = readRawData(id);

	if(buf.size())
	{
	    quint16 tileCount = qFromLittleEndian(*((quint16*) buf.data()));

	    if(index < tileCount)
	    {
		quint16 tileWidth = qFromLittleEndian(*((quint16*) (buf.data() + 2)));
		quint16 tileHeight = qFromLittleEndian(*((quint16*) (buf.data() + 4)));

		QImage image((uchar*) buf.data() + 6 + index * tileWidth * tileHeight, tileWidth, tileHeight, QImage::Format_Indexed8);
		image.setColorTable(colors);

		result = QPixmap::fromImage(image);
		QPixmapCache::insert(key, result);
	    }
	    else
		qCritical() << "AGG::File::getImageTIL:" << "out of range" << index;
	}
    }

    return result;
}

QPair<QPixmap, QPoint> AGG::File::getImageICN(const QString & id, quint16 index, QVector<QRgb> & colors)
{
    QString key = id + QString::number(index);
    QPixmap result;
    QPoint offset;

    QByteArray buf = readRawData(id);

    if(buf.size())
    {
	quint16 icnCount = qFromLittleEndian(*((quint16*) buf.data()));

	if(index < icnCount)
	{
	    mp2icn_t header(buf.data() + 6 + index * mp2icn_t::sizeOf());

	    if(! QPixmapCache::find(key, & result) &&
		! items.isEmpty())
	    {
		quint32 sizeData = 0;

		if(index + 1 < icnCount)
		{
		    mp2icn_t headerNext(buf.data() + 6 + (index + 1) * mp2icn_t::sizeOf());
		    sizeData = headerNext.offsetData - header.offsetData;
		}
		else
		    sizeData = qFromLittleEndian(*((quint32*) (buf.data() + 2))) - header.offsetData;

		H2::ICNSprite image(header, buf.data() + 6 + header.offsetData, sizeData, colors);

		result = QPixmap::fromImage(image);
		QPixmapCache::insert(key, result);
	    }

	    offset = QPoint(header.offsetX, header.offsetY);
	}
	else
	    qCritical() << "AGG::File::getImageICN:" << "out of range" << index;
    }

    return qMakePair<QPixmap, QPoint>(result, offset);
}

AGG::Spool::Spool(const QString & file)
{
    if(first.loadFile(file))
    {
	QStringList list = QFileInfo(file).absoluteDir().entryList(QStringList() << "heroes2x.agg", QDir::Files | QDir::Readable);

	if(list.size())
	    second.loadFile(QDir::toNativeSeparators(QFileInfo(file).absolutePath() + QDir::separator() + list.front()));

	// load palette
	QByteArray array = first.readRawData("KB.PAL");

	if(array.size())
	{
	    qint8 r, g, b;
	    const quint32 palSize = array.size() / 3;
	    colors.reserve(palSize);

	    for(quint32 num = 0; num < palSize; ++num)
	    {
		r = array[num * 3];
		g = array[num * 3 + 1];
		b = array[num * 3 + 2];

		colors.push_back(qRgb(r << 2, g << 2, b << 2));
	    }

	    qDebug() << "AGG::Spool:" << "loaded palette: " << colors.size();
	}
	else
	    qCritical() << "AGG::Spool:" << "palette not found";
    }
}

QPixmap AGG::Spool::getImageTIL(const QString & til, quint16 index)
{
    return second.isReadable() && second.exists(til) ?
	second.getImageTIL(til, index, colors) : first.getImageTIL(til, index, colors);
}

QPair<QPixmap, QPoint> AGG::Spool::getImageICN(const QString & icn, quint16 index)
{
    return second.isReadable() && second.exists(icn) ?
	second.getImageICN(icn, index, colors) : first.getImageICN(icn, index, colors);
}

quint32 Rand(quint32 min, quint32 max)
{
    if(min > max) qSwap(min, max);
    return min + Rand(max - min);
}

quint32 Rand(quint32 max)
{
    return static_cast<quint32>((max + 1) * (qrand() / (RAND_MAX + 1.0)));
}

QString H2::mapICN(int type)
{
    switch(type)
    {
	// artifact
	case 0x2C: case 0x2D: case 0x2E: case 0x2F: return "OBJNARTI.ICN";
	// monster
	case 0x30: case 0x31: case 0x32: case 0x33: return "MONS32.ICN";
	// castle flags
	case 0x38: case 0x39: case 0x3A: case 0x3B: return "FLAG32.ICN";
	// heroes
	case 0x54: case 0x55: case 0x56: case 0x57: return "MINIHERO.ICN";
	// relief: snow
	case 0x58: case 0x59: case 0x5A: case 0x5B: return "MTNSNOW.ICN";
	// relief: swamp
	case 0x5C: case 0x5D: case 0x5E: case 0x5F: return "MTNSWMP.ICN";
	// relief: lava
	case 0x60: case 0x61: case 0x62: case 0x63: return "MTNLAVA.ICN";
	// relief: desert
	case 0x64: case 0x65: case 0x66: case 0x67: return "MTNDSRT.ICN";
	// relief: dirt
	case 0x68: case 0x69: case 0x6A: case 0x6B: return "MTNDIRT.ICN";
	// relief: others
	case 0x6C: case 0x6D: case 0x6E: case 0x6F: return "MTNMULT.ICN";
	// mines
	case 0x74: return "EXTRAOVR.ICN";
	// road
	case 0x78: case 0x79: case 0x7A: case 0x7B: return "ROAD.ICN";
	// relief: crck
	case 0x7C: case 0x7D: case 0x7E: case 0x7F: return "MTNCRCK.ICN";
	// relief: gras
	case 0x80: case 0x81: case 0x82: case 0x83: return "MTNGRAS.ICN";
	// trees jungle
	case 0x84: case 0x85: case 0x86: case 0x87: return "TREJNGL.ICN";
	// trees evil
	case 0x88: case 0x89: case 0x8A: case 0x8B: return "TREEVIL.ICN";
	// castle and tower
	case 0x8C: case 0x8D: case 0x8E: case 0x8F: return "OBJNTOWN.ICN";
	// castle lands
	case 0x90: case 0x91: case 0x92: case 0x93: return "OBJNTWBA.ICN";
	// castle shadow
	case 0x94: case 0x95: case 0x96: case 0x97: return "OBJNTWSH.ICN";
	// random castle
	case 0x98: case 0x99: case 0x9A: case 0x9B: return "OBJNTWRD.ICN";
	// water object
	case 0xA0: case 0xA1: case 0xA2: case 0xA3: return "OBJNWAT2.ICN";
	// object other
	case 0xA4: case 0xA5: case 0xA6: case 0xA7: return "OBJNMUL2.ICN";
	// trees snow
	case 0xA8: case 0xA9: case 0xAA: case 0xAB: return "TRESNOW.ICN";
	// trees trefir
	case 0xAC: case 0xAD: case 0xAE: case 0xAF: return "TREFIR.ICN";
	// trees
	case 0xB0: case 0xB1: case 0xB2: case 0xB3: return "TREFALL.ICN";
	// river
	case 0xB4: case 0xB5: case 0xB6: case 0xB7: return "STREAM.ICN";
	// resource
	case 0xB8: case 0xB9: case 0xBA: case 0xBB: return "OBJNRSRC.ICN";
	// gras object
	case 0xC0: case 0xC1: case 0xC2: case 0xC3: return "OBJNGRA2.ICN";
	// trees tredeci
	case 0xC4: case 0xC5: case 0xC6: case 0xC7: return "TREDECI.ICN";
	// sea object
	case 0xC8: case 0xC9: case 0xCA: case 0xCB: return "OBJNWATR.ICN";
        // vegetation gras                                            
        case 0xCC: case 0xCD: case 0xCE: case 0xCF: return "OBJNGRAS.ICN";
	// object on snow                                             
	case 0xD0: case 0xD1: case 0xD2: case 0xD3: return "OBJNSNOW.ICN";
        // object on swamp                                            
        case 0xD4: case 0xD5: case 0xD6: case 0xD7: return "OBJNSWMP.ICN";
	// object on lava                                             
	case 0xD8: case 0xD9: case 0xDA: case 0xDB: return "OBJNLAVA.ICN";
        // object on desert                                           
        case 0xDC: case 0xDD: case 0xDE: case 0xDF: return "OBJNDSRT.ICN";
        // object on dirt                                             
	case 0xE0: case 0xE1: case 0xE2: case 0xE3: return "OBJNDIRT.ICN";
	// object on crck
	case 0xE4: case 0xE5: case 0xE6: case 0xE7: return "OBJNCRCK.ICN";
	// object on lava
	case 0xE8: case 0xE9: case 0xEA: case 0xEB: return "OBJNLAV3.ICN";
	// object on earth
	case 0xEC: case 0xED: case 0xEE: case 0xEF: return "OBJNMULT.ICN";
	//  object on lava                                            
	case 0xF0: case 0xF1: case 0xF2: case 0xF3: return "OBJNLAV2.ICN";
	// extra objects for loyalty version
	case 0xF4: case 0xF5: case 0xF6: case 0xF7: return "X_LOC1.ICN";
	// extra objects for loyalty version
	case 0xF8: case 0xF9: case 0xFA: case 0xFB: return "X_LOC2.ICN";
	// extra objects for loyalty version
	case 0xFC: case 0xFD: case 0xFE: case 0xFF: return "X_LOC3.ICN";
	// unknown
	default: qWarning() << "H2::mapICN: return NULL, object:" << type; break;
    }

    return NULL;
}

int H2::isAnimationICN(const mp2lev_t & ext, int ticket)
{
    switch(ext.object)
    {
	// object on lava, OBJNLAVA
	case 0xD8: case 0xD9: case 0xDA: case 0xDB:
	    switch(ext.index)
	    {
		// shadow of lava
		case 0x4F: case 0x58: case 0x62:
		    return ext.index + (ticket % 9) + 1;
		default: break;
	    }
	    break;

	//  object on lava, OBJNLAV2
	case 0xF0: case 0xF1: case 0xF2: case 0xF3:
	    switch(ext.index)
	    {
		// middle volcano
		case 0x00:
		// shadow
		case 0x07: case 0x0E:
		// lava
		case 0x15:
		    return ext.index + (ticket % 6) + 1;
		// small volcano
		// shadow
		case 0x21: case 0x2C:
		// lava
		case 0x37: case 0x43:
		    return ext.index + (ticket % 10) + 1;
		default: break;
	    }
	    break;

	// object on lava, OBJNLAV3
	case 0xE8: case 0xE9: case 0xEA: case 0xEB:
	    // big volcano
	    switch(ext.index)
	    {
		// smoke
		case 0x00: case 0x0F: case 0x1E: case 0x2D: case 0x3C: case 0x4B: case 0x5A: case 0x69: case 0x87: case 0x96: case 0xA5:
		// shadow
		case 0x78: case 0xB4: case 0xC3: case 0xD2: case 0xE1:
		    return ext.index + (ticket % 14) + 1;
		default: break;
	    }
	    break;

	// object other, OBJNMUL2
	case 0xA4: case 0xA5: case 0xA6: case 0xA7:
	    switch(ext.index)
	    {
		// lighthouse
		case 0x3D:
		    return ext.index + (ticket % 9) + 1;
		// alchemytower
		case 0x1B:
		// watermill
		case 0x53: case 0x5A: case 0x62: case 0x69:
		// fire in wagoncamp
		case 0x81:
		// smoke smithy (2 chimney)
		case 0xA6:
		// smoke smithy (1 chimney)
		case 0xAD:
		// shadow smoke
		case 0xB4:
		    return ext.index + (ticket % 6) + 1;
		// magic garden
		case 0xBE:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	// object on snow, OBJNSNOW
	case 0xD0: case 0xD1: case 0xD2: case 0xD3:
	    switch(ext.index)
	    {
		// firecamp
		case 0x04:
		// alchemytower
		case 0x97:
		// watermill
		case 0xA2: case 0xA9: case 0xB1: case 0xB8:
		    return ext.index + (ticket % 6) + 1;
		// mill
		case 0x60: case 0x64: case 0x68: case 0x6C: case 0x70: case 0x74: case 0x78: case 0x7C: case 0x80: case 0x84:
		    return ext.index + (ticket % 3) + 1;
		default: break;
	    }
	    break;

	// object on swamp, OBJNSWMP
	case 0xD4: case 0xD5: case 0xD6: case 0xD7:
	    switch(ext.index)
	    {
		// shadow
		case 0x00: case 0x0E: case 0x2B:
		// smoke
		case 0x07: case 0x22: case 0x33:
		// light in window
		case 0x16: case 0x3A: case 0x43: case 0x4A:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	// object on desert, OBJNDSRT
	case 0xDC: case 0xDD: case 0xDE: case 0xDF:
	    switch(ext.index)
	    {
		// campfire
		case 0x36: case 0x3D:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	// gras object, OBJNGRA2
	case 0xC0: case 0xC1: case 0xC2: case 0xC3:
	    switch(ext.index)
	    {
		// mill
		case 0x17: case 0x1B: case 0x1F: case 0x23: case 0x27: case 0x2B: case 0x2F: case 0x33: case 0x37: case 0x3B:
		    return ext.index + (ticket % 3) + 1;
		// smoke from chimney
		case 0x3F: case 0x46: case 0x4D:
		// archerhouse
		case 0x54:
		// smoke from chimney
		case 0x5D: case 0x64:
		// shadow smoke
		case 0x6B:
		// peasanthunt
		case 0x72:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;
	// object on crck, OBJNCRCK
	case 0xE4: case 0xE5: case 0xE6: case 0xE7:
	    switch(ext.index)
	    {
		// pool of oil
		case 0x50: case 0x5B: case 0x66: case 0x71: case 0x7C: case 0x89: case 0x94: case 0x9F: case 0xAA:
		// smoke from chimney
		case 0xBE:
		// shadow smoke
		case 0xCA:
		    return ext.index + (ticket % 10) + 1;
		default: break;
	    }
	    break;

	// object on dirt, OBJNDIRT
	case 0xE0: case 0xE1: case 0xE2: case 0xE3:
	    switch(ext.index)
	    {
		// mill
		case 0x99: case 0x9D: case 0xA1: case 0xA5: case 0xA9: case 0xAD: case 0xB1: case 0xB5: case 0xB9: case 0xBD:
		    return ext.index + (ticket % 3) + 1;
		default: break;
	    }
	    break;

	// object on earth, OBJNMULT
	case 0xEC: case 0xED: case 0xEE: case 0xEF:
	    switch(ext.index)
	    {
		// smoke
		case 0x05:
		// shadow
		case 0x0F: case 0x19:
		    return ext.index + (ticket % 9) + 1;
		// smoke
		case 0x24:
		// shadow
		case 0x2D:
		    return ext.index + (ticket % 8) + 1;
		// smoke
		case 0x5A:
		// shadow
		case 0x61: case 0x68: case 0x7C:
		// campfire
		case 0x83:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	// sea object, OBJNWATR
	case 0xC8: case 0xC9: case 0xCA: case 0xCB:
	    switch(ext.index)
	    {
		// buttle
		case 0x00:
		    return ext.index + (ticket % 11) + 1;
		// shadow
		case 0x0C:
		// chest
		case 0x13:
		// shadow
		case 0x26:
		// flotsam
		case 0x2D:
		// unkn
		case 0x37:
		// boat
		case 0x3E:
		// waves
		case 0x45:
		// seaweed
		case 0x4C: case 0x53: case 0x5A: case 0x61: case 0x68:
		// sailor-man
		case 0x6F:
		// shadow
		case 0xBC:
		// buoy
		case 0xC3:
		// broken ship (right)
		case 0xE2: case 0xE9: case 0xF1: case 0xF8:
		    return ext.index + (ticket % 6) + 1;
		// seagull on stones
		case 0x76: case 0x86: case 0x96:
		    return ext.index + (ticket % 15) + 1;
		// whirlpool
		case 0xCA: case 0xCE: case 0xD2: case 0xD6: case 0xDA: case 0xDE:
		    return ext.index + (ticket % 3) + 1;
		default: break;

	    }
	    break;

	// water object, OBJNWAT2
	case 0xA0: case 0xA1: case 0xA2: case 0xA3:
	    switch(ext.index)
	    {
		// sail broken ship (left)
		case 0x03: case 0x0C:
		    return ext.index + (ticket % 6) + 1;
		default: break;
	    }
	    break;

	// extra objects, X_LOC1
	case 0xF4: case 0xF5: case 0xF6: case 0xF7:
	    switch(ext.index)
	    {
		// alchemist tower
		case 0x04: case 0x0D: case 0x16:
		// arena
		case 0x1F: case 0x28: case 0x32: case 0x3B:
		// earth altar
		case 0x55: case 0x5E: case 0x67:
		    return ext.index + (ticket % 8) + 1;
		default: break;
	    }
	    break;

	// extra objects, X_LOC2
	case 0xF8: case 0xF9: case 0xFA: case 0xFB:
	    switch(ext.index)
	    {
		// mermaid
		case 0x0A: case 0x13: case 0x1C: case 0x25: 
		// sirens
		case 0x2F: case 0x38: case 0x41: case 0x4A: case 0x53: case 0x5C: case 0x66:
		    return ext.index + (ticket % 8) + 1;
		default: break;
	    }
	    break;

	// extra objects, X_LOC3
	case 0xFC: case 0xFD: case 0xFE: case 0xFF:
	    switch(ext.index)
	    {
		// hut magi
		case 0x00: case 0x0A: case 0x14:
		// eye magi
		case 0x20: case 0x29: case 0x32:
		    return ext.index + (ticket % 8) + 1;
		// barrier
		case 0x3C: case 0x42: case 0x48: case 0x4E: case 0x54: case 0x5A: case 0x60: case 0x66:
		    return ext.index + (ticket % 4) + 1;
		default: break;
	    }

	default: break;
    }

    return 0;
}