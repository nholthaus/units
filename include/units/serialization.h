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
/// @file	units/serialization.h
/// @brief	self-describing binary serialization of quantities
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_serialization_h_
#define units_serialization_h_

#include <units/core.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace units
{
	namespace detail
	{
		/// The library's known dimensions, offered to `visit` as the default candidate set so a stream of any
		/// built-in dimension resolves without the caller naming it. This is NOT the wire vocabulary: the stream
		/// keys each base dimension by a hash of its `name` STRING, so a dimension the library has never seen —
		/// including a user-defined `make_dimension<my_tag>` — still serializes and round-trips. `visit` cannot
		/// resolve a user-defined dimension unless the caller lists it (`visit<my_dimension>(f)`), because C++ cannot
		/// materialize a type from the runtime hash — the runtime→type wall. The set is otherwise open by design.
		using builtin_dimensions = std::tuple<dimension::length, dimension::mass, dimension::time, dimension::current, dimension::temperature,
			dimension::substance, dimension::luminous_intensity, dimension::angle, dimension::data, dimension::solid_angle, dimension::frequency,
			dimension::velocity, dimension::angular_velocity, dimension::acceleration, dimension::force, dimension::area, dimension::volume,
			dimension::volume_flow_rate, dimension::pressure, dimension::charge, dimension::energy, dimension::power, dimension::voltage,
			dimension::capacitance, dimension::impedance, dimension::conductance, dimension::magnetic_flux, dimension::inductance,
			dimension::luminous_flux, dimension::illuminance, dimension::luminance, dimension::radioactivity, dimension::substance_mass,
			dimension::substance_concentration, dimension::magnetic_field_strength, dimension::radiant_intensity, dimension::radiance,
			dimension::irradiance, dimension::spectral_intensity, dimension::spectral_flux, dimension::spectral_radiance,
			dimension::spectral_irradiance, dimension::jerk, dimension::torque, dimension::density, dimension::energy_density,
			dimension::concentration>;

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: name_hash [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      FNV-1a 64-bit hash of a base dimension's name
		/// @details	Hashing the library-controlled `name` string (not a compiler-generated type name) is portable
		///				across compilers and versions, is a fixed 8 bytes regardless of name length, and needs no
		///				central table — any dimension name, built-in or user-defined, hashes to a stable wire key.
		/// @param[in]  name  the dimension name
		/// @return     the 64-bit hash
		//------------------------------------------------------------------------------------------------------------------
		constexpr std::uint64_t name_hash(std::string_view name) noexcept
		{
			std::uint64_t h = 1469598103934665603ULL;
			for (char c : name)
			{
				h ^= static_cast<std::uint8_t>(c);
				h *= 1099511628211ULL;
			}
			return h;
		}
	} // namespace detail

	/// @brief	the reasons a deserialize can fail
	enum class deserialize_error
	{
		truncated,              ///< the byte range ended before a complete quantity was read
		bad_version,            ///< the stream's format version is not understood
		dimension_mismatch,     ///< the stream's dimension does not match the requested target
		unknown_base_dimension, ///< the stream names a base-dimension code this build does not know
		lossy_target            ///< the value cannot be represented in the requested underlying type without loss
	};

	/// @brief	a decoded quantity whose concrete type was not known at the call site
	/// @details	`deserialize` yields this erased value. It carries the dimension signature and the magnitude
	///				in SI canonical base. It has NO arithmetic operators — to compute, collapse it into a concrete
	///				unit with `to<Unit>()` (safe, returns `std::expected`), `try_to<Unit>()` / `unit_cast<Unit>()`
	///				(throwing), or `visit()` (the canonical unit for the decoded dimension, no target named).
	class any_unit;

	//======================================================================================================================
	//	VARINT (LEB128) — the terse integer codec
	//======================================================================================================================

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: put_uvarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      appends an unsigned integer to a byte buffer as an LEB128 varint
		/// @param[out] out    buffer to append to
		/// @param[in]  value  value to encode
		//------------------------------------------------------------------------------------------------------------------
		inline void put_uvarint(std::vector<std::byte>& out, std::uint64_t value)
		{
			do
			{
				std::uint8_t byte = value & 0x7F;
				value >>= 7;
				if (value != 0)
					byte |= 0x80;
				out.push_back(std::byte{byte});
			} while (value != 0);
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: put_svarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      appends a signed integer to a byte buffer as a zig-zag LEB128 varint
		/// @param[out] out    buffer to append to
		/// @param[in]  value  value to encode
		//------------------------------------------------------------------------------------------------------------------
		inline void put_svarint(std::vector<std::byte>& out, std::int64_t value)
		{
			put_uvarint(out, (static_cast<std::uint64_t>(value) << 1) ^ static_cast<std::uint64_t>(value >> 63));
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: get_uvarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      reads an LEB128 unsigned varint from a byte cursor
		/// @param[in,out] cursor  pointer into the buffer; advanced past the value on success
		/// @param[in]  end     one-past-the-end of the buffer
		/// @param[out] value   decoded value
		/// @return     true on success, false if the buffer was truncated
		//------------------------------------------------------------------------------------------------------------------
		inline bool get_uvarint(const std::byte*& cursor, const std::byte* end, std::uint64_t& value)
		{
			value             = 0;
			unsigned int shift = 0;
			while (cursor != end)
			{
				const std::uint8_t byte = std::to_integer<std::uint8_t>(*cursor++);
				value |= static_cast<std::uint64_t>(byte & 0x7F) << shift;
				if ((byte & 0x80) == 0)
					return true;
				shift += 7;
				if (shift >= 64)
					return false;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: get_svarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      reads a zig-zag LEB128 signed varint from a byte cursor
		/// @param[in,out] cursor  pointer into the buffer; advanced past the value on success
		/// @param[in]  end     one-past-the-end of the buffer
		/// @param[out] value   decoded value
		/// @return     true on success, false if the buffer was truncated
		//------------------------------------------------------------------------------------------------------------------
		inline bool get_svarint(const std::byte*& cursor, const std::byte* end, std::int64_t& value)
		{
			std::uint64_t raw;
			if (!get_uvarint(cursor, end, raw))
				return false;
			value = static_cast<std::int64_t>((raw >> 1) ^ (~(raw & 1) + 1));
			return true;
		}
	} // namespace detail

	//======================================================================================================================
	//	UNIT IDENTITY — the compile-time dimension signature, and its runtime form
	//======================================================================================================================

	/// @brief	one base-dimension term of a signature: which base dimension (by name-hash), and its rational exponent
	/// @details	The base dimension is identified by an FNV-1a hash of its `name` string, so an arbitrary — including
	///				user-defined — base dimension round-trips in a fixed 8 bytes with no central registry.
	struct dimension_term
	{
		std::uint64_t hash; ///< FNV-1a hash of the base dimension's name; the wire identity
		std::int64_t  num;  ///< exponent numerator
		std::int64_t  den;  ///< exponent denominator (1 for the common integer-exponent case)
	};

	/// @brief	the runtime identity of a quantity's dimension — the set of nonzero base-dimension terms
	/// @details	Backed by a `std::vector`, so there is NO baked-in ceiling on the number of base dimensions a
	///				quantity may compose. The terms are held sorted by hash so equality is order-independent.
	struct unit_identity
	{
		std::vector<dimension_term> terms;

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator== [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      dimension-signature equality
		/// @param[in]  other  identity to compare against
		/// @return     true iff both carry the same nonzero base-dimension terms (by hash and exponent)
		//------------------------------------------------------------------------------------------------------------------
		bool operator==(const unit_identity& other) const noexcept
		{
			if (terms.size() != other.terms.size())
				return false;
			for (std::size_t i = 0; i < terms.size(); ++i)
				if (terms[i].hash != other.terms[i].hash || terms[i].num != other.terms[i].num || terms[i].den != other.terms[i].den)
					return false;
			return true;
		}
	};

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: dimension_arity [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the number of base-dimension terms in a `dimension_t<...>` list
		/// @tparam     DimensionList  a `dimension_t<...>` specialization
		/// @return     the term count
		//------------------------------------------------------------------------------------------------------------------
		template<class DimensionList>
		consteval std::size_t dimension_arity()
		{
			if constexpr (DimensionList::empty)
				return 0;
			else
				return 1 + dimension_arity<typename DimensionList::pop_front>();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: fill_terms [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      writes a term per base dimension of a `dimension_t<...>` list into a fixed span
		/// @details	Each term is keyed by a hash of the base dimension's `name`, so ANY base dimension — built-in
		///				or user-defined `make_dimension<my_tag>` — is captured; there is no fixed vocabulary.
		/// @tparam     DimensionList  a `dimension_t<...>` specialization
		/// @tparam     N  the fixed capacity of the destination array
		/// @param[out] out  the destination array
		/// @param[in]  at   the index to write the front term at
		//------------------------------------------------------------------------------------------------------------------
		template<class DimensionList, std::size_t N>
		consteval void fill_terms(std::array<dimension_term, N>& out, std::size_t at)
		{
			if constexpr (!DimensionList::empty)
			{
				using front_dim = typename DimensionList::front;
				using tag       = typename front_dim::dimension;
				using exponent  = typename front_dim::exponent;
				out[at]         = dimension_term{name_hash(std::string_view(tag::name)), exponent::num, exponent::den};
				fill_terms<typename DimensionList::pop_front, N>(out, at + 1);
			}
		}

		/// @brief	the compile-time signature of a unit as a fixed-size, sorted array of terms
		/// @tparam	Unit  a `UnitType`
		template<UnitType Unit>
		struct signature
		{
			using Dim                            = traits::dimension_of_t<typename traits::unit_traits<Unit>::conversion_factor>;
			static constexpr std::size_t arity   = dimension_arity<Dim>();

			static consteval std::array<dimension_term, arity> compute()
			{
				std::array<dimension_term, arity> terms{};
				fill_terms<Dim, arity>(terms, 0);
				// insertion sort by hash — arity is tiny (the base dimensions of one quantity)
				for (std::size_t i = 1; i < arity; ++i)
				{
					dimension_term key = terms[i];
					std::size_t    j   = i;
					while (j > 0 && terms[j - 1].hash > key.hash)
					{
						terms[j] = terms[j - 1];
						--j;
					}
					terms[j] = key;
				}
				return terms;
			}

			static constexpr std::array<dimension_term, arity> value = compute();
		};

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: identity_of [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the dimension signature of a unit type, as the runtime (vector-backed) identity
		/// @details	Materializes the compile-time fixed-array signature into a `unit_identity` for runtime compare.
		/// @tparam     Unit  a `UnitType`
		/// @return     the unit_identity (nonzero base-dimension terms, sorted by hash)
		//------------------------------------------------------------------------------------------------------------------
		template<UnitType Unit>
		unit_identity identity_of()
		{
			unit_identity id;
			id.terms.assign(signature<Unit>::value.begin(), signature<Unit>::value.end());
			return id;
		}

		/// @brief	the canonical SI base unit of a dimension (ratio 1, no pi, no translation)
		template<class Dim>
		using canonical_unit_t = unit<conversion_factor<std::ratio<1>, Dim>, double>;
	} // namespace detail

	//======================================================================================================================
	//	WIRE FORMAT
	//======================================================================================================================

	namespace detail
	{
		inline constexpr std::uint8_t serialization_version = 1;

		/// header byte layout: [ valueKind:2 | fracExp:1 | reserved:5 ]
		enum class value_kind : std::uint8_t
		{
			ivarint = 0, ///< value is an integer in SI base, zig-zag varint
			f32     = 1, ///< value is an exact 32-bit float
			f64     = 2  ///< value is a 64-bit double
		};
	} // namespace detail

	//======================================================================================================================
	//	any_unit
	//======================================================================================================================

	class any_unit
	{
	public:
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: any_unit [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      constructs an erased quantity from a decoded identity and SI-base magnitude
		/// @param[in]  id     the dimension signature
		/// @param[in]  base   the magnitude in SI canonical base
		//------------------------------------------------------------------------------------------------------------------
		any_unit(unit_identity id, double base) noexcept
		  : m_identity(std::move(id))
		  , m_base(base)
		{
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: is [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      whether this erased quantity is of the requested dimension
		/// @tparam     Dimension  a `dimension::*` type
		/// @return     true iff the decoded dimension matches
		//------------------------------------------------------------------------------------------------------------------
		template<class Dimension>
		[[nodiscard]] bool is() const noexcept
		{
			return m_identity == detail::identity_of<detail::canonical_unit_t<Dimension>>();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: value_in_base [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the magnitude in SI canonical base units, for logging or routing
		/// @return     the value
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] double value_in_base() const noexcept { return m_base; }

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: identity [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the decoded dimension signature
		/// @return     the unit_identity
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] const unit_identity& identity() const noexcept { return m_identity; }

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: to [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      collapses into a concrete unit, checked (the safe default)
		/// @tparam     Unit  the target unit type
		/// @return     the value as `Unit` on a dimension match, else `deserialize_error::dimension_mismatch`
		//------------------------------------------------------------------------------------------------------------------
		template<UnitType Unit>
		[[nodiscard]] std::expected<Unit, deserialize_error> to() const
		{
			if (m_identity != detail::identity_of<Unit>())
				return std::unexpected(deserialize_error::dimension_mismatch);
			using Dim  = traits::dimension_of_t<typename traits::unit_traits<Unit>::conversion_factor>;
			using Base = detail::canonical_unit_t<Dim>;
			return Unit(Base(m_base));
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: try_to [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      collapses into a concrete unit, throwing on a dimension mismatch
		/// @tparam     Unit  the target unit type
		/// @return     the value as `Unit`
		//------------------------------------------------------------------------------------------------------------------
		template<UnitType Unit>
		[[nodiscard]] Unit try_to() const
		{
			auto result = to<Unit>();
			if (!result)
				throw std::runtime_error("units::any_unit: dimension mismatch collapsing to the requested unit");
			return *result;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: visit [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      invokes a visitor with the canonical quantity for the decoded dimension
		/// @details	The visitor is called with the canonical SI unit of whichever candidate dimension the stream
		///				holds, so no target type is named at the call site and all arithmetic inside the visitor is
		///				compile-time checked. With no explicit candidates, every dimension the library defines is a
		///				candidate (so velocity/force/energy/... resolve out of the box); pass explicit candidate
		///				dimensions (`visit<my_dimension>(f)`) to resolve a user-defined dimension or to disambiguate
		///				dimensions that share a signature (e.g. torque vs energy — the first listed wins). The visitor
		///				must be a generic callable (e.g. a `[](auto q)` lambda). Throws if no candidate matched.
		/// @tparam     Dimensions  candidate dimension types (defaults to the library's known dimensions)
		/// @tparam     Visitor  a callable invocable with each candidate's canonical unit
		/// @param[in]  visitor  the callable
		//------------------------------------------------------------------------------------------------------------------
		template<class... Dimensions, class Visitor>
		void visit(Visitor&& visitor) const
		{
			bool matched;
			if constexpr (sizeof...(Dimensions) == 0)
				matched = dispatch_tuple<Visitor, detail::builtin_dimensions>(std::forward<Visitor>(visitor));
			else
				matched = dispatch_list<Visitor, Dimensions...>(std::forward<Visitor>(visitor));
			if (!matched)
				throw std::runtime_error("units::any_unit: no candidate dimension matched the stream");
		}

	private:
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: try_dispatch_one [private]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      if the erased dimension matches `Dimension`, invokes the visitor with its canonical unit
		/// @tparam     Dimension  the candidate dimension
		/// @tparam     Visitor  the visitor callable
		/// @param[in]  visitor  the callable
		/// @return     true iff the dimension matched and the visitor was invoked
		//------------------------------------------------------------------------------------------------------------------
		template<class Dimension, class Visitor>
		bool try_dispatch_one(Visitor&& visitor) const
		{
			using Base = detail::canonical_unit_t<Dimension>;
			if (m_identity == detail::identity_of<Base>())
			{
				std::forward<Visitor>(visitor)(Base(m_base));
				return true;
			}
			return false;
		}

		/// dispatch over an explicit candidate pack, in order (first match wins)
		template<class Visitor, class... Dimensions>
		bool dispatch_list(Visitor&& visitor) const
		{
			bool matched = false;
			// fold in order; stop invoking once matched
			((matched = matched || try_dispatch_one<Dimensions>(std::forward<Visitor>(visitor))), ...);
			return matched;
		}

		/// dispatch over a tuple of candidate dimensions, in order (first match wins)
		template<class Visitor, class DimTuple, std::size_t I = 0>
		bool dispatch_tuple(Visitor&& visitor) const
		{
			if constexpr (I < std::tuple_size_v<DimTuple>)
			{
				if (try_dispatch_one<std::tuple_element_t<I, DimTuple>>(std::forward<Visitor>(visitor)))
					return true;
				return dispatch_tuple<Visitor, DimTuple, I + 1>(std::forward<Visitor>(visitor));
			}
			return false;
		}

		unit_identity m_identity;
		double        m_base;
	};

	//======================================================================================================================
	//	unit_cast — reclaimed: the explicit throwing collapse from any_unit to a concrete unit
	//======================================================================================================================

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: unit_cast [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      explicit cast from an erased `any_unit` to a concrete unit, throwing on a dimension mismatch
	/// @details	The free-function cast idiom for the same operation as `any_unit::try_to<Unit>()`. Mirrors
	///				`std::any_cast`. This is the purpose `unit_cast` was created for.
	/// @tparam     Target  the target unit type
	/// @param[in]  value   the erased quantity
	/// @return     the value as `Target`
	//----------------------------------------------------------------------------------------------------------------------
	template<UnitType Target>
	[[nodiscard]] Target unit_cast(const any_unit& value)
	{
		return value.try_to<Target>();
	}

	//======================================================================================================================
	//	serialize / deserialize
	//======================================================================================================================

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: serialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      serializes a quantity to a self-describing byte stream
	/// @details	The stream carries the dimension signature and the magnitude in SI canonical base. A peer can decode
	///				it with `deserialize` without prior agreement on the type.
	/// @tparam     Unit  a `UnitType`
	/// @param[in]  quantity  the value to serialize
	/// @return     the encoded bytes
	//----------------------------------------------------------------------------------------------------------------------
	template<UnitType Unit>
	[[nodiscard]] std::vector<std::byte> serialize(const Unit& quantity)
	{
		constexpr auto& sig = detail::signature<Unit>::value;   // fixed-array compile-time signature (sorted by hash)

		// value in SI canonical base
		using Dim   = traits::dimension_of_t<typename traits::unit_traits<Unit>::conversion_factor>;
		using Base  = detail::canonical_unit_t<Dim>;
		const double base = Base(quantity).value();

		// choose the tersest exact value encoding
		detail::value_kind kind;
		if (base == std::floor(base) && std::abs(base) < 9.0e15)
			kind = detail::value_kind::ivarint;
		else if (static_cast<double>(static_cast<float>(base)) == base)
			kind = detail::value_kind::f32;
		else
			kind = detail::value_kind::f64;

		// any fractional exponent forces the fracExp flag
		bool fracExp = false;
		for (const auto& term : sig)
			if (term.den != 1)
				fracExp = true;

		std::vector<std::byte> out;
		out.push_back(std::byte{detail::serialization_version});
		const std::uint8_t header = static_cast<std::uint8_t>(static_cast<std::uint8_t>(kind) | (fracExp ? 0x04 : 0x00));
		out.push_back(std::byte{header});
		detail::put_uvarint(out, sig.size());
		for (const auto& term : sig)
		{
			// base dimension keyed by an 8-byte name-hash: fixed size, no central table, any dimension round-trips
			for (unsigned int i = 0; i < 8; ++i)
				out.push_back(std::byte{static_cast<std::uint8_t>(term.hash >> (8 * i))});
			detail::put_svarint(out, term.num);
			if (fracExp)
				detail::put_svarint(out, term.den);
		}

		switch (kind)
		{
		case detail::value_kind::ivarint: detail::put_svarint(out, static_cast<std::int64_t>(base)); break;
		case detail::value_kind::f32:
		{
			const float f = static_cast<float>(base);
			std::uint32_t bits;
			std::memcpy(&bits, &f, sizeof(bits));
			for (unsigned int i = 0; i < 4; ++i)
				out.push_back(std::byte{static_cast<std::uint8_t>(bits >> (8 * i))});
			break;
		}
		case detail::value_kind::f64:
		{
			std::uint64_t bits;
			std::memcpy(&bits, &base, sizeof(bits));
			for (unsigned int i = 0; i < 8; ++i)
				out.push_back(std::byte{static_cast<std::uint8_t>(bits >> (8 * i))});
			break;
		}
		}
		return out;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      decodes a self-describing byte stream into an erased quantity
	/// @param[in]  bytes  the encoded stream
	/// @return     an `any_unit` on success, else a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	[[nodiscard]] inline std::expected<any_unit, deserialize_error> deserialize(std::span<const std::byte> bytes)
	{
		const std::byte* cursor = bytes.data();
		const std::byte* end    = bytes.data() + bytes.size();

		if (cursor == end)
			return std::unexpected(deserialize_error::truncated);
		const std::uint8_t version = std::to_integer<std::uint8_t>(*cursor++);
		if (version != detail::serialization_version)
			return std::unexpected(deserialize_error::bad_version);

		if (cursor == end)
			return std::unexpected(deserialize_error::truncated);
		const std::uint8_t header  = std::to_integer<std::uint8_t>(*cursor++);
		const auto         kind    = static_cast<detail::value_kind>(header & 0x03);
		const bool         fracExp = (header & 0x04) != 0;

		std::uint64_t count = 0;
		if (!detail::get_uvarint(cursor, end, count))
			return std::unexpected(deserialize_error::truncated);

		unit_identity id;
		id.terms.reserve(count);
		for (std::uint64_t i = 0; i < count; ++i)
		{
			if (end - cursor < 8)
				return std::unexpected(deserialize_error::truncated);
			std::uint64_t hash = 0;
			for (unsigned int b = 0; b < 8; ++b)
				hash |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * b);
			std::int64_t num = 0;
			std::int64_t den = 1;
			if (!detail::get_svarint(cursor, end, num))
				return std::unexpected(deserialize_error::truncated);
			if (fracExp && !detail::get_svarint(cursor, end, den))
				return std::unexpected(deserialize_error::truncated);
			id.terms.push_back(dimension_term{hash, num, den});
		}

		double base = 0.0;
		switch (kind)
		{
		case detail::value_kind::ivarint:
		{
			std::int64_t v;
			if (!detail::get_svarint(cursor, end, v))
				return std::unexpected(deserialize_error::truncated);
			base = static_cast<double>(v);
			break;
		}
		case detail::value_kind::f32:
		{
			if (end - cursor < 4)
				return std::unexpected(deserialize_error::truncated);
			std::uint32_t bits = 0;
			for (unsigned int i = 0; i < 4; ++i)
				bits |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * i);
			float f;
			std::memcpy(&f, &bits, sizeof(f));
			base = static_cast<double>(f);
			break;
		}
		case detail::value_kind::f64:
		{
			if (end - cursor < 8)
				return std::unexpected(deserialize_error::truncated);
			std::uint64_t bits = 0;
			for (unsigned int i = 0; i < 8; ++i)
				bits |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * i);
			std::memcpy(&base, &bits, sizeof(base));
			break;
		}
		default: return std::unexpected(deserialize_error::bad_version);
		}

		return any_unit(std::move(id), base);
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      decodes a self-describing byte stream directly into a known unit type (fully static)
	/// @tparam     Unit  the expected unit type
	/// @param[in]  bytes  the encoded stream
	/// @return     the value as `Unit` on success, else a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	template<UnitType Unit>
	[[nodiscard]] std::expected<Unit, deserialize_error> deserialize(std::span<const std::byte> bytes)
	{
		auto erased = deserialize(bytes);
		if (!erased)
			return std::unexpected(erased.error());
		return erased->template to<Unit>();
	}
} // namespace units

#endif // units_serialization_h_
