/*-
 * Copyright (c) 2009-2010 Juan Romero Pardines.
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

#include <xbps_api.h>

/**
 * @file lib/repository_findpkg.c
 * @brief Repository transaction handling routines
 * @defgroup repo_pkgs Repository transaction handling functions
 *
 * The following image shows off the full transaction dictionary returned
 * by xbps_repository_get_transaction_dict().
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

static prop_dictionary_t trans_dict;
static bool trans_dict_initialized;

static int
create_transaction_dictionary(void)
{
	prop_array_t unsorted, missing;
	int rv = 0;

	if (trans_dict_initialized)
		return 0;

	trans_dict = prop_dictionary_create();
	if (trans_dict == NULL)
		return ENOMEM;

	missing = prop_array_create();
	if (missing == NULL) {
		rv = ENOMEM;
		goto fail;
	}

        unsorted = prop_array_create();
        if (unsorted == NULL) {
                rv = ENOMEM;
                goto fail2;
        }

        if (!xbps_add_obj_to_dict(trans_dict, missing, "missing_deps")) {
                rv = EINVAL;
                goto fail3;
        }
        if (!xbps_add_obj_to_dict(trans_dict, unsorted, "unsorted_deps")) {
                rv = EINVAL;
                goto fail3;
        }

	trans_dict_initialized = true;

        return rv;

fail3:
        prop_object_release(unsorted);
fail2:
        prop_object_release(missing);
fail:
        prop_object_release(trans_dict);

        return rv;
}

static int
compute_transaction_sizes(void)
{
	prop_object_iterator_t iter;
	prop_object_t obj;
	uint64_t tsize = 0, dlsize = 0, instsize = 0;
	int rv = 0;
	const char *tract;

	iter = xbps_get_array_iter_from_dict(trans_dict, "packages");
	if (iter == NULL)
		return -1;

	while ((obj = prop_object_iterator_next(iter)) != NULL) {
		if (!prop_dictionary_get_cstring_nocopy(obj,
		    "trans-action", &tract)) {
			rv = -1;
			goto out;
		}
		/*
		 * Skip pkgs that need to be configured.
		 */
		if (strcmp(tract, "configure") == 0)
			continue;

		if (!prop_dictionary_get_uint64(obj,
		    "filename-size", &tsize)) {
			rv = -1;
			goto out;
		}
		dlsize += tsize;
		tsize = 0;
		if (!prop_dictionary_get_uint64(obj,
		    "installed_size", &tsize)) {
			rv = -1;
			goto out;
		}
		instsize += tsize;
		tsize = 0;
	}

	/*
	 * Add object in transaction dictionary with total installed
	 * size that it will take.
	 */
	if (!prop_dictionary_set_uint64(trans_dict,
	    "total-installed-size", instsize)) {
		rv = -1;
		goto out;
	}
	/*
	 * Add object in transaction dictionary with total download
	 * size that needs to be sucked in.
	 */
	if (!prop_dictionary_set_uint64(trans_dict,
	    "total-download-size", dlsize)) {
		rv = -1;
		goto out;
	}
out:
	prop_object_iterator_release(iter);

	return rv;
}

static int
set_pkg_state(prop_dictionary_t pkgd, const char *pkgname)
{
	pkg_state_t state = 0;
	int rv = 0;

	rv = xbps_set_pkg_state_dictionary(pkgd, XBPS_PKG_STATE_NOT_INSTALLED);
	if (rv != 0)
		return rv;
	/*
	 * Overwrite package state in dictionary if it was unpacked
	 * previously.
	 */
	rv = xbps_get_pkg_state_installed(pkgname, &state);
	if (rv == 0) {
		if ((rv = xbps_set_pkg_state_dictionary(pkgd, state)) != 0)
			return rv;
        } else if (rv == ENOENT)
		rv = 0;

	return rv;
}

