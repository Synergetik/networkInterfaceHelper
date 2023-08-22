////////////////////////////////////////
// Network Interface Helper SWIG file
////////////////////////////////////////

%module(directors="1") la_networkInterfaceHelper

#if defined(SWIGCSHARP)
#define %nspaceapp(x) %nspace x
#elif defined(SWIGPYTHON)
%feature("flatnested", "1");    // Flatten nested classes
%rename(Unknown) None;          // Rename all "None" identifiers to "Unknown"
%ignore hash;                   // Ignore any hash structres (not needed)
%rename(__str__) operator std::string;
%rename(__int__) operator int;
#define %nspaceapp(x)
#endif

%include <stl.i>
%include <std_string.i>
%include <stdint.i>
%include <std_array.i>
%include <std_vector.i>
%include <windows.i>
#ifdef SWIGCSHARP
%include <arrays_csharp.i>
#endif

// Generated wrapper file needs to include our header file
%{
		#include <iomanip>
		#include <sstream>
		#include <la/networkInterfaceHelper/networkInterfaceHelper.hpp>
%}

// C# Specifics
#if defined(SWIGCSHARP)
// Optimize code generation by enabling RVO
%typemap(out, optimal="1") SWIGTYPE
%{
		$result = new $1_ltype(($1_ltype const&)$1);
%}
// Marshal all std::string as UTF8Str
%typemap(imtype, outattributes="[return: System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPUTF8Str)]", inattributes="[System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPUTF8Str)] ") std::string, std::string const& "string"
// Better debug display
%typemap(csattributes) la::networkInterface::IPAddress "[System.Diagnostics.DebuggerDisplay(\"{toString()}\")]"
#endif

////////////////////////////////////////
// IPAddress
////////////////////////////////////////
%nspaceapp(la::networkInterface::IPAddress);
%ignore la::networkInterface::IPAddress::IPAddress(IPAddress&&); // Ignore move constructor
%ignore la::networkInterface::IPAddress::operator bool; // Ignore bool operator (equivalent to isValid)
%ignore la::networkInterface::IPAddress::operator value_type_v4; // Ignore value_type_v4 operator (equivalent to getIPV4)
%ignore la::networkInterface::IPAddress::operator value_type_v6; // Ignore value_type_v6 operator (equivalent to getIPV6)
%ignore la::networkInterface::IPAddress::operator value_type_packed_v4; // Ignore value_type_packed_v4 operator (equivalent to getIPV4Packed)
%ignore la::networkInterface::IPAddress::hash; // Ignore hash (not needed)
%rename("toString") la::networkInterface::IPAddress::operator std::string;
%ignore operator++(IPAddress& lhs); // Redefined in %extend
%ignore operator--(IPAddress& lhs); // Redefined in %extend
%ignore operator&(IPAddress const& lhs, IPAddress const& rhs); // Redefined in %extend
%ignore operator|(IPAddress const& lhs, IPAddress const& rhs); // Redefined in %extend
// Extend the class
%extend la::networkInterface::IPAddress
{
		IPAddress& increment()
		{
				++(*$self);
				return *$self;
		}
		IPAddress& decrement()
		{
				--(*$self);
				return *$self;
		}
		static IPAddress Add(IPAddress const& lhs, std::uint32_t const value)
		{
				return lhs + value;
		}
		static IPAddress Sub(IPAddress const& lhs, std::uint32_t const value)
		{
				return lhs - value;
		}
		static IPAddress And(IPAddress const& lhs, IPAddress const& rhs)
		{
				return lhs & rhs;
		}
		static IPAddress Or(IPAddress const& lhs, IPAddress const& rhs)
		{
				return lhs | rhs;
		}
#if defined(SWIGCSHARP)
	// Provide a more native ToString() method
	std::string ToString() const noexcept
#elif defined(SWIGPYTHON)
	// Provide a more native __str__() method
	std::string __repr__() const noexcept
#endif
	{
		return static_cast<std::string>(*$self);
	}

#if defined(SWIGCSHARP)
	// Provide a more native ToString() method
	std::string ToString() const noexcept
#elif defined(SWIGPYTHON)
	// Provide a more native __str__() method
	std::string __repr__() const noexcept
#endif
	{
		return *$self == other;
	}
};
// Enable some templates
%template(IPAddressV4) std::array<std::uint8_t, 4>;
%template(IPAddressV6) std::array<std::uint16_t, 8>;

////////////////////////////////////////
// IPAddressInfo
////////////////////////////////////////
%nspaceapp(la::networkInterface::IPAddressInfo);

////////////////////////////////////////
// Interface
////////////////////////////////////////
%nspaceapp(la::networkInterface::Interface);
// Extend the struct
%extend la::networkInterface::Interface
{
	// Add default constructor
	Interface()
	{
		return new la::networkInterface::Interface();
	}
	// Add a copy-constructor
	Interface(la::networkInterface::Interface const& other)
	{
		return new la::networkInterface::Interface(other);
	}
#if defined(SWIGCSHARP)
	// Provide a more native Equals() method
	bool Equals(la::networkInterface::Interface const& other) const noexcept
#elif defined(SWIGPYTHON)
	// Provide a more native __eq__() method
	bool __eq__(la::networkInterface::Interface const& other) const noexcept
#endif
	{
		return $self->id == other.id && $self->description == other.description && $self->alias == other.alias && $self->macAddress == other.macAddress && $self->ipAddressInfos == other.ipAddressInfos && $self->gateways == other.gateways && $self->type == other.type && $self->isEnabled == other.isEnabled && $self->isConnected == other.isConnected && $self->isVirtual == other.isVirtual;
	}
};

// Enable some templates
%template(IPAddressInfos) std::vector<la::networkInterface::IPAddressInfo>;
%template(Gateways) std::vector<la::networkInterface::IPAddress>;
%template(MacAddress) std::array<std::uint8_t, 6>;

// Extend the struct
%extend std::array<std::uint8_t, 6>
{
#if defined(SWIGCSHARP)
	// Provide a more native ToString() method
	std::string ToString() const noexcept
#elif defined(SWIGPYTHON)
	// Provide a more native __repr__() method
	std::string __repr__() const noexcept
#endif
	{
		bool first{ true };
		std::stringstream ss;
		ss << std::hex << std::setfill('0');

		for (auto const v : *$self)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				ss << ":";
			}
			ss << std::setw(2) << static_cast<uint32_t>(v);
		}

		return ss.str();
	}
};

// Ignore MacAddressHash
%ignore la::networkInterface::MacAddressHash;

////////////////////////////////////////
// NetworkInterfaceHelper
////////////////////////////////////////
%nspaceapp(la::networkInterface::NetworkInterfaceHelper);
%nspaceapp(la::networkInterface::NetworkInterfaceHelper::Observer);
%ignore la::networkInterface::NetworkInterfaceHelper::enumerateInterfaces; // Disable this method, use Observer instead
%feature("director") la::networkInterface::NetworkInterfaceHelper::Observer;
%feature("director") la::networkInterface::NetworkInterfaceHelper::DefaultedObserver;

#define final // Final keyword not properly parsed by SWIG when used on a class
%include "la/networkInterfaceHelper/networkInterfaceHelper.hpp"
#undef final
