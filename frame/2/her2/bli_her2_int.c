/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

#define FUNCPTR_T her2_fp

typedef void (*FUNCPTR_T)( conj_t   conjh,
                           obj_t*   alpha,
                           obj_t*   alpha_conj,
                           obj_t*   x,
                           obj_t*   y,
                           obj_t*   c,
                           her2_t*  cntl );

static FUNCPTR_T vars[4][3] =
{
	// unblocked          unblocked with fusing   blocked
	{ bli_her2_unb_var1,  bli_her2_unf_var1,      bli_her2_blk_var1 },
	{ bli_her2_unb_var2,  NULL,                   bli_her2_blk_var2 },
	{ bli_her2_unb_var3,  NULL,                   bli_her2_blk_var3 },
	{ bli_her2_unb_var4,  bli_her2_unf_var4,      bli_her2_blk_var4 },
};

void bli_her2_int( conj_t   conjh,
                   obj_t*   alpha,
                   obj_t*   alpha_conj,
                   obj_t*   x,
                   obj_t*   y,
                   obj_t*   c,
                   her2_t*  cntl )
{
	varnum_t  n;
	impl_t    i;
	FUNCPTR_T f;
	obj_t     alpha_local;
	obj_t     alpha_conj_local;
	obj_t     x_local;
	obj_t     y_local;
	obj_t     c_local;

	// Check parameters.
	if ( bli_error_checking_is_enabled() )
		bli_her2_int_check( conjh, alpha, x, y, c, cntl );

	// If C, x, or y has a zero dimension, return early.
	if ( bli_obj_has_zero_dim( *c ) ) return;
	if ( bli_obj_has_zero_dim( *x ) ) return;
	if ( bli_obj_has_zero_dim( *y ) ) return;

	// Alias the operands in case we need to apply conjugations.
	bli_obj_alias_to( *x, x_local );
	bli_obj_alias_to( *y, y_local );
	bli_obj_alias_to( *c, c_local );

	// If matrix C is marked for conjugation, we interpret this as a request
	// to apply a conjugation to the other operands.
	if ( bli_obj_has_conj( c_local ) )
	{
		bli_obj_toggle_conj( c_local );

		bli_obj_toggle_conj( x_local );
		bli_obj_toggle_conj( y_local );

		bli_obj_scalar_init_detached_copy_of( bli_obj_datatype( *alpha ),
		                             BLIS_CONJUGATE,
		                             alpha,
		                             &alpha_local );
		bli_obj_scalar_init_detached_copy_of( bli_obj_datatype( *alpha_conj ),
		                             BLIS_CONJUGATE,
		                             alpha_conj,
		                             &alpha_conj_local );
	}
	else
	{
		bli_obj_alias_to( *alpha, alpha_local );
		bli_obj_alias_to( *alpha_conj, alpha_conj_local );
	}


	// Extract the variant number and implementation type.
	n = cntl_var_num( cntl );
	i = cntl_impl_type( cntl );

	// Index into the variant array to extract the correct function pointer.
	f = vars[n][i];

	// Invoke the variant.
	f( conjh,
	   &alpha_local,
	   &alpha_conj_local,
	   &x_local,
	   &y_local,
	   &c_local,
	   cntl );
}

