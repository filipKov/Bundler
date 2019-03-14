#pragma once

namespace Utils {

	enum class TimeUnits
	{
		Nanoseconds,
		Microseconds,
		Milliseconds,
		Seconds,
		Minutes,
		Hours,
		Days,
	};

	namespace Internal {

		// From Microsecond conversion
		template < TimeUnits units >
		struct FromMicroseconds;

		template <>
		struct FromMicroseconds< TimeUnits::Nanoseconds >
		{ 
			constexpr static const double convertConstant = 1e3; 
		};

		template <>
		struct FromMicroseconds< TimeUnits::Microseconds >
		{
			constexpr static const double convertConstant = 1e-0;
		};

		template <>
		struct FromMicroseconds< TimeUnits::Milliseconds >
		{
			constexpr static const double convertConstant = 1e-3;
		};

		template <>
		struct FromMicroseconds< TimeUnits::Seconds >
		{
			constexpr static const double convertConstant = 1e-6;
		};

		template <>
		struct FromMicroseconds< TimeUnits::Minutes >
		{
			constexpr static const double convertConstant = 1.6666666666666666666666666666667e-8;
		};

		template <>
		struct FromMicroseconds< TimeUnits::Hours >
		{
			constexpr static const double convertConstant = 2.7777777777777777777777777777778e-10;
		};

		template <>
		struct FromMicroseconds< TimeUnits::Days >
		{
			constexpr static const double convertConstant = 1.1574074074074074074074074074074e-11;
		};

		// To Microsecond conversion
		template < TimeUnits units >
		struct ToMicroseconds;

		template <>
		struct ToMicroseconds< TimeUnits::Nanoseconds >
		{
			constexpr static const double convertConstant = 1e-3;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Microseconds >
		{
			constexpr static const double convertConstant = 1e+0;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Milliseconds >
		{
			constexpr static const double convertConstant = 1e+3;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Seconds >
		{
			constexpr static const double convertConstant = 1e+6;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Minutes >
		{
			constexpr static const double convertConstant = 6e+7;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Hours >
		{
			constexpr static const double convertConstant = 3.6e+9;
		};

		template <>
		struct ToMicroseconds< TimeUnits::Days >
		{
			constexpr static const double convertConstant = 8.64e+10;
		};

		template < TimeUnits sourceUnits, TimeUnits destinationUnits >
		struct TimeConverterTraits
		{
			constexpr static const double convertConstant = ToMicroseconds< sourceUnits >::convertConstant * FromMicroseconds< destinationUnits >::convertConstant;
		};

	}

	class TimeConverter {
	public:

		template < TimeUnits sourceUnits, TimeUnits destinationUnits >
		static double Convert( __in const double time ) {
			return time * Internal::TimeConverterTraits< sourceUnits, destinationUnits >::convertConstant;
		}
	};

}