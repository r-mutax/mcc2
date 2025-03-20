#pragma once

// DWARF directives


typedef struct Dwarf_dstr Dwarf_dstr;

// debug_string table
struct Dwarf_dstr {
    int id;
    char* str;
    Dwarf_dstr* next;
};

#define DW_ATTR(X, Y)   do {print("  .uleb128 0x%x\n", X); print("  .uleb128 0x%x\n", Y);} while(0)

// attribute form encoding
// attribute form encoding
enum dwarf_form_encoding {
    DW_FORM_addr            = 0x01,  // 0x01
    /* 0x02 is reserved */
    DW_FORM_block2          = 0x03,  // 0x03
    DW_FORM_block4,                  // 0x04
    DW_FORM_data2,                   // 0x05
    DW_FORM_data4,                   // 0x06
    DW_FORM_data8,                   // 0x07
    DW_FORM_string,                  // 0x08
    DW_FORM_block,                   // 0x09
    DW_FORM_block1,                  // 0x0a
    DW_FORM_data1,                   // 0x0b
    DW_FORM_flag,                    // 0x0c
    DW_FORM_sdata,                   // 0x0d
    DW_FORM_strp,                    // 0x0e
    DW_FORM_udata,                   // 0x0f
    DW_FORM_ref_addr,                // 0x10
    DW_FORM_ref1,                    // 0x11
    DW_FORM_ref2,                    // 0x12
    DW_FORM_ref4,                    // 0x13
    DW_FORM_ref8,                    // 0x14
    DW_FORM_ref_udata,               // 0x15
    DW_FORM_indirect,                // 0x16
    DW_FORM_sec_offset,              // 0x17
    DW_FORM_exprloc,                 // 0x18
    DW_FORM_flag_present,            // 0x19
    DW_FORM_strx,                    // 0x1a
    DW_FORM_addrx,                   // 0x1b
    DW_FORM_ref_sup4,                // 0x1c
    DW_FORM_strp_sup,                // 0x1d
    DW_FORM_data16,                  // 0x1e
    DW_FORM_line_strp,               // 0x1f
    DW_FORM_ref_sig8,                // 0x20
    DW_FORM_implicit_const,          // 0x21
    DW_FORM_loclistx,                // 0x22
    DW_FORM_rnglistx,                // 0x23
    DW_FORM_ref_sup8,                // 0x24
    DW_FORM_strx1,                   // 0x25
    DW_FORM_strx2,                   // 0x26
    DW_FORM_strx3,                   // 0x27
    DW_FORM_strx4,                   // 0x28
    DW_FORM_addrx1,                  // 0x29
    DW_FORM_addrx2,                  // 0x2a
    DW_FORM_addrx3,                  // 0x2b
    DW_FORM_addrx4                   // 0x2c
};

