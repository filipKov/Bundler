//
//  Number.hpp
//  AutomaticDerivations
//
//  Created by Filip Kovac on 10/12/2016.
//  Copyright © 2016 Filip Kovac. All rights reserved.
//

#pragma once

#include "stdafx.h"

namespace NumericOptimization { namespace AutoDifferentiationOld {

/*
	Old implementation of Forward AD differentiating with respect to 1 variable
 */
class Number {
public:
	float fx;
	float dx;

public:
	Number(): fx(0.0f), dx(1.0f) {};
	Number( float x0, float dx0 = 0.0f ): fx(x0), dx(dx0) {};
	
	Number operator+ ( const Number &o ) const;
	Number operator- ( const Number &o ) const;
	Number operator* ( const Number &o ) const;
	Number operator/ ( const Number &o ) const;
	Number sin() const;
	Number cos() const;
	Number exp() const;
	Number log() const;
	Number pow( const float k ) const;
	
};

} }
