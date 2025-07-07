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
 * @file networkInterfaceHelper_python.hpp
 * @author Vitalij Mast
 * @brief Network interface helper for python bindings library.
 */

#include "networkInterfaceHelper_python.hpp"

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- pybind helpers -------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/
namespace py = pybind11;

// For non-const member functions
template<typename Ret, typename Class, typename... Args>
auto with_released_gil(Ret (Class::*method)(Args...))
{
	return [method](Class* self, Args... args) -> Ret
	{
		py::gil_scoped_release release;
		return (self->*method)(std::forward<Args>(args)...);
	};
}

// For const member functions
template<typename Ret, typename Class, typename... Args>
auto with_released_gil(Ret (Class::*method)(Args...) const)
{
	return [method](const Class* self, Args... args) -> Ret
	{
		py::gil_scoped_release release;
		return (self->*method)(std::forward<Args>(args)...);
	};
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- Declarations ---------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/
void bindMacAddress(py::module_& m);
void bindIPAddress(py::module_& m);
void bindIPAddressInfo(py::module_& m);
void bindInterface(py::module_& m);
void bindNetworkInterfaceHelper(py::module_& m);

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- Module entry definition ----------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/
PYBIND11_MODULE(la_networkInterfaceHelper, m)
{
	m.doc() = "Python bindings for la::networkInterfaceHelper";

	m.def("getLibraryVersion", &la::networkInterface::getLibraryVersion, "Gets the library version string.");
	m.def("getLibraryName", &la::networkInterface::getLibraryName, "Gets the full name of the library.");
	m.def("getLibraryCopyright", &la::networkInterface::getLibraryCopyright, "Gets the copyright string of the library.");

	bindMacAddress(m);
	bindIPAddress(m);
	bindIPAddressInfo(m);
	bindInterface(m);
	bindNetworkInterfaceHelper(m);
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- MacAddress bindings --------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
void bindMacAddress(py::module_& m)
{
	// Bind MacAddress
	py::class_<MacAddress>(m, "MacAddress")
		.def(py::init<>())
		.def(py::init<const std::array<std::uint8_t, 6>&>(), py::arg("mac_bytes"), "Constructs a MacAddress from a string like [00, 11, 22, 33, 44, 55]")
		.def(py::init<const std::string&>(), py::arg("mac_string"), "Constructs a MacAddress from a string like '00:11:22:33:44:55'")
		.def_property_readonly("isValid", &MacAddress::isValid, "Returns True if IP is valid.")
		.def("__repr__", &MacAddress::repr, "String representation of MacAddress.")
		.def("__getitem__",
			[](const MacAddress& mac, size_t i)
			{
				if (i >= mac.data().size())
					throw py::index_error();
				return mac.data()[i];
			})
		.def("__len__", [](const MacAddress&) { return MacAddress::size; });
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- IPAddress bindings ---------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
void bindIPAddress(py::module_& m)
{
	using IP = la::networkInterface::IPAddress;
	using V4 = IP::value_type_v4;
	using V6 = IP::value_type_v6;
	using PackedV4 = IP::value_type_packed_v4;
	using PackedV6 = IP::value_type_packed_v6;

	// Bind IPAddress::Type enum
	py::enum_<IP::Type>(m, "IPType", "IP address type").value("Unspecified", IP::Type::None).value("V4", IP::Type::V4).value("V6", IP::Type::V6);

	// Bind IPAddress class
	py::class_<IP>(m, "IPAddress")
		.def(py::init<>())
		.def(py::init<V4>(), "Construct from IPv4 byte array.")
		.def(py::init<V6>(), "Construct from IPv6 word array.")
		.def(py::init<PackedV4>(), "Construct from packed IPv4.")
		.def(py::init<PackedV6>(), "Construct from packed IPv6.")
		.def(py::init<const std::string&>(), "Construct from string representation (IPv4 or IPv6).")

		// Useful getters
		.def("getType", &IP::getType, "Returns the type of the IP address.")
		.def("isValid", &IP::isValid, "Returns True if IP is valid.")
		.def("getIPV4", &IP::getIPV4, "Returns the IPv4 address as byte array. Raises if not V4.")
		.def("getIPV6", &IP::getIPV6, "Returns the IPv6 address as word array. Raises if not V6.")
		.def("getIPV4Packed", &IP::getIPV4Packed, "Returns packed IPv4. Raises if not V4.")
		.def("getIPV6Packed", &IP::getIPV6Packed, "Returns packed IPv6. Raises if not V6.")

		// Setters (optional)
		.def("setValue", static_cast<void (IP::*)(V4 const)>(&IP::setValue), py::arg("ipv4"), "Set IP from IPv4 byte array.")
		.def("setValue", static_cast<void (IP::*)(V6 const)>(&IP::setValue), py::arg("ipv6"), "Set IP from IPv6 word array.")
		.def("setValue", static_cast<void (IP::*)(PackedV4 const)>(&IP::setValue), py::arg("packed_v4"), "Set IP from packed IPv4.")
		.def("setValue", static_cast<void (IP::*)(PackedV6 const)>(&IP::setValue), py::arg("packed_v6"), "Set IP from packed IPv6.")

		// Operators / conversion
		.def(
			"__str__", [](const IP& ip) { return static_cast<std::string>(ip); }, "String representation of IPAddress.")
		.def(
			"__repr__", [](const IP& ip) { return static_cast<std::string>(ip); }, "String representation of IPAddress.")
		.def("__bool__", &IP::operator bool, "Returns True if IP is valid.")
		.def(py::self == py::self, "Returns True if both IPAddress objects are equal.")
		.def(py::self != py::self, "Returns True if the IPAddress objects differ.")

		// Static helpers
		.def_static("pack", static_cast<PackedV4 (*)(V4 const)>(&IP::pack), py::arg("ipv4"), "Pack IPv4 address array into uint32.")
		.def_static("unpack", static_cast<V4 (*)(PackedV4 const)>(&IP::unpack), py::arg("packed_v4"), "Unpack packed uint32 to IPv4 array.")
		.def_static("pack", static_cast<PackedV6 (*)(V6 const)>(&IP::pack), py::arg("ipv6"), "Pack IPv6 array into pair<uint64, uint64>.")
		.def_static("unpack", static_cast<V6 (*)(PackedV6 const)>(&IP::unpack), py::arg("packed_v6"), "Unpack packed pair to IPv6 array.");
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*--- IPAddressInfo bindings ----------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
void bindIPAddressInfo(py::module_& m)
{
	using IPAddressInfo = la::networkInterface::IPAddressInfo;

	// Bind IPAddressInfo class
	py::class_<IPAddressInfo>(m, "IPAddressInfo", "Represents an IP address and its associated netmask.")
		.def(py::init<>(), "Creates an empty IPAddressInfo object with default values.")
		.def_readwrite("address", &IPAddressInfo::address, "The IP address component. Must be a valid IPAddress object.")
		.def_readwrite("netmask", &IPAddressInfo::netmask, "The netmask associated with the IP address. Must be a valid IPAddress object.")

		.def_property_readonly("networkBaseAddress", &IPAddressInfo::getNetworkBaseAddress,
			"Returns the network base address, calculated from the IP address and netmask.\n\n"
			"Raises:\n"
			"    ValueError: If address or netmask is invalid, or if their types do not match.")
		.def_property_readonly("broadcastAddress", &IPAddressInfo::getBroadcastAddress,
			"Returns the broadcast address, based on the IP address and netmask.\n\n"
			"Raises:\n"
			"    ValueError: If address or netmask is invalid, or if their types do not match.")
		.def_property_readonly("privateNetworkAddress", &IPAddressInfo::isPrivateNetworkAddress,
			"Returns True if the address is within a private IP range (RFC 1918 / RFC 4193).\n\n"
			"Raises:\n"
			"    ValueError: If address or netmask is invalid, or if their types do not match.")
		.def(py::self == py::self, "Returns True if both IPAddressInfo objects are equal.")
		.def(py::self != py::self, "Returns True if the IPAddressInfo objects differ.")
		.def(py::self < py::self, "Compares two IPAddressInfo objects for ordering.\n\n"
															"Raises:\n"
															"    ValueError: If address or netmask type is unsupported.")
		.def(py::self <= py::self, "Compares two IPAddressInfo objects for ordering (<=).\n\n"
															 "Raises:\n"
															 "    ValueError: If address or netmask type is unsupported.")
		.def(
			"__repr__",
			[](const IPAddressInfo& ipinfo)
			{
				std::ostringstream oss;
				oss << "<IPAddressInfo "
						<< "ip=" << std::string(ipinfo.address) << ", "
						<< "mask=" << std::string(ipinfo.netmask) << ">";
				return oss.str();
			},
			"Returns a string representation of the interface.");
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- Interface bindings ---------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
void bindInterface(py::module_& m)
{
	using Interface = la::networkInterface::Interface;

	// Bind Interface::Type enum
	py::enum_<Interface::Type>(m, "InterfaceType", "Enumeration of network interface types.").value("Unspecified", Interface::Type::None, "Placeholder value. Should never occur in real interfaces.").value("Loopback", Interface::Type::Loopback, "Loopback interface (127.0.0.1, ::1).").value("Ethernet", Interface::Type::Ethernet, "Wired Ethernet interface.").value("WiFi", Interface::Type::WiFi, "Wireless 802.11 WiFi interface.").value("AWDL", Interface::Type::AWDL, "Apple Wireless Direct Link interface.");

	// Bind Interface struct
	py::class_<Interface>(m, "Interface", "Represents a network interface and its associated metadata.")
		.def(py::init<>())
		.def_readwrite("id", &Interface::id, "System-assigned unique identifier for the interface (UTF-8).")
		.def_readwrite("description", &Interface::description, "System-assigned description of the interface (UTF-8).")
		.def_readwrite("alias", &Interface::alias, "User-assigned alias or name for the interface (UTF-8).")
		.def_property(
			"macAddress", [](const Interface& self) { return MacAddress(self.macAddress); }, [](Interface& self, const MacAddress& mac) { self.macAddress = mac.data(); }, "MAC address of the interface.")
		.def_readwrite("ipAddressInfos", &Interface::ipAddressInfos, "List of IPAddressInfo entries associated with this interface.")
		.def_readwrite("gateways", &Interface::gateways, "List of default gateway IP addresses for this interface.")
		.def_readwrite("type", &Interface::type, "The type of the interface (e.g., Ethernet, WiFi).")
		.def_readwrite("isEnabled", &Interface::isEnabled, "True if the interface is currently enabled.")
		.def_readwrite("isConnected", &Interface::isConnected, "True if the interface is connected to a working network.")
		.def_readwrite("isVirtual", &Interface::isVirtual, "True if this is a virtual interface (e.g., VM, Bluetooth, loopback).")
		.def(
			"__repr__",
			[](const Interface& iface)
			{
				std::ostringstream oss;
				oss << "<Interface "
						<< "alias='" << iface.alias << "', "
						<< "mac=" << la::networkInterface::NetworkInterfaceHelper::macAddressToString(iface.macAddress) << ">";
				return oss.str();
			},
			"Returns a string representation of the interface.");
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*-- NetworkInterfaceHelper and observer bindings -------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------*/
struct PyDefaultedObserver : public la::networkInterface::NetworkInterfaceHelper::DefaultedObserver
{
public:
	using DefaultedObserver::DefaultedObserver;
	using Interface = la::networkInterface::Interface;

	virtual void onInterfaceAdded(const Interface& iface) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceAdded, iface);
	}

	virtual void onInterfaceRemoved(const Interface& iface) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceRemoved, iface);
	}

	virtual void onInterfaceEnabledStateChanged(const Interface& iface, bool isEnabled) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceEnabledStateChanged, iface, isEnabled);
	}

	virtual void onInterfaceConnectedStateChanged(const Interface& iface, bool isConnected) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceConnectedStateChanged, iface, isConnected);
	}

	virtual void onInterfaceAliasChanged(const Interface& iface, const std::string& alias) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceAliasChanged, iface, alias);
	}

	virtual void onInterfaceIPAddressInfosChanged(const Interface& iface, const Interface::IPAddressInfos& ipAddressInfos) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceIPAddressInfosChanged, iface, ipAddressInfos);
	}

	virtual void onInterfaceGateWaysChanged(const Interface& iface, const Interface::Gateways& gateways) noexcept override
	{
		PYBIND11_OVERRIDE(void, DefaultedObserver, onInterfaceGateWaysChanged, iface, gateways);
	}
};

