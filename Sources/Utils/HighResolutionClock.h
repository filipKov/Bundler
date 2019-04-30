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

					LARGE_INTEGER currentFrequency;
					QueryPerformanceFrequency( &currentFrequency );
					m_tickDuration = 1e+6 * ( 1.0 / currentFrequency.QuadPart );

					QueryPerformanceCounter( &m_startTimestamp );
					m_lastTimestamp = m_startTimestamp;
				}
			}

			void Stop() {
				LARGE_INTEGER endTimestamp;
				QueryPerformanceCounter( &endTimestamp );

				if ( m_running ) {
					m_totalElpased += ( endTimestamp.QuadPart - m_startTimestamp.QuadPart ) * m_tickDuration;
					m_running = false;
				}
			}

			void Lap() {
				LARGE_INTEGER endTimestamp;
				QueryPerformanceCounter( &endTimestamp );

				if ( m_running ) {
					m_elapsedLaps.Add( ( endTimestamp.QuadPart - m_lastTimestamp.QuadPart ) * m_tickDuration );
					m_lastTimestamp = endTimestamp;
				}
			}

			template < TimeUnits units = TimeUnits::Milliseconds >
			const double GetTotalTime() const {
				return TimeConverter::Convert< TimeUnits::Microseconds, units >( m_totalElpased );
			}

			size_t GetLapCount() const {
				return m_elapsedLaps.Length();
			}

			template < TimeUnits units = TimeUnits::Milliseconds >
			double GetLap( __in const uint lapIx ) {
				_ASSERT_EXPR( lapIx < GetLapCount(), "Lap index out of range" );

				return TimeConverter::Convert< TimeUnits::Microseconds, units >( m_elapsedLaps[ lapIx ] );
			}

		protected:
			
			double m_tickDuration;
			LARGE_INTEGER m_startTimestamp;
			LARGE_INTEGER m_lastTimestamp;

			Containers::PagedVector< double > m_elapsedLaps;
			double m_totalElpased;

			bool m_running;
		};

	}

	using HighResolutionClock = Internal::HighResolutionClockImpl;

}