// attribute encoding
enum dwarf_attribute_encoding {
    DW_AT_sibling           = 0x01,  // 0x01
    DW_AT_location,                  // 0x02
    DW_AT_name,                      // 0x03
    // 0x04-0x08 reserved
    DW_AT_ordering          = 0x09,  // 0x09
    // 0x0A reserved
    DW_AT_byte_size         = 0x0B,  // 0x0B
    // 0x0C reserved
    DW_AT_bit_size          = 0x0D,  // 0x0D
    // 0x0E-0x0F reserved
    DW_AT_stmt_list         = 0x10,  // 0x10
    DW_AT_low_pc,                    // 0x11
    DW_AT_high_pc,                   // 0x12
    DW_AT_language,                  // 0x13
    // 0x14 reserved
    DW_AT_discr             = 0x15,  // 0x15
    DW_AT_discr_value,               // 0x16
    DW_AT_visibility,                // 0x17
    DW_AT_import,                    // 0x18
    DW_AT_string_length,             // 0x19
    DW_AT_common_reference,          // 0x1A
    DW_AT_comp_dir,                  // 0x1B
    DW_AT_const_value,               // 0x1C
    DW_AT_containing_type,           // 0x1D
    DW_AT_default_value,             // 0x1E
    // 0x1F reserved
    DW_AT_inline            = 0x20,  // 0x20
    DW_AT_is_optional,               // 0x21
    DW_AT_lower_bound,               // 0x22
    // 0x23-0x24 reserved
    DW_AT_producer          = 0x25,  // 0x25
    // 0x26 reserved
    DW_AT_prototyped        = 0x27,  // 0x27
    // 0x28-0x29 reserved
    DW_AT_return_addr       = 0x2A,  // 0x2A
    // 0x2B reserved
    DW_AT_start_scope       = 0x2C,  // 0x2C
    // 0x2D reserved
    DW_AT_bit_stride        = 0x2E,  // 0x2E
    DW_AT_upper_bound,               // 0x2F
    // 0x30 reserved
    DW_AT_abstract_origin   = 0x31,  // 0x31
    DW_AT_accessibility,             // 0x32
    DW_AT_address_class,             // 0x33
    DW_AT_artificial,                // 0x34
    DW_AT_base_types,                // 0x35
    DW_AT_calling_convention,        // 0x36
    DW_AT_count,                     // 0x37
    DW_AT_data_member_location,      // 0x38
    DW_AT_decl_column,               // 0x39
    DW_AT_decl_file,                 // 0x3A
    DW_AT_decl_line,                 // 0x3B
    DW_AT_declaration,               // 0x3C
    DW_AT_discr_list,                // 0x3D
    DW_AT_encoding,                  // 0x3E
    DW_AT_external,                  // 0x3F
    DW_AT_frame_base,                // 0x40
    DW_AT_friend,                    // 0x41
    DW_AT_identifier_case,           // 0x42
    // 0x43 reserved
    DW_AT_namelist_item     = 0x44,  // 0x44
    DW_AT_priority,                  // 0x45
    DW_AT_segment,                   // 0x46
    DW_AT_specification,             // 0x47
    DW_AT_static_link,               // 0x48
    DW_AT_type,                      // 0x49
    DW_AT_use_location,              // 0x4A
    DW_AT_variable_parameter,        // 0x4B
    DW_AT_virtuality,                // 0x4C
    DW_AT_vtable_elem_location,      // 0x4D
    DW_AT_allocated,                 // 0x4E
    DW_AT_associated,                // 0x4F
    DW_AT_data_location,             // 0x50
    DW_AT_byte_stride,               // 0x51
    DW_AT_entry_pc,                  // 0x52
    DW_AT_use_UTF8,                  // 0x53
    DW_AT_extension,                 // 0x54
    DW_AT_ranges,                    // 0x55
    DW_AT_trampoline,                // 0x56
    DW_AT_call_column,               // 0x57
    DW_AT_call_file,                 // 0x58
    DW_AT_call_line,                 // 0x59
    DW_AT_description,               // 0x5A
    DW_AT_binary_scale,              // 0x5B
    DW_AT_decimal_scale,             // 0x5C
    DW_AT_small,                     // 0x5D
    DW_AT_decimal_sign,              // 0x5E
    DW_AT_digit_count,               // 0x5F
    DW_AT_picture_string,            // 0x60
    DW_AT_mutable,                   // 0x61
    DW_AT_threads_scaled,            // 0x62
    DW_AT_explicit,                  // 0x63
    DW_AT_object_pointer,            // 0x64
    DW_AT_endianity,                 // 0x65
    DW_AT_elemental,                 // 0x66
    DW_AT_pure,                      // 0x67
    DW_AT_recursive,                 // 0x68
    DW_AT_signature,                 // 0x69
    DW_AT_main_subprogram,           // 0x6A
    DW_AT_data_bit_offset,           // 0x6B
    DW_AT_const_expr,                // 0x6C
    DW_AT_enum_class,                // 0x6D
    DW_AT_linkage_name,              // 0x6E
    DW_AT_string_length_bit_size,    // 0x6F
    DW_AT_string_length_byte_size,   // 0x70
    DW_AT_rank,                      // 0x71
    DW_AT_str_offsets_base,          // 0x72
    DW_AT_addr_base,                 // 0x73
    DW_AT_rnglists_base,             // 0x74
    // 0x75 reserved
    DW_AT_dwo_name          = 0x76,  // 0x76
    DW_AT_reference,                 // 0x77
    DW_AT_rvalue_reference,          // 0x78
    DW_AT_macros,                    // 0x79
    DW_AT_call_all_calls,            // 0x7A
    DW_AT_call_all_source_calls,     // 0x7B
    DW_AT_call_all_tail_calls,       // 0x7C
    DW_AT_call_return_pc,            // 0x7D
    DW_AT_call_value,                // 0x7E
    DW_AT_call_origin,               // 0x7F
    DW_AT_call_parameter,            // 0x80
    DW_AT_call_pc,                   // 0x81
    DW_AT_call_tail_call,            // 0x82
    DW_AT_call_target,               // 0x83
    DW_AT_call_target_clobbered,     // 0x84
    DW_AT_call_data_location,        // 0x85
    DW_AT_call_data_value,           // 0x86
    DW_AT_noreturn,                  // 0x87
    DW_AT_alignment,                 // 0x88
    DW_AT_export_symbols,            // 0x89
    DW_AT_deleted,                   // 0x8A
    DW_AT_defaulted,                 // 0x8B
    DW_AT_loclists_base,             // 0x8C
    // 0x8D-0x1FFF reserved
    DW_AT_lo_user           = 0x2000,// 0x2000
    DW_AT_hi_user           = 0x3FFF // 0x3FFF
};

