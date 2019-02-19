#pragma once

namespace Utils {

	namespace Internal {

		class HighResolutionClockImpl 
		{
		public:

			HighResolutionClockImpl() {
				m_running = false;
				Clear();
			}

			void Clear() {
				if ( !m_running ) {
					m_elapsedLaps.Clear();
					m_totalElpased = 0;
				}
			}

			void Start() {
				if ( !m_running ) {
					m_running = true;

					QueryPerformanceFrequency( &m_currentFrequency );
					QueryPerformanceCounter( &m_startTimestamp );
					m_lastTimestamp = m_startTimestamp;
				}
			}

			void Stop() {
				LARGE_INTEGER endTimestamp;
				QueryPerformanceCounter( &endTimestamp );

				if ( m_running ) {
					m_totalElpased += ( ( endTimestamp.QuadPart - m_startTimestamp.QuadPart ) * 1000000 ) / m_currentFrequency.QuadPart;
					m_running = false;
				}
			}

			void Lap() {
				LARGE_INTEGER endTimestamp;
				QueryPerformanceCounter( &endTimestamp );

				if ( m_running ) {
					m_elapsedLaps.Add( ( ( endTimestamp.QuadPart - m_lastTimestamp.QuadPart ) * 1000000 ) / m_currentFrequency.QuadPart );
					m_lastTimestamp = endTimestamp;
				}
			}

			template < TimeUnits units = TimeUnits::Milliseconds >
			const double GetTotalTime() const {
				return TimeConverter::ConvertMicroseconds< units >( double( m_totalElpased ) );
			}

			size_t GetLapCount() const {
				return m_elapsedLaps.Length();
			}

			template < TimeUnits units = TimeUnits::Milliseconds >
			double GetLap( __in const uint lapIx ) {
				_ASSERT_EXPR( lapIx < GetLapCount(), "Lap index out of range" );

				return TimeConverter::ConvertMicroseconds< units >( (double)m_elapsedLaps[ lapIX ] );
			}

		protected:

			LARGE_INTEGER m_currentFrequency;
			LARGE_INTEGER m_startTimestamp;
			LARGE_INTEGER m_lastTimestamp;

			Containers::PagedVector< int64 > m_elapsedLaps;
			int64 m_totalElpased;

			bool m_running;
		};

	}

	using HighResolutionClock = Internal::HighResolutionClockImpl;

}