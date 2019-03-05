#pragma once

namespace Bundler { namespace LinearSolver {

	class PCGSolver 
	{
	public:
		
		// void Initialize();
		
	protected:
		
		Vector< Scalar > m_r;
		Vector< Scalar > m_d;
		Vector< Scalar > m_Ad;
		Vector< Scalar > m_MInvR;

		Scalar m_errSq;

	};

} }