// tag encoding
#define DW_ABBREV_IDX()     print("  .uleb128 0x%x\n", g_abbrev_idx++)
#define DW_ABBREV_TAG(X)    print("  .uleb128 0x%x\n", X);
#define DW_CHILDREN_no()    print("  .byte 0x00\n")
#define DW_CHILDREN_yes()   print("  .byte 0x01\n")

enum dwarf_tag_encoding {
    DW_TAG_array_type       = 0x01,  // 0x01
    DW_TAG_class_type,               // 0x02
    DW_TAG_entry_point,              // 0x03
    DW_TAG_enumeration_type,         // 0x04
    DW_TAG_formal_parameter,         // 0x05
    // 0x06 - 0x07 reserved
    DW_TAG_imported_declaration  = 0x08,  // 0x08
    // 0x09 reserved
    DW_TAG_label            = 0x0A,  // 0x0A
    DW_TAG_lexical_block,            // 0x0B
    // 0x0C reserved
    DW_TAG_member           = 0x0D,  // 0x0D
    // 0x0E reserved
    DW_TAG_pointer_type     = 0x0F,  // 0x0F
    DW_TAG_reference_type,           // 0x10
    DW_TAG_compile_unit,             // 0x11
    DW_TAG_string_type,              // 0x12
    DW_TAG_structure_type,           // 0x13
    // 0x14 reserved
    DW_TAG_subroutine_type   = 0x15,  // 0x15
    DW_TAG_typedef          = 0x16,  // 0x16
    DW_TAG_union_type,               // 0x17
    DW_TAG_unspecified_parameters,   // 0x18
    DW_TAG_variant,                  // 0x19
    DW_TAG_common_block,             // 0x1A
    DW_TAG_common_inclusion,         // 0x1B
    DW_TAG_inheritance,              // 0x1C
    DW_TAG_inlined_subroutine,       // 0x1D
    DW_TAG_module,                   // 0x1E
    DW_TAG_ptr_to_member_type,       // 0x1F
    DW_TAG_set_type,                 // 0x20
    DW_TAG_subrange_type,            // 0x21
    DW_TAG_with_stmt,                // 0x22
    DW_TAG_access_declaration,       // 0x23
    DW_TAG_base_type,                // 0x24
    DW_TAG_catch_block,              // 0x25
    DW_TAG_const_type,               // 0x26
    DW_TAG_constant,                 // 0x27
    DW_TAG_enumerator,               // 0x28
    DW_TAG_file_type,                // 0x29
    DW_TAG_friend,                   // 0x2A
    DW_TAG_namelist,                 // 0x2B
    DW_TAG_namelist_item,            // 0x2C
    DW_TAG_packed_type,              // 0x2D
    DW_TAG_subprogram,               // 0x2E
    DW_TAG_template_type_parameter,  // 0x2F
    DW_TAG_template_value_parameter, // 0x30
    DW_TAG_thrown_type,              // 0x31
    DW_TAG_try_block,                // 0x32
    DW_TAG_variant_part,             // 0x33
    DW_TAG_variable,                 // 0x34
    DW_TAG_volatile_type,            // 0x35
    DW_TAG_dwarf_procedure,          // 0x36
    DW_TAG_restrict_type,            // 0x37
    DW_TAG_interface_type,           // 0x38
    DW_TAG_namespace,                // 0x39
    DW_TAG_imported_module,          // 0x3A
    DW_TAG_unspecified_type,         // 0x3B
    DW_TAG_partial_unit,             // 0x3C
    DW_TAG_imported_unit,            // 0x3D
    // 0x3E reserved
    DW_TAG_condition,                // 0x3F
    DW_TAG_shared_type,              // 0x40
    DW_TAG_type_unit,                // 0x41
    DW_TAG_rvalue_reference_type,    // 0x42
    DW_TAG_template_alias,           // 0x43
    DW_TAG_coarray_type,             // 0x44
    DW_TAG_generic_subrange,         // 0x45
    DW_TAG_dynamic_type,             // 0x46
    DW_TAG_atomic_type,              // 0x47
    DW_TAG_call_site,                // 0x48
    DW_TAG_call_site_parameter,      // 0x49
    DW_TAG_skeleton_unit,            // 0x4A
    DW_TAG_immutable_type,           // 0x4B
    DW_TAG_lo_user           = 0x4080,// 0x4080
    DW_TAG_hi_user           = 0xFFFF // 0xFFFF
};

