/*
* Copyright (C) 2016-2025, L-Acoustics

* This file is part of LA_networkInterfaceHelper.

* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:

*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  nor the names of its contributors may be used to
*    endorse or promote products derived from this software without specific
*    prior written permission.

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.

* You should have received a copy of the BSD 3-clause License
* along with LA_networkInterfaceHelper.  If not, see <https://opensource.org/licenses/BSD-3-Clause>.
*/

/**
 * @file networkInterfaceHelper_python.cpp
 * @author Vitalij Mast
 * @brief Network interface helper for python bindings library.
 */

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <la/networkInterfaceHelper/networkInterfaceHelper.hpp>

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- MacAddress wrapper and bindings --------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
class MacAddress
{
public:
    const static uint32_t size = 6;

    MacAddress() = default;

    explicit MacAddress(const std::array<std::uint8_t, 6>& bytes)
        : data_(bytes)
    {}

    explicit MacAddress(const std::string& str)
        : data_(la::networkInterface::NetworkInterfaceHelper::stringToMacAddress(str))
    {}

    const std::array<std::uint8_t, 6>& data() const
    {
        return data_;
    }
    std::array<std::uint8_t, 6>& data()
    {
        return data_;
    }

    bool isValid() const
    {
        return la::networkInterface::NetworkInterfaceHelper::isMacAddressValid(data_);
    }

    std::string repr() const
    {
        return la::networkInterface::NetworkInterfaceHelper::macAddressToString(data_, ":");
    }

private:
    std::array<std::uint8_t, size> data_;
};
