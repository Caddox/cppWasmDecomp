#ifndef OPCODES_H
#define OPCODES_H
#include <string>

namespace opcodes
{
	// Structure defining 
	typedef unsigned char byte;
	struct ins
	{
		byte op;					// The byte associated with the instruction
		/// <summary>The number of bytes the instructions expect to follow it.
		/// If 0xff, then infinitely long.
		///		Treat the nextBytes as a declaration of the potential ending bytes.
		///	</summary>
		int associatedBytes;
		int bytesConsumed;			// A number defining how many previous bytes the instruction eats off the stack
		std::string symbol;			// A string description of what the instruction is
		/// <summary> A list of EXPECTED bytes. If associatedBytes is 0xff, this represents a list of
		/// potential final bytes in the sequence
		/// </summary>
		byte* nextBytes = nullptr;

		ins(byte op, std::string name, int len = 0, int typeLength = 0)
		{
			this->op = op;
			associatedBytes = len;
			bytesConsumed = typeLength;
			symbol = name;
		};

		template<typename ... Ts>
		ins(byte op, std::string name, int len = 0, int typeLength = 0, Ts... ts)
		{
			this->op = op;
			associatedBytes = len;
			bytesConsumed = typeLength;
			symbol = name;

			auto size = sizeof...(ts);
			auto values = { ts... };

			if (size == 0)
				return;

			nextBytes = new byte[size];

			int i = 0;
			for (auto value : values)
			{
				nextBytes[i] = value;
				i++;
			}
		}
	};

	enum types
	{
		i32, i64,
		f32, f64,

		any
	};

	// Define an error struct
	const ins error{ 0xef, "error" };

	/// <summary>This namespace wraps the control instructions present in
	/// a given function. 
	/// If a length is given without any required bytes, assume that it 
	/// is telling you that the next byte is required but not strictly defined.
	/// 0xff represents a wildcard slot until it actually means something.
	/// </summary>
	namespace control
	{	// 0x00 -> 0x05
		// 0x0c -> 0x11
		const ins unreachable{ 0x00, "unreachable" };
		const ins nop{ 0x01, "nop" };
		const ins block{ 0x02, "block",0, 0, 0x40 };
		const ins loop{ 0x03, "loop", 0, 0, 0x40 };
		const ins if_op{ 0x04, "if", 0, 1, 0x40 };
		//const ins if_else       { 0x04, "if", 1, 0x40 };
		const ins else_ins{ 0x05, "else" };
		const ins end{ 0x0b, "end" };
		const ins br{ 0x0c, "br", 1 };
		const ins br_if{ 0x0d, "br_if",  1 , 1 };
		const ins br_table{ 0x0e, "br_table", 2 };
		const ins ret{ 0x0f, "ret" };
		const ins call{ 0x10, "call", 1 };
		const ins call_indir{ 0x11, "call_indir", 2, 0, 0xff, 0x00 };

		// Define an error struct
		const ins func_start{ 0xee, "function" };
		const ins error{ 0xef, "error" };

	}

	namespace variable
	{
		// 0x20 -> 0x24
		const ins local_get{ 0x20, "local_get", 1 };
		const ins local_set{ 0x21, "local_set", 1, 1 };
		const ins local_tee{ 0x22, "local_tee", 1, 1 };
		const ins global_get{ 0x23, "global_get", 1 };
		const ins global_set{ 0x24, "global_set", 1, 1 };
	}

