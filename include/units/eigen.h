//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++23 unit conversion library with no dependencies
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2016 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units/eigen.h
/// @brief	optional, dependency-free interoperability between units and the Eigen linear-algebra library
/// @details	Including this header after <Eigen/Core> teaches Eigen how to treat a units type as a matrix
///		scalar, so a vector or matrix can hold dimensioned quantities (e.g. Eigen::Matrix<meters<double>, 3, 1>).
///		It is inert when Eigen is not present: the whole body is guarded by __has_include(<Eigen/Core>), so
///		units carries no dependency on Eigen and the header is a no-op if Eigen is unavailable.
///
///		The scalar-trait specializations unlock the same-dimension operations directly on Eigen expressions —
///		construction, storage, addition/subtraction, scaling by a plain scalar, reductions such as sum(), block
///		and Map views, and casting. The operations whose result changes dimension (a dot or cross product turns
///		meters into square_meters, a norm takes a square root) cannot go through Eigen's built-ins, which assume
///		scalar*scalar yields the same scalar; those are provided as free helper functions that compute the
///		dimensionally-correct result type.
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_eigen_h_
#define units_eigen_h_

#include <units/core.h>

#if defined __has_include
#if __has_include(<Eigen/Core>)
#include <Eigen/Core>

#include <cmath>

namespace Eigen
{
	/**
	 * @brief		Numeric traits that let Eigen use a units type as a matrix scalar.
	 * @details		Constrained to any type satisfying `units::UnitType` (a named unit such as `meters<double>`
	 *				derives from `units::unit`, so a structural specialization on `unit<...>` would not match; the
	 *				concept matches the named class). The traits forward every numeric property to the underlying
	 *				arithmetic type and mark the scalar as requiring initialization, so Eigen value-initializes
	 *				unit coefficients.
	 * @tparam		U a units type (`units::UnitType`).
	 */
	template<units::UnitType U>
	struct NumTraits<U> : NumTraits<typename units::traits::unit_traits<U>::underlying_type>
	{
		using T          = typename units::traits::unit_traits<U>::underlying_type; ///< the unit's underlying arithmetic type
		using Real       = U;
		using NonInteger = U;
		using Nested     = U;
		using Literal    = T;

		enum
		{
			IsComplex             = 0,
			IsInteger             = NumTraits<T>::IsInteger,
			IsSigned              = NumTraits<T>::IsSigned,
			RequireInitialization = 1,
			ReadCost              = 1,
			AddCost               = 1,
			MulCost               = 1
		};
	};

	/**
	 * @brief		Result-type trait for scaling a unit scalar by a plain arithmetic scalar (unit * scalar).
	 * @details		Scaling preserves the dimension, so `meters<double> * 2.0` is `meters<double>`. This lets an
	 *				Eigen expression such as `v * 2.0` compile for a vector of units. A linear scale is required, so
	 *				that a matrix scales exactly where a scalar of the same unit does: an affine reading scales in its
	 *				own scale, while a decibel value has no scalar `*` at all.
	 * @tparam		U a units type (`units::UnitType`).
	 * @tparam		X the plain arithmetic scalar type.
	 */
	template<units::UnitType U, class X>
		requires(units::traits::has_linear_scale_v<U>)
	struct ScalarBinaryOpTraits<U, X, internal::scalar_product_op<U, X>>
	{
		using ReturnType = U; ///< scaling preserves the unit's dimension
	};

	/**
	 * @brief		Result-type trait for scaling a unit scalar by a plain arithmetic scalar (scalar * unit).
	 * @tparam		X the plain arithmetic scalar type.
	 * @tparam		U a units type (`units::UnitType`).
	 */
	template<class X, units::UnitType U>
		requires(units::traits::has_linear_scale_v<U>)
	struct ScalarBinaryOpTraits<X, U, internal::scalar_product_op<X, U>>
	{
		using ReturnType = U; ///< scaling preserves the unit's dimension
	};

