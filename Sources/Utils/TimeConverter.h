#pragma once

namespace Utils {

	enum class TimeUnits
	{
		Microseconds,
		Milliseconds,
		Seconds,
		Minutes,
		Hours,
		Days,
	};

	namespace Internal {

		template < TimeUnits units >
		struct TimeConverterTraits;

		template <>
		struct TimeConverterTraits< TimeUnits::Microseconds >
		{
			constexpr static const double convertConstant = 1e0;
		};

		template <>
		struct TimeConverterTraits< TimeUnits::Milliseconds >
		{
			constexpr static const double convertConstant = 1e-3;
		};

		template <>
		struct TimeConverterTraits< TimeUnits::Seconds >
		{
			constexpr static const double convertConstant = 1e-6;
		};

		template <>
		struct TimeConverterTraits< TimeUnits::Minutes >
		{
			constexpr static const double convertConstant = 1.6666666666666666666666666666667e-8;
		};

		template <>
		struct TimeConverterTraits< TimeUnits::Hours >
		{
			constexpr static const double convertConstant = 2.7777777777777777777777777777778e-10;
		};

		template <>
		struct TimeConverterTraits< TimeUnits::Days >
		{
			constexpr static const double convertConstant = 1.1574074074074074074074074074074e-11;
		};
	}

	class TimeConverter {
	public:

		template < TimeUnits destinationUnits >
		static double ConvertMicroseconds( __in const double time ) {
			return time * Internal::TimeConverterTraits< destinationUnits >::convertConstant;
		}
	};

}