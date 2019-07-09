#pragma once

namespace Bundler {

	using Scalar = float;
	using ScalarType = float; // DEPRECATED, DO NOT USE!

	using Vector2 = LinearAlgebra::Vector< Scalar, 2 >;
	using Vector3 = LinearAlgebra::Vector< Scalar, 3 >;
	using Vector4 = LinearAlgebra::Vector< Scalar, 4 >;

	using Matrix3x3 = LinearAlgebra::Matrix< Scalar, 3, 3 >;
	using Matrix3x4 = LinearAlgebra::Matrix< Scalar, 3, 4 >;
	using Matrix4x4 = LinearAlgebra::Matrix< Scalar, 4, 4 >;

	template < uint parameterCount >
	using DScalar = AutomaticDifferentiation::DiffNum< Scalar, parameterCount >;

	template < uint parameterCount >
	using DVector2 = LinearAlgebra::Vector< DScalar< parameterCount >, 2 >;

	template < uint parameterCount >
	using DVector3 = LinearAlgebra::Vector< DScalar< parameterCount >, 3 >;

	template < uint parameterCount >
	using DVector4 = LinearAlgebra::Vector< DScalar< parameterCount >, 4 >;

	template < uint parameterCount >
	using DMatrix3x3 = LinearAlgebra::Matrix< DScalar< parameterCount >, 3, 3 >;

	template < uint parameterCount >
	using DMatrix3x4 = LinearAlgebra::Matrix< DScalar< parameterCount >, 3, 4 >;

	template < uint parameterCount >
	using DMatrix4x4 = LinearAlgebra::Matrix< DScalar< parameterCount >, 4, 4 >;



}