	/**
	 * @brief		Result-type trait for dividing a unit scalar by a plain arithmetic scalar (unit / scalar).
	 * @details		Dividing by a dimensionless factor preserves the dimension, so `meters<double> / 2.0` is
	 *				`meters<double>`; this lets an Eigen expression such as `v / 2.0` compile. The reverse
	 *				(scalar / unit) is intentionally not provided: its result is a reciprocal dimension, rarely the
	 *				intended elementwise operation.
	 * @tparam		U a units type (`units::UnitType`).
	 * @tparam		X the plain arithmetic scalar type.
	 */
	template<units::UnitType U, class X>
		requires(units::traits::has_linear_scale_v<U>)
	struct ScalarBinaryOpTraits<U, X, internal::scalar_quotient_op<U, X>>
	{
		using ReturnType = U; ///< dividing by a dimensionless factor preserves the unit's dimension
	};
	/**
	 * @brief		Result-type trait for the difference of two readings measured from an arbitrary origin.
	 * @details		Eigen otherwise assumes `op(T,T) -> T` for a coefficient-wise binary operation, which is exactly
	 *				what a point/amount model denies: the scalar difference of two readings is an offset-free AMOUNT,
	 *				and assigning it back into the reading type re-applies the datum. `(v - w).eval()` on a matrix of
	 *				equal celsius readings read -273.15 rather than 0 for that reason. Naming the amount type here
	 *				makes the matrix difference agree with the scalar one.
	 * @tparam		U a units type measured from an arbitrary origin.
	 */
	template<units::UnitType U>
		requires(units::traits::has_arbitrary_origin_v<U>)
	struct ScalarBinaryOpTraits<U, U, internal::scalar_difference_op<U, U>>
	{
		using ReturnType = decltype(std::declval<U>() - std::declval<U>()); ///< a difference of readings is an amount
	};

} // namespace Eigen

namespace units
{
	/**
	 * @brief		Dimensionally-correct dot product of two Eigen vectors of units.
	 * @details		Sums the elementwise products, so the result carries the product dimension of the operands —
	 *				a dot of two `meters` vectors is a `square_meters`. Eigen's built-in `dot()` assumes the product
	 *				of two scalars is the same scalar type, which is not true for units, so it cannot be used here.
	 * @tparam		DerivedA the Eigen expression type of the first operand.
	 * @tparam		DerivedB the Eigen expression type of the second operand.
	 * @param[in]	lhs the first vector.
	 * @param[in]	rhs the second vector.
	 * @returns		the dot product, in the product unit of the two operands' scalars.
	 */
	template<class DerivedA, class DerivedB>
		requires(units::traits::has_linear_scale_v<typename DerivedA::Scalar>)
	auto unit_dot(const Eigen::MatrixBase<DerivedA>& lhs, const Eigen::MatrixBase<DerivedB>& rhs)
	{
		using UnitA   = typename DerivedA::Scalar;
		using UnitB   = typename DerivedB::Scalar;
		using Product = decltype(std::declval<UnitA>() * std::declval<UnitB>());

		Product accumulator = Product(0);
		for (Eigen::Index i = 0; i < lhs.size(); ++i)
			accumulator += lhs(i) * rhs(i);
		return accumulator;
	}

	/**
	 * @brief		Dimensionally-correct squared magnitude of an Eigen vector of units.
	 * @details		Equivalent to `unit_dot(v, v)`; a `meters` vector yields a `square_meters`.
	 * @tparam		Derived the Eigen expression type.
	 * @param[in]	v the vector.
	 * @returns		the squared magnitude, in the squared unit of the vector's scalar.
	 */
	template<class Derived>
		requires(units::traits::has_linear_scale_v<typename Derived::Scalar>)
	auto unit_squared_norm(const Eigen::MatrixBase<Derived>& v)
	{
		return unit_dot(v, v);
	}

	/**
	 * @brief		Euclidean magnitude of an Eigen vector of units, in the vector's own unit.
	 * @details		The magnitude is the square root of the sum of squares. The squared sum has the squared
	 *				dimension, and its square root returns to the vector's original dimension, so the norm of a
	 *				`meters` vector is a `meters`. It is the square root of `unit_squared_norm`, so it shares that
	 *				helper's dimension-correct native accumulation (never Eigen's `norm()`, which cannot express the
	 *				dimension round-trip) and stays correct for ratio-scaled dimensionless units. An integral scalar
	 *				is promoted to floating point by `sqrt`, so a magnitude that is not a whole number is not truncated.
	 * @tparam		Derived the Eigen expression type.
	 * @param[in]	v the vector.
	 * @returns		the magnitude, in the vector's scalar unit (floating-point promoted for an integral scalar).
	 */
	template<class Derived>
		requires(units::traits::has_linear_scale_v<typename Derived::Scalar>)
	auto unit_norm(const Eigen::MatrixBase<Derived>& v)
	{
		return units::sqrt(unit_squared_norm(v));
	}