prop_dictionary_t
xbps_repository_get_transaction_dict(void)
{
	int rv = 0;

	if (trans_dict_initialized == false) {
		errno = ENOENT;
		return NULL;
	}

	/*
	 * Sort package list if necessary.
	 */
	if ((rv = xbps_sort_pkg_deps(trans_dict)) != 0) {
		errno = rv;
		return NULL;
	}

	/*
	 * Add total transaction installed/download sizes
	 * to the transaction dictionary.
	 */
	if (compute_transaction_sizes() != 0)
		return NULL;

	return trans_dict;
}

int
xbps_repository_update_allpkgs(void)
{
	prop_dictionary_t dict;
	prop_object_t obj;
	prop_object_iterator_t iter;
	const char *pkgname;
	int rv = 0;
	bool newpkg_found = false;

	/*
	 * Prepare dictionary with all registered packages.
	 */
	dict = xbps_regpkgs_dictionary_init();
	if (dict == NULL)
		return ENOENT;

	if ((rv = xbps_repository_pool_init()) != 0)
		goto out;

	iter = xbps_get_array_iter_from_dict(dict, "packages");
	if (iter == NULL) {
		rv = EINVAL;
		goto out;
	}

	/*
	 * Find out if there is a newer version for all currently
	 * installed packages.
	 */
	while ((obj = prop_object_iterator_next(iter)) != NULL) {
		if (!prop_dictionary_get_cstring_nocopy(obj,
		    "pkgname", &pkgname)) {
			rv = errno;
			break;
		}
		rv = xbps_repository_update_pkg(pkgname, obj);
		if (rv == ENOENT)
			continue;
		else if (rv == EEXIST) {
			rv = 0;
			continue;
		} else if (rv != 0)
			break;

		newpkg_found = true;
	}
	prop_object_iterator_release(iter);

	if (rv != ENOENT && !newpkg_found)
		rv = ENOPKG;
out:
	xbps_repository_pool_release();
	xbps_regpkgs_dictionary_release();

	return rv;
}

int
xbps_repository_update_pkg(const char *pkgname, prop_dictionary_t instpkg)
{
	prop_dictionary_t pkgrd = NULL;
	prop_array_t unsorted;
	struct repository_pool *rpool;
	const char *repover, *instver;
	int rv = 0;
	bool newpkg_found = false;

	assert(pkgname != NULL);
	assert(instpkg != NULL);

	/*
	 * Prepare repository pool queue.
	 */
	if ((rv = xbps_repository_pool_init()) != 0)
		return rv;

	SIMPLEQ_FOREACH(rpool, &rp_queue, rp_entries) {
		/*
		 * Get the package dictionary from current repository.
		 * If it's not there, pass to the next repository.
		 */
		pkgrd = xbps_find_pkg_in_dict_by_name(rpool->rp_repod,
		    "packages", pkgname);
		if (pkgrd == NULL) {
			if (errno && errno != ENOENT) {
				rv = errno;
				break;
			}
			DPRINTF(("Package %s not found in repo %s.\n",
			    pkgname, rpool->rp_uri));
		} else if (pkgrd != NULL) {
			/*
			 * Check if version in repository is greater than
			 * the version currently installed.
			 */
			if (!prop_dictionary_get_cstring_nocopy(instpkg,
			    "version", &instver)) {
				rv = errno;
				break;
			}
			if (!prop_dictionary_get_cstring_nocopy(pkgrd,
			    "version", &repover)) {
				rv = errno;
				break;
			}
			if (xbps_cmpver(repover, instver) > 0) {
				DPRINTF(("Found %s-%s in repo %s.\n",
				    pkgname, repover, rpool->rp_uri));
				newpkg_found = true;
				break;
			}
			DPRINTF(("Skipping %s-%s in repo %s.\n",
			    pkgname, repover, rpool->rp_uri));
			continue;
		}
	}
	if (!newpkg_found) {
		rv = EEXIST;
		goto out;
	}

	if (pkgrd == NULL) {
		rv = ENOENT;
		goto out;
	}
	/*
	 * Create the transaction dictionary.
	 */
	if ((rv = create_transaction_dictionary()) != 0)
		goto out;

	/*
	 * Set repository in pkg dictionary.
	 */
	if (!prop_dictionary_set_cstring(pkgrd, "repository", rpool->rp_uri)) {
		rv = errno;
		goto out;
	}

	/*
	 * Construct the dependency chain for this package.
	 */
	if ((rv = xbps_repository_find_pkg_deps(trans_dict, pkgrd)) != 0)
		goto out;

	/*
	 * Add required package dictionary into the packages
	 * dictionary.
	 */
	unsorted = prop_dictionary_get(trans_dict, "unsorted_deps");
	if (unsorted == NULL) {
		rv = EINVAL;
		goto out;
	}

	/*
	 * Always set "not-installed" package state. Will be overwritten
	 * to its correct state later.
	 */
	if ((rv = set_pkg_state(pkgrd, pkgname)) != 0)
		goto out;

	if (!prop_dictionary_set_cstring_nocopy(pkgrd,
	    "trans-action", "update")) {
		rv = errno;
		goto out;
	}

	if (!prop_array_add(unsorted, pkgrd))
		rv = errno;

out:
	xbps_repository_pool_release();

	return rv;
}