	namespace memory
	{
		// 0x28 -> 0x40
		const ins i32_load{ 0x28, "i32_load", 2, 1 };
		const ins i64_load{ 0x29, "i64_load", 2 , 1 };
		const ins f32_load{ 0x2a, "f32_load", 2, 1 };
		const ins f64_load{ 0x2b, "f64_load", 2, 1 };
		const ins i32_load8_s{ 0x2c, "i32_load8_s", 2, 1 };
		const ins i32_load8_u{ 0x2d, "i32_load8_u", 2, 1 };
		const ins i32_load16_s{ 0x2e, "i32_load16_s", 2, 1 };
		const ins i32_load16_u{ 0x2f, "i32_load16_u", 2, 1 };
		const ins i64_load8_s{ 0x30, "i64_load8_s", 2, 1 };
		const ins i64_load8_u{ 0x31, "i64_load8_u", 2, 1 };
		const ins i64_load16_s{ 0x32, "i64_load16_s", 2, 1 };
		const ins i64_load16_u{ 0x33, "i64_load16_u", 2, 1 };
		const ins i64_load32_s{ 0x34, "i64_load32_s", 2, 1 };
		const ins i64_load32_u{ 0x35, "i64_load32_u", 2, 1 };
		const ins i32_store{ 0x36, "i32_store", 2, 1 };
		const ins i64_store{ 0x37, "i64_store", 2, 1 };
		const ins f32_store{ 0x38, "f32_store", 2, 1 };
		const ins f64_store{ 0x39, "f64_store", 2, 1 };
		const ins i32_store8{ 0x3a, "i32_store8", 2, 1 };
		const ins i32_store16{ 0x3b, "i32_store16", 2, 1 };
		const ins i64_store8{ 0x3c, "i64_store8", 2, 1 };
		const ins i64_store16{ 0x3d, "i64_store16", 2, 1 };
		const ins i64_store32{ 0x3e, "i64_store32", 2, 1 };
		const ins memory_size{ 0x3f, "memory_size", 1, 0, 0x00 };
		const ins memory_grow{ 0x40, "memory_grow", 1, 1, 0x00 };
	}

	namespace numeric
	{
		// Literals
		const ins i32_const{ 0x41, "i32_const", 1 };
		const ins i64_const{ 0x42, "i64_const", 1 };
		const ins f32_const{ 0x43, "f32_const", 1 };
		const ins f64_const{ 0x44, "f64_const", 1 };

		namespace i32
		{
			// 0x45 -> 0x4f
			const ins i32_eqz{ 0x45, "i32_eqz", 0, 1 };
			const ins i32_eq{ 0x46, "i32_eq", 0, 2 };
			const ins i32_ne{ 0x47, "i32_ne", 0, 2 };
			const ins i32_lt_s{ 0x48, "i32_lt_s", 0, 2 };
			const ins i32_lt_u{ 0x49, "i32_lt_u", 0, 2 };
			const ins i32_gt_s{ 0x4a, "i32_gt_s", 0, 2 };
			const ins i32_gt_u{ 0x4b, "i32_gt_u", 0, 2 };
			const ins i32_le_s{ 0x4c, "i32_le_s", 0, 2 };
			const ins i32_le_u{ 0x4d, "i32_le_u", 0, 2 };
			const ins i32_ge_s{ 0x4e, "i32_ge_s", 0, 2 };
			const ins i32_ge_u{ 0x4f, "i32_ge_u", 0, 2 };

			// 0x67 -> 0x78
			const ins i32_clz{ 0x67, "i32_clz", 0, 1 };
			const ins i32_ctz{ 0x68, "i32_ctz", 0, 1 }; // Count trailing zero bits
			const ins i32_popcnt{ 0x69, "i32_popcnt", 0, 1 };
			const ins i32_add{ 0x6a, "i32_add", 0, 2 };
			const ins i32_sub{ 0x6b, "i32_sub", 0, 2 };
			const ins i32_mul{ 0x6c, "i32_mul", 0, 2 };
			const ins i32_div_s{ 0x6d, "i32_div_s", 0, 2 };
			const ins i32_div_u{ 0x6e, "i32_div_u", 0, 2 };
			const ins i32_rem_s{ 0x6f, "i32_rem_s", 0, 2 };
			const ins i32_rem_u{ 0x70, "i32_rem_u", 0, 2 };
			const ins i32_and{ 0x71, "i32_and", 0, 2 };
			const ins i32_or{ 0x72, "i32_or", 0, 2 };
			const ins i32_xor{ 0x73, "i32_xor", 0, 2 };
			const ins i32_shl{ 0x74, "i32_shl", 0, 2 };
			const ins i32_shr_s{ 0x75, "i32_shr_s", 0, 2 };
			const ins i32_shr_u{ 0x76, "i32_shr_u", 0, 2 };
			const ins i32_rotl{ 0x77, "i32_rotl", 0, 2 };
			const ins i32_rotr{ 0x78, "i32_rotr", 0, 2 };