	/**
	 * @brief		Direction (dimensionless unit vector) of an Eigen vector of units.
	 * @details		Divides each component by the magnitude, yielding a plain dimensionless vector. A direction has
	 *				no dimension, so the result is not a vector of units. Each component is the dimensionless ratio of
	 *				the component to the magnitude (both the same unit), so the result is correct for ratio-scaled
	 *				dimensionless units and an integral scalar is promoted to floating point rather than truncated. The
	 *				result is sized to the input's runtime length, so it is correct for dynamically-sized vectors.
	 * @tparam		Derived the Eigen expression type.
	 * @param[in]	v the vector.
	 * @returns		a column vector of the (floating-point promoted) underlying arithmetic type, of unit length, the
	 *				same size as the input.
	 */
	template<class Derived>
		requires(units::traits::has_linear_scale_v<typename Derived::Scalar>)
	auto unit_normalized(const Eigen::MatrixBase<Derived>& v)
	{
		using Unit       = typename Derived::Scalar;
		using Underlying = detail::floating_point_promotion_t<typename traits::unit_traits<Unit>::underlying_type>;

		const auto magnitude = unit_norm(v);

		Eigen::Matrix<Underlying, Derived::RowsAtCompileTime, 1> direction(v.size());
		for (Eigen::Index i = 0; i < v.size(); ++i)
			direction(i) = (v(i) / magnitude).template to<Underlying>();
		return direction;
	}

	/**
	 * @brief		Dimensionally-correct 3D cross product of two Eigen vectors of units.
	 * @details		Each component of the cross product is a difference of products of the operands' scalars, so
	 *				the result carries the product dimension — a cross of two `meters` vectors is a `square_meters`
	 *				vector. Eigen's built-in `cross()` assumes a scalar-preserving product and cannot be used.
	 * @tparam		DerivedA the Eigen expression type of the first operand.
	 * @tparam		DerivedB the Eigen expression type of the second operand.
	 * @param[in]	lhs the first vector (size 3).
	 * @param[in]	rhs the second vector (size 3).
	 * @returns		the cross product, a 3-vector in the product unit of the two operands' scalars.
	 */
	template<class DerivedA, class DerivedB>
		requires(units::traits::has_linear_scale_v<typename DerivedA::Scalar>)
	auto unit_cross(const Eigen::MatrixBase<DerivedA>& lhs, const Eigen::MatrixBase<DerivedB>& rhs)
	{
		using UnitA   = typename DerivedA::Scalar;
		using UnitB   = typename DerivedB::Scalar;
		using Product = decltype(std::declval<UnitA>() * std::declval<UnitB>());

		Eigen::Matrix<Product, 3, 1> result;
		result(0) = lhs(1) * rhs(2) - lhs(2) * rhs(1);
		result(1) = lhs(2) * rhs(0) - lhs(0) * rhs(2);
		result(2) = lhs(0) * rhs(1) - lhs(1) * rhs(0);
		return result;
	}

	/**
	 * @brief		Apply a dimensionless matrix to a vector of units, preserving the vector's dimension.
	 * @details		The canonical case is a rotation or direction-cosine matrix (a plain `double` matrix) times a
	 *				position or velocity (a vector of units): the transform is dimensionless, so the result keeps
	 *				the input's unit. A matrix of units cannot itself carry a coherent single dimension, so the
	 *				matrix operand is a plain-scalar matrix; the vector's components are unwrapped to the underlying
	 *				type for the product and rewrapped.
	 * @tparam		MatrixDerived the Eigen expression type of the (plain-scalar) matrix.
	 * @tparam		VectorDerived the Eigen expression type of the vector of units.
	 * @param[in]	matrix the dimensionless transform.
	 * @param[in]	vector the vector of units to transform.
	 * @returns		the transformed vector, in the input vector's scalar unit.
	 */
	template<class MatrixDerived, class VectorDerived>
		requires(units::traits::has_linear_scale_v<typename VectorDerived::Scalar>)
	auto unit_transform(const Eigen::MatrixBase<MatrixDerived>& matrix, const Eigen::MatrixBase<VectorDerived>& vector)
	{
		using Unit       = typename VectorDerived::Scalar;
		using Underlying = typename traits::unit_traits<Unit>::underlying_type;

		// Operate on each component's own-scale value (raw), so a ratio-scaled dimensionless input transforms in the
		// scale the caller sees and the result reconstructs on that same scale. Every intermediate is sized to its
		// runtime length so a dynamically-sized transform does not index a zero-length matrix.
		Eigen::Matrix<Underlying, VectorDerived::RowsAtCompileTime, 1> raw(vector.size());
		for (Eigen::Index i = 0; i < vector.size(); ++i)
			raw(i) = vector(i).raw();

		const Eigen::Matrix<Underlying, MatrixDerived::RowsAtCompileTime, 1> product = matrix * raw;

		Eigen::Matrix<Unit, MatrixDerived::RowsAtCompileTime, 1> result(product.size());
		for (Eigen::Index i = 0; i < product.size(); ++i)
			result(i) = Unit(product(i));
		return result;
	}
} // namespace units

#endif // __has_include(<Eigen/Core>)
#endif // defined __has_include

#endif // units_eigen_h_
