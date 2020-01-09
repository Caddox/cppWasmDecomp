#include "opcodes.h"

namespace opcodes
{
	ins find(byte id)
	{
		// Control instructions
		if ((id >= 0x00 && id <= 0x05) || (id >= 0x0b && id <= 0x11))
		{
			switch (id)
			{
			case 0x00: return control::unreachable;
			case 0x01: return control::nop;
			case 0x02: return control::block;
			case 0x03: return control::loop;
			case 0x04: return control::if_op;
			case 0x05: return control::else_ins;

			case 0x0b: return control::end;
			case 0x0c: return control::br;
			case 0x0d: return control::br_if;
			case 0x0e: return control::br_table;
			case 0x0f: return control::ret;
			case 0x10: return control::call;
			case 0x11: return control::call_indir;
			default: return control::error;
			}
		}

		// Variable instructions
		if ((id >= 0x20 && id <= 0x24))
		{
			switch (id)
			{
			case 0x20: return variable::local_get;
			case 0x21: return variable::local_set;
			case 0x22: return variable::local_tee;
			case 0x23: return variable::global_get;
			case 0x24: return variable::global_set;
			default: return control::error;
			}
		}

		// Memory instructions
		if (id >= 0x28 && id <= 0x40)
		{
			switch (id)
			{
			case 0x28: return memory::i32_load;
			case 0x29: return memory::i64_load;
			case 0x2a: return memory::f32_load;
			case 0x2b: return memory::f64_load;
			case 0x2c: return memory::i32_load8_s;
			case 0x2d: return memory::i32_load8_u;
			case 0x2e: return memory::i32_load16_s;
			case 0x2f: return memory::i32_load16_u;
			case 0x30: return memory::i64_load8_s;
			case 0x31: return memory::i64_load8_u;
			case 0x32: return memory::i64_load16_s;
			case 0x33: return memory::i64_load16_u;
			case 0x34: return memory::i64_load32_s;
			case 0x35: return memory::i64_load32_u;
			case 0x36: return memory::i32_store;
			case 0x37: return memory::i64_store;
			case 0x38: return memory::f32_store;
			case 0x39: return memory::f64_store;
			case 0x3a: return memory::i32_store8;
			case 0x3b: return memory::i32_store16;
			case 0x3c: return memory::i64_store8;
			case 0x3d: return memory::i64_store16;
			case 0x3e: return memory::i64_store32;
			case 0x3f: return memory::memory_size;
			case 0x40: return memory::memory_grow;
			default: return control::error;
			}
		}

		// Numeric instructions
		if (id >= 0x41 && id <= 0xbf)
		{
			using namespace numeric;
			// Numeric consts
			if (id >= 0x41 && id <= 0x44)
			{
				switch (id)
				{
				case 0x41: return i32_const;
				case 0x42: return i64_const;
				case 0x43: return f32_const;
				case 0x44: return f64_const;
				default: return control::error;
				}
			}

			// i32
			if ((id >= 0x45 && id <= 0x4f) || (id >= 0x67 && id <= 0x78) ||
				(id >= 0xa7 && id <= 0xab) || id == 0xbc)
			{
				switch (id)
				{
				case 0x45: return i32::i32_eqz;
				case 0x46: return i32::i32_eq;
				case 0x47: return i32::i32_ne;
				case 0x48: return i32::i32_lt_s;
				case 0x49: return i32::i32_lt_u;
				case 0x4a: return i32::i32_gt_s;
				case 0x4b: return i32::i32_gt_u;
				case 0x4c: return i32::i32_le_s;
				case 0x4d: return i32::i32_le_u;
				case 0x4e: return i32::i32_ge_s;
				case 0x4f: return i32::i32_ge_u;

				case 0x67: return i32::i32_clz;
				case 0x68: return i32::i32_ctz;
				case 0x69: return i32::i32_popcnt;
				case 0x6a: return i32::i32_add;
				case 0x6b: return i32::i32_sub;
				case 0x6c: return i32::i32_mul;
				case 0x6d: return i32::i32_div_s;
				case 0x6e: return i32::i32_div_u;
				case 0x6f: return i32::i32_rem_s;
				case 0x70: return i32::i32_rem_u;
				case 0x71: return i32::i32_and;
				case 0x72: return i32::i32_or;
				case 0x73: return i32::i32_xor;
				case 0x74: return i32::i32_shl;
				case 0x75: return i32::i32_shr_s;
				case 0x76: return i32::i32_shr_u;
				case 0x77: return i32::i32_rotl;
				case 0x78: return i32::i32_rotr;

				case 0xa7: return i32::i32_wrap_i64;
				case 0xa8: return i32::i32_trunc_f32_s;
				case 0xa9: return i32::i32_trunc_f32_u;
				case 0xaa: return i32::i32_trunc_f64_s;
				case 0xab: return i32::i32_trunc_f64_u;

				case 0xbc: return i32::i32_reinterpret_f32;

				default: return control::error;
				}
			}

			// i64
			if ((id >= 0x50 && id <= 0x5a) || (id >= 0x79 && id <= 0x8a) ||
				(id >= 0xac && id <= 0xb1) || id == 0xbd)
			{
				switch (id)
				{
				case 0x50: return i64::i64_eqz;
				case 0x51: return i64::i64_eq;
				case 0x52: return i64::i64_ne;
				case 0x53: return i64::i64_lt_s;
				case 0x54: return i64::i64_lt_u;
				case 0x55: return i64::i64_gt_s;
				case 0x56: return i64::i64_gt_u;
				case 0x57: return i64::i64_le_s;
				case 0x58: return i64::i64_le_u;
				case 0x59: return i64::i64_ge_s;
				case 0x5a: return i64::i64_ge_u;

				case 0x79: return i64::i64_clz;
				case 0x7a: return i64::i64_ctz;
				case 0x7b: return i64::i64_popcnt;
				case 0x7c: return i64::i64_add;
				case 0x7d: return i64::i64_sub;
				case 0x7e: return i64::i64_mul;
				case 0x7f: return i64::i64_div_s;
				case 0x80: return i64::i64_div_u;
				case 0x81: return i64::i64_rem_s;
				case 0x82: return i64::i64_rem_u;
				case 0x83: return i64::i64_and;
				case 0x84: return i64::i64_or;
				case 0x85: return i64::i64_xor;
				case 0x86: return i64::i64_shl;
				case 0x87: return i64::i64_shr_s;
				case 0x88: return i64::i64_shr_u;
				case 0x89: return i64::i64_rotl;
				case 0x8a: return i64::i64_rotr;

				case 0xac: return i64::i64_extend_i32_s;
				case 0xad: return i64::i64_extend_i32_u;
				case 0xae: return i64::i64_trunc_f32_s;
				case 0xaf: return i64::i64_trunc_f64_u;
				case 0xb0: return i64::i64_trunc_f64_s;
				case 0xb1: return i64::i64_trunc_f64_u;

				case 0xbd: return i64::i64_reinterpret_f64;

				default: return control::error;
				}

			}

			// f32
			if ((id >= 0x5b && id <= 0x60) || (id >= 0x8b && id <= 0x98) ||
				(id >= 0xb2 && id <= 0xb6) || id == 0xbe)
			{
				switch (id)
				{
				case 0x5b: return f32::f32_eq;
				case 0x5c: return f32::f32_ne;
				case 0x5d: return f32::f32_lt;
				case 0x5e: return f32::f32_gt;
				case 0x5f: return f32::f32_le;
				case 0x60: return f32::f32_ge;

					// 0x8b -> 0x98
				case 0x8b: return f32::f32_abs;
				case 0x8c: return f32::f32_neg;
				case 0x8d: return f32::f32_ceil;
				case 0x8e: return f32::f32_floor;
				case 0x8f: return f32::f32_trunc;
				case 0x90: return f32::f32_nearest;
				case 0x91: return f32::f32_sqrt;
				case 0x92: return f32::f32_add;
				case 0x93: return f32::f32_sum;
				case 0x94: return f32::f32_mul;
				case 0x95: return f32::f32_div;
				case 0x96: return f32::f32_min;
				case 0x97: return f32::f32_max;
				case 0x98: return f32::f32_copysign;

					// 0xb2 -> 0xb6
					// 0xbe
				case 0xb2: return f32::f32_convert_i32_s;
				case 0xb3: return f32::f32_convert_i32_u;
				case 0xb4: return f32::f32_convert_i64_s;
				case 0xb5: return f32::f32_convert_i64_u;
				case 0xb6: return f32::f32_demote_f64;
				case 0xbe: return f32::f32_reinterpret_i32;
				default: return control::error;
				}
			}

			// f64
			if ((id >= 0x61 && id <= 0x66) || (id >= 0x99 && id <= 0xa6) ||
				(id >= 0xb7 && id <= 0xbb) || id == 0xbf)
			{
				switch (id)
				{
					// 0x61 -> 0x66
				case 0x61: return f64::f64_eq;
				case 0x62: return f64::f64_ne;
				case 0x63: return f64::f64_lt;
				case 0x64: return f64::f64_gt;
				case 0x65: return f64::f64_le;
				case 0x66: return f64::f64_ge;

					// 0x99 -> 0xa6
				case 0x99: return f64::f64_abs;
				case 0x9a: return f64::f64_neg;
				case 0x9b: return f64::f64_ceil;
				case 0x9c: return f64::f64_floor;
				case 0x9d: return f64::f64_trunc;
				case 0x9e: return f64::f64_nearest;
				case 0x9f: return f64::f64_sqrt;
				case 0xa0: return f64::f64_add;
				case 0xa1: return f64::f64_sub;
				case 0xa2: return f64::f64_mul;
				case 0xa3: return f64::f64_div;
				case 0xa4: return f64::f64_min;
				case 0xa5: return f64::f64_max;
				case 0xa6: return f64::f64_copysign;

					// 0xb7 -> 0xbb
					// 0xbf
				case 0xb7: return f64::f64_convert_i32_s;
				case 0xb8: return f64::f64_convert_i32_u;
				case 0xb9: return f64::f64_convert_i64_s;
				case 0xba: return f64::f64_convert_i64_u;
				case 0xbb: return f64::f64_promote_f32;
				case 0xbf: return f64::f64_reinterpret_i64;

				default: return control::error;
				}

			}
		}

		return control::error;
	}

}