			// 0xa7 -> 0xab
			// 0xbc
			const ins i32_wrap_i64{ 0xa7, "i32_wrap_i64", 0, 1 };
			const ins i32_trunc_f32_s{ 0xa8, "i32_trunc_f32_s", 0, 1 };
			const ins i32_trunc_f32_u{ 0xa9, "i32_trunc_f32_u", 0, 1 };
			const ins i32_trunc_f64_s{ 0xaa, "i32_trunc_f64_s", 0, 1 };
			const ins i32_trunc_f64_u{ 0xab, "i32_trunc_f64_u", 0, 1 };
			const ins i32_reinterpret_f32{ 0xbc, "i32_reinterpret_f32", 0, 1 };
		}

		namespace i64
		{
			// 0b50 -> 0x5a
			const ins i64_eqz{ 0x50, "i64_eqz", 0, 1 };
			const ins i64_eq{ 0x51, "i64_eq", 0, 2 };
			const ins i64_ne{ 0x52, "i64_ne", 0, 2 };
			const ins i64_lt_s{ 0x53, "i64_lt_s", 0, 2 };
			const ins i64_lt_u{ 0x54, "i64_lt_u", 0, 2 };
			const ins i64_gt_s{ 0x55, "i64_gt_s", 0, 2 };
			const ins i64_gt_u{ 0x56, "i64_gt_u", 0, 2 };
			const ins i64_le_s{ 0x57, "i64_le_s", 0, 2 };
			const ins i64_le_u{ 0x58, "i64_le_u", 0, 2 };
			const ins i64_ge_s{ 0x59, "i64_ge_s", 0, 2 };
			const ins i64_ge_u{ 0x5a, "i64_ge_u", 0, 2 };

			// 0x79 -> 0x8a
			const ins i64_clz{ 0x79, "i64_clz", 0, 1 };
			const ins i64_ctz{ 0x7a, "i64_ctz", 0, 1 };
			const ins i64_popcnt{ 0x7b, "i64_popcnt", 0, 1 };
			const ins i64_add{ 0x7c, "i64_add", 0, 2 };
			const ins i64_sub{ 0x7d, "i64_sub", 0, 2 };
			const ins i64_mul{ 0x7e, "i64_mul", 0, 2 };
			const ins i64_div_s{ 0x7f, "i64_div_s", 0, 2 };
			const ins i64_div_u{ 0x80, "i64_div_u", 0, 2 };
			const ins i64_rem_s{ 0x81, "i64_rem_s", 0, 2 };
			const ins i64_rem_u{ 0x82, "i64_rem_u", 0, 2 };
			const ins i64_and{ 0x83, "i64_and", 0, 2 };
			const ins i64_or{ 0x84, "i64_or", 0, 2 };
			const ins i64_xor{ 0x85, "i64_xor", 0, 2 };
			const ins i64_shl{ 0x86, "i64_shl", 0, 2 };
			const ins i64_shr_s{ 0x87, "i64_shr_s", 0, 2 };
			const ins i64_shr_u{ 0x88, "i64_shr_u", 0, 2 };
			const ins i64_rotl{ 0x89, "i64_rotl", 0, 2 };
			const ins i64_rotr{ 0x8a, "i64_rotr", 0, 2 };

			// 0xac -> 0xb1
			// 0xbd
			const ins i64_extend_i32_s{ 0xac, "i64_extend_i32_s", 0, 1 };
			const ins i64_extend_i32_u{ 0xad, "i64_extend_i32_u", 0, 1 };
			const ins i64_trunc_f32_s{ 0xae, "i64_trunc_f32_s", 0, 1 };
			const ins i64_trunc_f32_u{ 0xaf, "i64_trunc_f32_u", 0, 1 };
			const ins i64_trunc_f64_s{ 0xb0, "i64_trunc_f64_s", 0, 1 };
			const ins i64_trunc_f64_u{ 0xb1, "i64_trunc_f64_u", 0, 1 };
			const ins i64_reinterpret_f64{ 0xbd, "i64_reinterpret_f64", 0, 1 };

		}

		namespace f32
		{
			// 0x5b -> 0x60
			const ins f32_eq{ 0x5b, "f32_eq", 0, 2 };
			const ins f32_ne{ 0x5c, "f32_ne", 0, 2 };
			const ins f32_lt{ 0x5d, "f32_lt", 0, 2 };
			const ins f32_gt{ 0x5e, "f32_gt", 0, 2 };
			const ins f32_le{ 0x5f, "f32_le", 0, 2 };
			const ins f32_ge{ 0x60, "f32_ge", 0, 2 };