/*-------------------------------------------------------------------------------------------------------------------*/
void bindNetworkInterfaceHelper(py::module_& m)
{
	using DefaultedObserver = la::networkInterface::NetworkInterfaceHelper::DefaultedObserver;
	using NetworkInterfaceHelper = la::networkInterface::NetworkInterfaceHelper;

	// Bind DefaultedObserver delegate
	py::class_<DefaultedObserver, PyDefaultedObserver>(m, "DefaultedObserver",
		"Optional observer base class with no-op implementations of all callbacks.\n"
		"Users may subclass this in Python and override only the callbacks they need.")
		.def(py::init<>())
		.def("onInterfaceAdded", &DefaultedObserver::onInterfaceAdded, py::arg("interface"), "Called when a network interface is added.")
		.def("onInterfaceRemoved", &DefaultedObserver::onInterfaceRemoved, py::arg("interface"), "Called when a network interface is removed.")
		.def("onInterfaceEnabledStateChanged", &DefaultedObserver::onInterfaceEnabledStateChanged, py::arg("interface"), py::arg("is_enabled"), "Called when isEnabled state changes.")
		.def("onInterfaceConnectedStateChanged", &DefaultedObserver::onInterfaceConnectedStateChanged, py::arg("interface"), py::arg("is_connected"), "Called when isConnected state changes.")
		.def("onInterfaceAliasChanged", &DefaultedObserver::onInterfaceAliasChanged, py::arg("interface"), py::arg("alias"), "Called when the interface alias changes.")
		.def("onInterfaceIPAddressInfosChanged", &DefaultedObserver::onInterfaceIPAddressInfosChanged, py::arg("interface"), py::arg("ip_address_infos"), "Called when IPAddressInfos changes.")
		.def("onInterfaceGateWaysChanged", &DefaultedObserver::onInterfaceGateWaysChanged, py::arg("interface"), py::arg("gateways"), "Called when gateway list changes.");

	// Bind NetworkInterfaceHelper class
	py::class_<NetworkInterfaceHelper>(m, "NetworkInterfaceHelper", "Singleton helper for interacting with network interfaces.")
		.def_static("getInstance", &NetworkInterfaceHelper::getInstance, py::return_value_policy::reference, "Returns the singleton instance.")
		.def(
			"enumerateInterfaces",
			[](const NetworkInterfaceHelper& self, py::function handler)
			{
				py::gil_scoped_release release;
				self.enumerateInterfaces([&handler](const la::networkInterface::Interface& iface) { handler(iface); });
			},
			py::arg("handler"), "Calls the handler for each detected network interface.")
		.def("getAllInterfaces", with_released_gil(&NetworkInterfaceHelper::getAllInterfaces), "Returns a list of all current network interfaces.")
		.def("getInterfaceByName", with_released_gil(&NetworkInterfaceHelper::getInterfaceByName), py::arg("name"), "Returns the interface with the specified name.")
		.def(
			"registerObserver",
			[](NetworkInterfaceHelper& self, DefaultedObserver* observer)
			{
				py::gil_scoped_release release;
				self.registerObserver(observer);
			},
			py::arg("observer"), "Registers an observer instance (must subclass Observer in Python).")
		.def(
			"unregisterObserver",
			[](NetworkInterfaceHelper& self, DefaultedObserver* observer)
			{
				py::gil_scoped_release release;
				self.unregisterObserver(observer);
			},
			py::arg("observer"), "Unregisters a previously registered observer.");
}