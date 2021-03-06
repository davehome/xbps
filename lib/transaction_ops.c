/*-
 * Copyright (c) 2009-2012 Juan Romero Pardines.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "xbps_api_impl.h"

/**
 * @file lib/transaction_ops.c
 * @brief Transaction handling routines
 * @defgroup transaction Transaction handling functions
 *
 * The following image shows off the full transaction dictionary returned
 * by xbps_transaction_prepare().
 *
 * @image html images/xbps_transaction_dictionary.png
 *
 * Legend:
 *  - <b>Salmon bg box</b>: The transaction dictionary.
 *  - <b>White bg box</b>: mandatory objects.
 *  - <b>Grey bg box</b>: optional objects.
 *  - <b>Green bg box</b>: possible value set in the object, only one of them
 *    will be set.
 *
 * Text inside of white boxes are the key associated with the object, its
 * data type is specified on its edge, i.e string, array, integer, dictionary.
 */
enum {
	TRANS_INSTALL = 1,
	TRANS_UPDATE
};

static int
transaction_find_pkg(struct xbps_handle *xhp,
		     const char *pkg,
		     bool bypattern,
		     bool best,
		     bool exact,
		     int action)
{
	prop_dictionary_t pkg_pkgdb, pkg_repod;
	prop_array_t unsorted;
	const char *pkgname, *repoloc, *repover, *repopkgver, *instver, *reason;
	int rv = 0;
	pkg_state_t state = 0;

	assert(pkg != NULL);

	if (action == TRANS_INSTALL) {
		/* install */
		reason = "install";
	} else {
		/* update */
		if ((pkg_pkgdb = xbps_pkgdb_get_pkgd(xhp, pkg, false)) == NULL)
			return ENODEV;

		reason = "update";
	}

	/*
	 * Find out if the pkg has been found in repository pool.
	 */
	if (action == TRANS_INSTALL) {
		if (exact) {
			pkg_repod = xbps_rpool_find_pkg_exact(xhp, pkg);
			if (pkg_repod == NULL) {
				/* not found */
				return errno;
			}
		} else {
			if (((pkg_repod = xbps_rpool_find_pkg(xhp, pkg, bypattern, best)) == NULL) &&
			    ((pkg_repod = xbps_rpool_find_virtualpkg_conf(xhp, pkg, bypattern)) == NULL) &&
			    ((pkg_repod = xbps_rpool_find_virtualpkg(xhp, pkg, bypattern)) == NULL)) {
				/* not found */
				return errno;
			}
		}
	} else {
		pkg_repod = xbps_rpool_find_pkg(xhp, pkg, false, true);
		if (pkg_repod == NULL) {
			/* not found */
			return errno;
		}
	}
	prop_dictionary_get_cstring_nocopy(pkg_repod, "pkgname", &pkgname);
	prop_dictionary_get_cstring_nocopy(pkg_repod, "version", &repover);
	prop_dictionary_get_cstring_nocopy(pkg_repod, "pkgver", &repopkgver);
	prop_dictionary_get_cstring_nocopy(pkg_repod, "repository", &repoloc);

	if (action == TRANS_UPDATE) {
		/*
		 * Compare installed version vs best pkg available in repos.
		 */
		prop_dictionary_get_cstring_nocopy(pkg_pkgdb,
		    "version", &instver);
		if (xbps_cmpver(repover, instver) <= 0) {
			xbps_dbg_printf(xhp, "[rpool] Skipping `%s-%s' "
			    "(installed: %s-%s) from repository `%s'\n",
			    pkgname, repover, pkgname, instver, repoloc);
			return EEXIST;
		}
	}
	/*
	 * Prepare transaction dictionary.
	 */
	if ((rv = xbps_transaction_init(xhp)) != 0)
		return rv;

	/*
	 * Find out if package has matched conflicts.
	 */
	xbps_pkg_find_conflicts(xhp, pkg_repod);

	unsorted = prop_dictionary_get(xhp->transd, "unsorted_deps");
	if (unsorted == NULL)
		return EINVAL;
	/*
	 * Find out if package being updated matches the one already
	 * in transaction, in that case ignore it.
	 */
	if (action == TRANS_UPDATE) {
		if (xbps_find_pkg_in_array_by_pkgver(xhp,
		    unsorted, repopkgver, NULL)) {
			xbps_dbg_printf(xhp, "[update] `%s' already queued in "
			    "transaction.\n", repopkgver);
			return EEXIST;
		}
	}

	/*
	 * Prepare required package dependencies and add them into the
	 * "unsorted" array in transaction dictionary.
	 */
	if ((rv = xbps_repository_find_pkg_deps(xhp, pkg_repod)) != 0)
		return rv;
	/*
	 * Set package state in dictionary with same state than the
	 * package currently uses, otherwise not-installed.
	 */
	if ((rv = xbps_pkg_state_installed(xhp, pkgname, &state)) != 0) {
		if (rv != ENOENT)
			return rv;
		/* Package not installed, don't error out */
		state = XBPS_PKG_STATE_NOT_INSTALLED;
	}
	if ((rv = xbps_set_pkg_state_dictionary(pkg_repod, state)) != 0)
		return rv;

	if (state == XBPS_PKG_STATE_UNPACKED)
		reason = "configure";
	else if (state == XBPS_PKG_STATE_NOT_INSTALLED ||
		 state == XBPS_PKG_STATE_HALF_UNPACKED)
		reason = "install";

	/*
	 * Set transaction obj in pkg dictionary to "install", "configure"
	 * or "update".
	 */
	if (!prop_dictionary_set_cstring_nocopy(pkg_repod,
	    "transaction", reason))
		return EINVAL;

	/*
	 * Add the pkg dictionary from repository's index dictionary into
	 * the "unsorted" array in transaction dictionary.
	 */
	if (!prop_array_add(unsorted, pkg_repod))
		return errno;

	xbps_dbg_printf(xhp, "%s-%s: added into the transaction (%s).\n",
	    pkgname, repover, repoloc);

	return rv;
}