			// 0x8b -> 0x98
			const ins f32_abs{ 0x8b, "f32_abs", 0, 1 };
			const ins f32_neg{ 0x8c, "f32_neg", 0, 1 };
			const ins f32_ceil{ 0x8d, "f32_ceil", 0, 1 };
			const ins f32_floor{ 0x8e, "f32_floor", 0, 1 };
			const ins f32_trunc{ 0x8f, "f32_trunc", 0, 1 };
			const ins f32_nearest{ 0x90, "f32_nearest", 0, 1 };
			const ins f32_sqrt{ 0x91, "f32_sqrt", 0, 1 };
			const ins f32_add{ 0x92, "f32_add", 0, 2 };
			const ins f32_sum{ 0x93, "f32_sum", 0, 2 };
			const ins f32_mul{ 0x94, "f32_mul", 0, 2 };
			const ins f32_div{ 0x95, "f32_div", 0, 2 };
			const ins f32_min{ 0x96, "f32_min", 0, 2 };
			const ins f32_max{ 0x97, "f32_max", 0, 2 };
			const ins f32_copysign{ 0x98, "f32_copysign", 0, 2 };

			// 0xb2 -> 0xb6
			// 0xbe
			const ins f32_convert_i32_s{ 0xb2, "f32_convert_i32_s", 0, 1 };
			const ins f32_convert_i32_u{ 0xb3, "f32_convert_i32_u", 0, 1 };
			const ins f32_convert_i64_s{ 0xb4, "f32_convert_i64_s", 0, 1 };
			const ins f32_convert_i64_u{ 0xb5, "f32_convert_i64_u", 0, 1 };
			const ins f32_demote_f64{ 0xb6, "f32_demote_f64", 0, 1 };
			const ins f32_reinterpret_i32{ 0xbe, "f32_reinterpret_i32", 0, 1 };

		}

		namespace f64
		{
			// 0x61 -> 0x66
			const ins f64_eq{ 0x61, "f64_eq", 0, 2 };
			const ins f64_ne{ 0x62, "f64_ne", 0, 2 };
			const ins f64_lt{ 0x63, "f64_lt", 0, 2 };
			const ins f64_gt{ 0x64, "f64_gt", 0, 2 };
			const ins f64_le{ 0x65, "f64_le", 0, 2 };
			const ins f64_ge{ 0x66, "f64_ge", 0, 2 };

			// 0x99 -> 0xa6
			const ins f64_abs{ 0x99, "f64_abs", 0, 1 };
			const ins f64_neg{ 0x9a, "f64_neg", 0, 1 };
			const ins f64_ceil{ 0x9b, "f64_ceil", 0, 1 };
			const ins f64_floor{ 0x9c, "f64_floor", 0, 1 };
			const ins f64_trunc{ 0x9d, "f64_trunc", 0, 1 };
			const ins f64_nearest{ 0x9e, "f64_nearest", 0, 1 };
			const ins f64_sqrt{ 0x9f, "f64_sqrt", 0, 1 };
			const ins f64_add{ 0xa0, "f64_add", 0, 2 };
			const ins f64_sub{ 0xa1, "f64_sub", 0, 2 };
			const ins f64_mul{ 0xa2, "f64_mul", 0, 2 };
			const ins f64_div{ 0xa3, "f64_div", 0, 2 };
			const ins f64_min{ 0xa4, "f64_min", 0, 2 };
			const ins f64_max{ 0xa5, "f64_max", 0, 2 };
			const ins f64_copysign{ 0xa6, "f64_copysign", 0, 2 };

			// 0xb7 -> 0xbb
			// 0xbf
			const ins f64_convert_i32_s{ 0xb7, "f64_convert_i32_s", 0, 1 };
			const ins f64_convert_i32_u{ 0xb8, "f64_convert_i32_u", 0, 1 };
			const ins f64_convert_i64_s{ 0xb9, "f64_convert_i64_s", 0, 1 };
			const ins f64_convert_i64_u{ 0xba, "f64_convert_i64_u", 0, 1 };
			const ins f64_promote_f32{ 0xbb, "f64_promote_f32", 0, 1 };
			const ins f64_reinterpret_i64{ 0xbf, "f64_reinterpret_i64", 0, 1 };
		}
	}

	/*************************** Function Section *************************/
	ins find(byte id);
}
#endif // OPCODES_H
