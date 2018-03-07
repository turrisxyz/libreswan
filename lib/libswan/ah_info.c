/*
 * Algorithm info parsing and creation functions
 * Author: JuanJo Ciarlante <jjo-ipsec@mendoza.gov.ar>
 *
 * Copyright (C) 2012 Paul Wouters <paul@libreswan.org>
 * Copyright (C) 2015-2017 Andrew Cagney
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "constants.h"  /* some how sucks in u_int8_t for pfkeyv2.h */
#include "libreswan/pfkeyv2.h"
#include "lswalloc.h"
#include "lswlog.h"
#include "alg_info.h"
#include "alg_byname.h"
#include "kernel_alg.h"
#include "lswfips.h"

#include "ike_alg.h"
#include "ike_alg_null.h"
#include "ike_alg_aes.h"
#include "ike_alg_sha1.h"

static bool ah_proposal_ok(const struct proposal_info *proposal,
			   char *err_buf, size_t err_buf_len)
{
	passert(proposal->encrypt == NULL);
	passert(proposal->prf == NULL);
	passert(proposal->integ != NULL);

	if (DBGP(IMPAIR_ALLOW_NULL_NULL))
		return true;

	/* ah=null is invalid */
	if (proposal->integ == &ike_alg_integ_none) {
		snprintf(err_buf, err_buf_len,
			 "AH cannot have 'none' as the integrity algorithm");
		return false;
	}
	return true;
}

static const struct ike_alg *default_ah_integ[] = {
	&ike_alg_integ_sha1.common,
	NULL,
};

const struct proposal_defaults ah_defaults = {
	.integ = default_ah_integ,
};

const struct proposal_protocol ah_proposal_protocol = {
	.name = "AH",
	.ikev1_alg_id = IKEv1_ESP_ID,
	.protoid = PROTO_IPSEC_AH,
	.ikev1_defaults = &ah_defaults,
	.ikev2_defaults = &ah_defaults,
	.proposal_ok = ah_proposal_ok,
	.integ_alg_byname = integ_alg_byname,
	.dh_alg_byname = dh_alg_byname,
};

/*
 * ??? why is this called _ah_ when almost everything refers to esp?
 * XXX: Because it is parsing an "ah" line which requires a different
 * parser configuration - encryption isn't allowed.
 *
 * ??? the only difference between this and alg_info_esp is in two
 * parameters to alg_info_parse_str.  XXX: Things are down to just the
 * last parameter being different - but that is critical as it
 * determines what is allowed.
 *
 * XXX: On the other hand, since "struct ike_info" and "struct
 * esp_info" are effectively the same, they can be merged.  Doing
 * that, would eliminate the AH using ESP confusion.
 */

/* This function is tested in testing/algparse/algparse.c */
struct alg_info_esp *alg_info_ah_create_from_str(const struct proposal_policy *policy,
						 const char *alg_str,
						 char *err_buf, size_t err_buf_len)
{
	/*
	 * alg_info storage should be sized dynamically
	 * but this may require two passes to know
	 * transform count in advance.
	 */
	struct alg_info_esp *alg_info_ah = alloc_thing(struct alg_info_esp, "alg_info_ah");

	if (!alg_info_parse_str(policy,
				&alg_info_ah->ai,
				alg_str,
				err_buf, err_buf_len,
				&ah_proposal_protocol)) {
		passert(err_buf[0] != '\0');
		alg_info_free(&alg_info_ah->ai);
		return NULL;
	}

	/* This call can free ALG_INFO_AH. */
	alg_info_discover_pfsgroup_hack(alg_info_ah, alg_str,
					err_buf, err_buf_len);
	if (err_buf[0] != '\0') {
		alg_info_free(&alg_info_ah->ai);
		return NULL;
	}

	return alg_info_ah;
}