int
xbps_transaction_update_packages(struct xbps_handle *xhp)
{
	prop_object_t obj;
	const char *pkgname, *holdpkgname;
	bool foundhold = false, newpkg_found = false;
	int rv = 0;
	size_t i, x;

	if ((rv = xbps_pkgdb_init(xhp)) != 0)
		return rv;

	for (i = 0; i < prop_array_count(xhp->pkgdb); i++) {
		obj = prop_array_get(xhp->pkgdb, i);
		prop_dictionary_get_cstring_nocopy(obj, "pkgname", &pkgname);
		for (x = 0; x < cfg_size(xhp->cfg, "PackagesOnHold"); x++) {
			holdpkgname = cfg_getnstr(xhp->cfg, "PackagesOnHold", x);
			if (strcmp(pkgname, holdpkgname) == 0) {
				xbps_dbg_printf(xhp, "[rpool] package %s on hold, "
				    "ignoring updates.\n", pkgname);
				foundhold = true;
				break;
			}
		}
		if (foundhold) {
			foundhold = false;
			continue;
		}
		rv = transaction_find_pkg(xhp, pkgname, false, true,
					  false, TRANS_UPDATE);
		if (rv == 0)
			newpkg_found = true;
		else if (rv == ENOENT || rv == EEXIST || rv == ENODEV) {
			/*
			 * missing pkg or installed version is greater than or
			 * equal than pkg in repositories.
			 */
			rv = 0;
		}
	}

	return newpkg_found ? rv : EEXIST;
}

int
xbps_transaction_update_pkg(struct xbps_handle *xhp, const char *pkgname)
{
	return transaction_find_pkg(xhp, pkgname, false,
				    true, false, TRANS_UPDATE);
}

int
xbps_transaction_install_pkg(struct xbps_handle *xhp,
			     const char *pkg,
			     bool reinstall)
{
	prop_dictionary_t pkgd = NULL;
	pkg_state_t state;
	char *pkgname;
	bool bypattern, best, exact;
	int rv;

	if (xbps_pkgpattern_version(pkg)) {
		bypattern = true;
		best = false;
		exact = false;
	} else if ((pkgname = xbps_pkg_name(pkg)) != NULL) {
		exact = true;
		bypattern = false;
		best = false;
	} else {
		exact = false;
		bypattern = false;
		best = true;
	}

	if (exact) {
		pkgd = xbps_pkgdb_get_pkgd(xhp, pkgname, false);
		free(pkgname);
	} else
		pkgd = xbps_pkgdb_get_pkgd(xhp, pkg, bypattern);

	if (pkgd) {
		if (xbps_pkg_state_dictionary(pkgd, &state) != 0)
			return EINVAL;
		if ((state == XBPS_PKG_STATE_INSTALLED) && !reinstall) {
			/* error out if pkg installed and no reinstall */
			return EEXIST;
		}
	}
	rv = transaction_find_pkg(xhp, pkg, bypattern, best, exact, TRANS_INSTALL);
	return rv;
}