int
xbps_repository_install_pkg(const char *pkg, bool bypattern)
{
	prop_dictionary_t origin_pkgrd = NULL, pkgrd = NULL;
	prop_array_t unsorted;
	struct repository_pool *rpool;
	const char *pkgname;
	int rv = 0;

	assert(pkg != NULL);

	if ((rv = xbps_repository_pool_init()) != 0)
		return rv;

	SIMPLEQ_FOREACH(rpool, &rp_queue, rp_entries) {
		/*
		 * Get the package dictionary from current repository.
		 * If it's not there, pass to the next repository.
		 */
		if (bypattern)
			pkgrd = xbps_find_pkg_in_dict_by_pattern(
			    rpool->rp_repod, "packages", pkg);
		else
			pkgrd = xbps_find_pkg_in_dict_by_name(
			    rpool->rp_repod, "packages", pkg);

		if (pkgrd == NULL) {
			if (errno && errno != ENOENT) {
				rv = errno;
				goto out;
			}
		} else if (pkgrd != NULL)
			break;
	}
	if (pkgrd == NULL) {
		rv = EAGAIN;
		goto out;
	}

	/*
	 * Create the transaction dictionary.
	 */
	if ((rv = create_transaction_dictionary()) != 0)
		goto out;

	/*
	 * Check that this pkg hasn't been added previously into
	 * the transaction.
	 */
	if (bypattern) {
		if (xbps_find_pkg_in_dict_by_pattern(trans_dict,
		    "unsorted_deps", pkg))
			goto out;
	} else {
		if (xbps_find_pkg_in_dict_by_name(trans_dict,
		    "unsorted_deps", pkg))
			goto out;
	}
	/*
	 * Set repository in pkg dictionary.
	 */
	if (!prop_dictionary_set_cstring(pkgrd, "repository", rpool->rp_uri)) {
		rv = errno;
		goto out;
	}
	origin_pkgrd = prop_dictionary_copy(pkgrd);

	prop_dictionary_get_cstring_nocopy(pkgrd, "pkgname", &pkgname);

	/*
	 * Prepare required package dependencies.
	 */
	if ((rv = xbps_repository_find_pkg_deps(trans_dict, pkgrd)) != 0)
		goto out;

	/*
	 * Add required package dictionary into the unsorted deps dictionary,
	 * set package state as not yet installed.
	 */
	unsorted = prop_dictionary_get(trans_dict, "unsorted_deps");
	if (unsorted == NULL) {
		rv = EINVAL;
		goto out;
	}
	if ((rv = set_pkg_state(origin_pkgrd, pkgname)) != 0)
		goto out;

	if (!prop_dictionary_set_cstring_nocopy(origin_pkgrd,
	    "trans-action", "install")) {
		rv = errno;
		goto out;
	}
	if (!prop_array_add(unsorted, origin_pkgrd))
		rv = errno;

out:
	if (origin_pkgrd)
		prop_object_release(origin_pkgrd);

	xbps_repository_pool_release();

	return rv;
}