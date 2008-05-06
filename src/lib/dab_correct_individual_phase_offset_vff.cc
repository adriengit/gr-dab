/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * config.h is generated by configure.  It contains the results
 * of probing for features, options etc.  It should be the first
 * file included in your .cc file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dab_correct_individual_phase_offset_vff.h>
#include <gr_io_signature.h>
#include <cmath>

#define M_PI_HALF M_PI/2
#define M_PI_QUARTER M_PI/4

/*
 * Create a new instance of dab_correct_individual_phase_offset_vff and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_correct_individual_phase_offset_vff_sptr 
dab_make_correct_individual_phase_offset_vff (unsigned int vlen, float alpha)
{
	return dab_correct_individual_phase_offset_vff_sptr (new dab_correct_individual_phase_offset_vff (vlen, alpha));
}

dab_correct_individual_phase_offset_vff::dab_correct_individual_phase_offset_vff (unsigned int vlen, float alpha) : 
	gr_sync_block ("correct_individual_phase_offset_vff",
	           gr_make_io_signature (1, 1, sizeof(float)*vlen),
	           gr_make_io_signature (1, 1, sizeof(float)*vlen)),
	d_vlen(vlen), d_alpha(alpha), d_debug(0)
{
  d_offset_estimation = new float[vlen];
  for (unsigned int i=0;i<vlen;i++)
    d_offset_estimation[i] = 0;
}

dab_correct_individual_phase_offset_vff::~dab_correct_individual_phase_offset_vff (void)
{
  delete [] d_offset_estimation;
}

int 
dab_correct_individual_phase_offset_vff::work (int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  float const *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];

  float ival;

  for (int i=0;i<noutput_items;i++) {
    for (unsigned int j=0;j<d_vlen;j++) {
      ival=in[0];
      while (ival>M_PI_HALF) // poor man's modulo pi/2
        ival-=M_PI_HALF;
      while (ival<0)
        ival+=M_PI_HALF;
      d_offset_estimation[j] = (1-d_alpha)*d_offset_estimation[j] + d_alpha*(ival-M_PI_QUARTER);
      out[0] = in[0] - d_offset_estimation[j];
      out++;
      in++;
    }
  }

  /* debug */
  d_debug++;
  if (d_debug%100==0) {
    printf("offset=[");
    for (unsigned int i=0; i<d_vlen; i++) 
      printf("%f,",d_offset_estimation[i]);
    printf("];\n");
  }

  return noutput_items;
}
