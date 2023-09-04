////////////////////////////////////////
// Network Interface Helper SWIG file
////////////////////////////////////////

%module(directors="1", threads="1") la_networkInterfaceHelper

#if defined(SWIGCSHARP)
  #if !defined(%nspaceapp)
    #define %nspaceapp(...) %nspace __VA_ARGS__
  #endif

  %rename(ToString) operator std::string;						// Converty any operator std::string() method to C# ToStirng() 
  %csmethodmodifiers operator std::string "public override";
  %csmethodmodifiers ToString "public override";
  
  #define SWIG_NATIVE_TO_STRING_FUNC_NAME   ToString
  #define SWIG_NATIVE_EQUALS_FUNC_NAME      Equals
#elif defined(SWIGPYTHON)
  #if !defined(%nspaceapp)
    #define %nspaceapp(...)
  #endif

  %feature("nothreadallow");

  %feature("flatnested", "1");              // Flatten nested classes
  %feature("python:annotations", "c");      // Enable annotations for python type hints
  
  %rename(Unknown) None;                    // Rename all "None" identifiers to "Unknown"
  
  %rename(__repr__) operator std::string;  // Converty any operator std::string() method to python __repr__
  %rename(__int__) operator int;           // Converty any operator int() method to python __int__
  
  #define SWIG_NATIVE_TO_STRING_FUNC_NAME   __repr__
  #define SWIG_NATIVE_EQUALS_FUNC_NAME      __eq__
#endif

%ignore hash;                             // Ignore any hash structres (not needed)


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

		// Provide a more native Equals() method
		bool SWIG_NATIVE_EQUALS_FUNC_NAME(la::networkInterface::IPAddress const& other) const noexcept
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

		// Provide a more native Equals() method
		bool SWIG_NATIVE_EQUALS_FUNC_NAME(la::networkInterface::Interface const& other) const noexcept
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
		// Provide a more native ToString() method
		std::string SWIG_NATIVE_TO_STRING_FUNC_NAME() const noexcept
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
%ignore la::networkInterface::NetworkInterfaceHelper::enumerateInterfaces; // Disable this method, use Observer instead

%nspaceapp(la::networkInterface::NetworkInterfaceHelper::Observer);
%feature("director") la::networkInterface::NetworkInterfaceHelper::Observer;

%nspaceapp(la::networkInterface::NetworkInterfaceHelper::DefaultedObserver);
%feature("director") la::networkInterface::NetworkInterfaceHelper::DefaultedObserver;

#define final // Final keyword not properly parsed by SWIG when used on a class
%include "la/networkInterfaceHelper/networkInterfaceHelper.hpp"
#undef final
