/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2021 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#ifndef PARTITION_CORE_SIZEUTILS_H
#define PARTITION_CORE_SIZEUTILS_H

#include <kpmcore/util/capacity.h>

/** @brief Helper function for printing sizes consistently.
 *
 * Most of Calamares uses a qint64 for partition sizes, so use that
 * parameter type. However, the human-visible formatting doesn't need
 * to bother with one-byte accuracy (and anyway, a double has at least 50 bits
 * at which point we're printing giga (or gibi) bytes).
 */
static inline QString
formatByteSize( qint64 sizeValue )
{
    return Capacity::formatByteSize( static_cast< double >( sizeValue ) );
}

/** @brief Adjusts @p start_sector to a 4K boundary
 *
 * Given a @p logicalSize of each sector, returns a sector-number to use (instead of)
 * @p start_sector such that that sector is 4K-aligned. If @p logicalSize is not
 * 512 (traditional block size) the device is assumed to be special in some way
 * and no adjustment is done.
 */
inline quint64
alignStartSectorTo4K( const qint64 logicalSize, const quint64 startSector )
{
    if ( logicalSize != 512 )
    {
        // RAID or non standard setup or already aligned
        return startSector;
    }

    // if logicalSize == 512 we round sectors number to value that align to 4K
    // for 512 sector size, sectors number must be divisible by 8
    quint64 const rem = ( startSector - 1 ) % 8;
    return ( startSector - rem + 7 );
}

/** @bief Adjusts @p end_sector to a 4K boundary
 *
 * Given a @p logicalSize of each sector, returns a sector-number to use (instead of)
 * @p end_sector such that that sector is 4K-aligned. If @p logicalSize is not
 * 512 (traditional block size) the device is assumed to be special in some way
 * and no adjustment is done.
 */
inline quint64
alignEndSectorTo4K( const qint64 logicalSize, const quint64 endSector )
{
    if ( logicalSize != 512 )
    {
        // RAID or non standard setup or already aligned
        return endSector;
    }

    // if logicalSize == 512 we round sectors number to value that align to 4K
    // for 512 sector size, sectors number must be divisible by 8
    quint64 const rem = ( endSector + 1 ) % 8;
    return ( endSector - rem );
}

#endif  // PARTITION_CORE_SIZEUTILS_H