int
xbps_transaction_remove_pkg(struct xbps_handle *xhp,
			    const char *pkgname,
			    bool recursive)
{
	prop_dictionary_t pkgd;
	prop_array_t orphans, orphans_pkg, unsorted, reqby;
	prop_object_t obj;
	const char *pkgver;
	size_t count;
	int rv = 0;

	assert(pkgname != NULL);

	if ((pkgd = xbps_pkgdb_get_pkgd(xhp, pkgname, false)) == NULL) {
		/* pkg not installed */
		return ENOENT;
	}
	/*
	 * Prepare transaction dictionary and missing deps array.
	 */
	if ((rv = xbps_transaction_init(xhp)) != 0)
		return rv;

	unsorted = prop_dictionary_get(xhp->transd, "unsorted_deps");
	if (!recursive)
		goto rmpkg;
	/*
	 * If recursive is set, find out which packages would be orphans
	 * if the supplied package were already removed.
	 */
	if ((orphans_pkg = prop_array_create()) == NULL)
		return ENOMEM;

	prop_array_set_cstring_nocopy(orphans_pkg, 0, pkgname);
	orphans = xbps_find_pkg_orphans(xhp, orphans_pkg);
	prop_object_release(orphans_pkg);
	if (prop_object_type(orphans) != PROP_TYPE_ARRAY)
		return EINVAL;

	count = prop_array_count(orphans);
	while (count--) {
		obj = prop_array_get(orphans, count);
		prop_dictionary_get_cstring_nocopy(obj, "pkgver", &pkgver);
		prop_dictionary_set_cstring_nocopy(obj, "transaction", "remove");
		prop_array_add(unsorted, obj);
		xbps_dbg_printf(xhp, "%s: added into transaction (remove).\n", pkgver);
	}
	prop_object_release(orphans);
rmpkg:
	/*
	 * Add pkg dictionary into the unsorted_deps array.
	 */
	prop_dictionary_get_cstring_nocopy(pkgd, "pkgver", &pkgver);
	prop_dictionary_set_cstring_nocopy(pkgd, "transaction", "remove");
	prop_array_add(unsorted, pkgd);
	xbps_dbg_printf(xhp, "%s: added into transaction (remove).\n", pkgver);
	reqby = prop_dictionary_get(pkgd, "requiredby");
	/*
	 * If target pkg is required by any installed pkg, the client must be aware
	 * of this to take appropiate action.
	 */
	if ((prop_object_type(reqby) == PROP_TYPE_ARRAY) &&
	    (prop_array_count(reqby) > 0))
		rv = EEXIST;

	return rv;
}

int
xbps_transaction_autoremove_pkgs(struct xbps_handle *xhp)
{
	prop_array_t orphans, unsorted;
	prop_object_t obj;
	const char *pkgver;
	size_t count;
	int rv = 0;

	orphans = xbps_find_pkg_orphans(xhp, NULL);
	if (prop_object_type(orphans) != PROP_TYPE_ARRAY)
		return EINVAL;

	count = prop_array_count(orphans);
	if (count == 0) {
		/* no orphans? we are done */
		rv = ENOENT;
		goto out;
	}
	/*
	 * Prepare transaction dictionary and missing deps array.
	 */
	if ((rv = xbps_transaction_init(xhp)) != 0)
		goto out;
	/*
	 * Add pkg orphan dictionary into the unsorted_deps array.
	 */
	unsorted = prop_dictionary_get(xhp->transd, "unsorted_deps");
	while (count--) {
		obj = prop_array_get(orphans, count);
		prop_dictionary_get_cstring_nocopy(obj, "pkgver", &pkgver);
		prop_dictionary_set_cstring_nocopy(obj,
		    "transaction", "remove");
		prop_array_add(unsorted, obj);
		xbps_dbg_printf(xhp, "%s: added into transaction (remove).\n",
		    pkgver);
	}
out:
	if (orphans != NULL)
		prop_object_release(orphans);
	return rv;
}
