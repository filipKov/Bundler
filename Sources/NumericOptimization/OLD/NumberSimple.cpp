//
//  Number.cpp
//  AutomaticDerivations
//
//  Created by Filip Kovac on 10/12/2016.
//  Copyright © 2016 Filip Kovac. All rights reserved.
//

#include "stdafx.h"
#include "NumberSimple.hpp"

namespace NumericOptimization { namespace AutoDifferentiationOld {

Number Number::operator+ ( const Number &o ) const {
	return Number( fx + o.fx, dx + o.dx );
}

Number Number::operator- ( const Number &o ) const {
	return Number( fx - o.fx, dx - o.dx );
}

Number Number::operator* ( const Number &o ) const {
	return Number( fx * o.fx, ( ( fx * o.dx ) + ( dx * o.fx ) ) );
}

Number Number::operator/ ( const Number &o ) const {
	return Number( fx / o.fx, ( ((dx * o.fx) - (fx * o.dx)) / (o.fx * o.fx) ) );
}

Number Number::sin() const {
	return Number( sinf(fx), cosf(fx) );
}

Number Number::cos() const {
	return Number( cosf(fx), -sinf(fx) * dx );
}

Number Number::exp() const {
	float ePowX = expf( fx );
	return Number( ePowX, dx * ePowX );
}

Number Number::log() const {
	return Number( logf(fx), dx / fx );
}

Number Number::pow( const float k ) const {
	float powK1 = powf( fx, k - 1);
	return Number( powK1 * fx, k * powK1 * dx  );
}

} }
