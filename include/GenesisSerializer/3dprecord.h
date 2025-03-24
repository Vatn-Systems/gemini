#pragma once

#include "console/ciheader.h"
#include "GlfLoggerGlobalTypes.h"

#include <vector>

namespace GLF
{

/*!
 * \class GLogPData
 * \brief
 */
struct AFX_EXT_CONSOLE_INTERFACE GLogPointData {

    console::PublicMessageHeader m_header;     // common interface header

    uint16_t m_positionFractionBits;
    uint16_t m_intensityFractionBits;
    float m_sonarFOV;

    std::vector<int32_t> m_pointData;

    GLogPointData()
    {
        m_positionFractionBits = 12;
        m_intensityFractionBits = 16;
    }
    GLogPointData(const GLogPointData& in)
    {
        *this = in;
    }

    GLogPointData& operator =(const GLogPointData& gLogPointData)
    {
        m_header.m_ciHeader = gLogPointData.m_header.m_ciHeader;
        m_positionFractionBits = gLogPointData.m_positionFractionBits;
        m_intensityFractionBits = gLogPointData.m_intensityFractionBits;
        m_sonarFOV = gLogPointData.m_sonarFOV;

        // basic vector copy will do...
        m_pointData = gLogPointData.m_pointData;

        return *this;
    }

    size_t size() const
    {
        size_t size = 0;
        size += sizeof(m_header) +
                sizeof(m_positionFractionBits) +
                sizeof(m_intensityFractionBits) +
                (m_pointData.size() * sizeof(uint32_t));
        return size;
    }

    UInt32 CopyTo(UInt8* buffer) const
    {
        UInt32 index = 0;
        memcpy( &buffer[index], &m_header.m_ciHeader, sizeof(m_header.m_ciHeader) );
        index = sizeof(m_header.m_ciHeader);

        UInt32 pointDataSize = m_pointData.size() * sizeof(uint32_t);
        memcpy( &buffer[index], m_pointData.data(), pointDataSize);
        index += pointDataSize;
        return index;
    }

    UInt32 CopyFrom (const UInt8* buffer)
    {
        UInt32 index = 0;
        memcpy( &m_header.m_ciHeader, &buffer[index], sizeof(m_header.m_ciHeader) );
        index = sizeof(m_header.m_ciHeader);

        return index;
    }
};

}
